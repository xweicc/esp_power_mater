
#include "main.h"

#define LCD_H_RES      172
#define LCD_V_RES      320
#define LCD_PIXEL_CLOCK_HZ (12 * 1000 * 1000)
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     2
#define LVGL_TASK_MIN_DELAY_MS 200
#define LVGL_TASK_MAX_DELAY_MS 100
#define LCD_HOST  SPI2_HOST


esp_err_t spi_lcd_common_initialize(esp_lcd_panel_io_handle_t *io_handle)
{
    spi_bus_config_t buscfg = {
        .sclk_io_num = GPIO_NUM_10,
        .mosi_io_num = GPIO_NUM_8,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    esp_err_t ret=spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Printf("spi_bus_initialize error:%d\n",ret);
        return ret;
    }

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = GPIO_NUM_7,
        .cs_gpio_num = GPIO_NUM_6,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
    };
    ret=esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, io_handle);
    if (ret != ESP_OK) {
        Printf("esp_lcd_new_panel_io_spi error:%d\n",ret);
        return ret;
    }

    return ESP_OK;
}


int lcd_init(void)
{
    esp_err_t ret=spi_lcd_common_initialize(&mvar.lcd.io_handle);
    if (ret != ESP_OK) {
        Printf("spi_lcd_common_initialize error:%d\n",ret);
        return ret;
    }
    
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ret=esp_lcd_new_panel_st7789(mvar.lcd.io_handle, &panel_config, &mvar.lcd.panel_handle);
    if (ret != ESP_OK) {
        Printf("spi_lcd_common_initialize error:%d\n",ret);
        return ret;
    }
    //mvar.lcd.panel_handle->init = panel_st7789t_init;

    esp_lcd_panel_reset(mvar.lcd.panel_handle);
    esp_lcd_panel_init(mvar.lcd.panel_handle);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_set_gap(mvar.lcd.panel_handle, 0, 34);
    esp_lcd_panel_swap_xy(mvar.lcd.panel_handle, true);
    esp_lcd_panel_invert_color(mvar.lcd.panel_handle, true);
    lcd_display_rotate(mvar.store.rotate);
    // turn on display
    esp_lcd_panel_disp_on_off(mvar.lcd.panel_handle, true);

    return ESP_OK;
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // because SPI LCD is big-endian, we need to swap the RGB bytes order
    lv_draw_sw_rgb565_swap(px_map, (offsetx2 + 1 - offsetx1) * (offsety2 + 1 - offsety1));
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_display_t *disp = (lv_display_t *)user_ctx;
    lv_display_flush_ready(disp);
    return false;
}


static void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

int lvgl_init(void)
{
    lv_init();

    // create a lvgl display
    mvar.lcd.display = lv_display_create(LCD_V_RES, LCD_H_RES);

    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    size_t draw_buffer_sz = (LCD_V_RES * LCD_H_RES * sizeof(lv_color16_t))/5;

    void *buf1 = spi_bus_dma_memory_alloc(LCD_HOST, draw_buffer_sz, 0);
    assert(buf1);
    void *buf2 = spi_bus_dma_memory_alloc(LCD_HOST, draw_buffer_sz, 0);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_display_set_buffers(mvar.lcd.display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
    // associate the mipi panel handle to the display
    lv_display_set_user_data(mvar.lcd.display, mvar.lcd.panel_handle);
    // set color depth
    lv_display_set_color_format(mvar.lcd.display, LV_COLOR_FORMAT_RGB565);
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(mvar.lcd.display, lvgl_flush_cb);

    Printf("Install LVGL tick timer\n");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    Printf("Register io panel event callback for LVGL flush ready notification\n");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lvgl_flush_ready,
    };
    /* Register done callback */
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(mvar.lcd.io_handle, &cbs, mvar.lcd.display));

    Printf("Initialize LVGL OK\n");
    return 0;
}

void lcd_display_rotate(int dir)
{
    if(dir){
        esp_lcd_panel_mirror(mvar.lcd.panel_handle, false, true);
    }else{
        esp_lcd_panel_mirror(mvar.lcd.panel_handle, true, false);
    }
}

