#ifndef __LCD_H__
#define __LCD_H__

typedef struct {
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_handle_t panel_handle;
    lv_display_t *display;
    _lock_t lvgl_api_lock;
} lcd_data_t;


int lcd_init(void);
int lvgl_init(void);
void lcd_display_rotate(int dir);


#endif
