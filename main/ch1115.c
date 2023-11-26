#include "main.h"

static spi_device_handle_t spi;

static void ch1115_write_cmd(uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Len is in bytes, transaction length is in bits.
    t.tx_buffer=&cmd;               //Data
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

static void ch1115_write_cmds(uint8_t *cmds, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=cmds;               //Data
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

static void ch1115_write_data(uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}


#define PIN_NUM_DC   4
#define PIN_NUM_RST  5

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
static void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}


//OLED配置
static void ch1115_config(void)
{
    uint8_t config[]={
        0xAE, //display off
        0x00, //set lower column address
        0x10, //set higher column address
        0xB0, //set page address
        0x40, //set display start lines
        0x81, //contract control
        0x88, //4d
        0x82, //iref resistor set and adjust ISEG
        0x00, 
        0xA1, //set segment remap 0xA0
        0xA2, //set seg pads hardware configuration
        0xA4, //Disable Entire Display On (0xA4/0xA5)
        0xA6, //normal / reverse
        0xA8, //multiplex ratio
        0x3F, //duty = 1/64
        0xC8, //Com scan direction 0XC0
        0xD3, //set display offset
        0x00, // 
        0xD5, //set osc division
        0xa0, 
        0xD9, //set pre-charge period
        0x22, 
        0xdb, //set vcomh
        0x40, 
        0x31, //Set pump 7.4v 
        0xad, //set charge pump enable
        0x8b, //Set DC-DC enable (0x8a=disable; 0x8b=enable) 
        0xAF, //display on
    };
    ch1115_write_cmds(config,sizeof(config));
}


static int ch1115_init(void)
{
    spi_bus_config_t buscfg={
        .miso_io_num=-1,
        .mosi_io_num=7,
        .sclk_io_num=6,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=8*128
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=1*1000*1000,            //Clock out at 1 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=10,                       //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    esp_err_t ret;
    //Initialize the SPI bus
    ret=spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    //Initialize non-SPI GPIOs
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((1ULL<<PIN_NUM_DC) | (1ULL<<PIN_NUM_RST));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(10);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100);

    ch1115_config();//配置OLED

    return 0;
}


void ch1115_flush(void)
{
	for(uint8_t i=0;i<8;i++){
		ch1115_write_cmd(0xb0+i);         //设置页地址(0~7)
		ch1115_write_cmd(0x00);           //设置显示位置—列低地址
		ch1115_write_cmd(0x10);           //设置显示位置—列高地址
		ch1115_write_data(GRAM[i],128);   //传输显示数据
	}
}


//屏幕旋转180度
void ch1115_rotate(int rotate)
{
    if(rotate==1){
        ch1115_write_cmd(0xC8);
        ch1115_write_cmd(0xA1);
	}else{
        ch1115_write_cmd(0xC0);
		ch1115_write_cmd(0xA0);
    }
}

void ch1115_contrast(int contrast)
{
    ch1115_write_cmd(0x81);
    if(contrast==contrastLow){
        ch1115_write_cmd(0x01);
    }else if(contrast==contrastMid){
        ch1115_write_cmd(0x40);
    }else if(contrast==contrastHigh){
        ch1115_write_cmd(0xFF);
    }
}


oled_driver_t *ch1115_new(void)
{
    oled_driver_t *oled=malloc(sizeof(*oled));
    if(oled){
        oled->init=ch1115_init;
        oled->flush=ch1115_flush;
        oled->rotate=ch1115_rotate;
        oled->contrast=ch1115_contrast;
    }

    return oled;
}

