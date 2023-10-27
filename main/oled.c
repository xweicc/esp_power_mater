
#include "main.h"

uint8_t GRAM[8][128];   //显存
spi_device_handle_t spi;
const uint8_t hmask[8]={0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
const uint8_t lmask[8]={0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

//屏幕旋转180度
void oled_display_rotate(uint8_t i)
{
    if(i==1){
        oled_write_cmd(0xC8);   //正向
        oled_write_cmd(0xA1);
	}else{
        oled_write_cmd(0xC0);   //旋转
		oled_write_cmd(0xA0);
    }
}

void oled_write_cmd(uint8_t cmd)
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

void oled_write_cmds(uint8_t *cmds, int len)
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

void oled_write_data(uint8_t *data, int len)
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

void oled_show_chinese(uint8_t x,uint8_t y,uint8_t no)
{
    for(int i=0;i<32;i++){
        int bit=y%8;
        int Xi=x+i;
        int Yi=y/8;
        if(i>=16){
            Yi+=1;
            Xi-=16;
        }
        
        if(!bit){
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]=CN[no][i];
            }
        }else{
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]&=~hmask[7-bit];
                GRAM[Yi][Xi]|=CN[no][i]<<bit;
            }
            if(Yi+1<8 && Xi<128){
                GRAM[Yi+1][Xi]&=~lmask[bit-1];
                GRAM[Yi+1][Xi]|=CN[no][i]>>(8-bit);
            }
        }
    }
}

void oled_show_text(uint8_t x,uint8_t y,char *text)
{
    for(int i=0;language[i].en;i++){
        if(!strcmp(language[i].en,text)){
            for(int j=0;j<language[i].len;j++){
                oled_show_chinese(x+j*18, y, language[i].cn[j]);
            }
            break;
        }
    }
}


void oled_show_char_6x8(uint8_t x,uint8_t y,uint8_t chr)
{
    chr-=' ';
    for(int i=0;i<6;i++){
        int bit=y%8;
        int Yi=y/8;
        if(!bit){
            if(Yi<8 && x+i<128){
                GRAM[Yi][x+i]=ascii_6x8[chr][i];
            }
        }else{
            if(Yi<8 && x+i<128){
                GRAM[Yi][x+i]&=~hmask[7-bit];
                GRAM[Yi][x+i]|=ascii_6x8[chr][i]<<bit;
            }
            if(Yi+1<8 && x+i<128){
                GRAM[Yi+1][x+i]&=~lmask[bit-1];
                GRAM[Yi+1][x+i]|=ascii_6x8[chr][i]>>(8-bit);
            }
        }
    }
}

void oled_show_char_8x16(uint8_t x,uint8_t y,uint8_t chr)
{
    chr-=' ';
    for(int i=0;i<16;i++){
        int bit=y%8;
        int Xi=x+i;
        int Yi=y/8;
        if(i>=8){
            Yi+=1;
            Xi-=8;
        }
        
        if(!bit){
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]=ascii_8x16[chr][i];
            }
        }else{
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]&=~hmask[7-bit];
                GRAM[Yi][Xi]|=ascii_8x16[chr][i]<<bit;
            }
            if(Yi+1<8 && Xi<128){
                GRAM[Yi+1][Xi]&=~lmask[bit-1];
                GRAM[Yi+1][Xi]|=ascii_8x16[chr][i]>>(8-bit);
            }
        }
    }
}

void oled_show_char_12x24(uint8_t x,uint8_t y,uint8_t chr)
{
    chr-=' ';
    for(int i=0;i<36;i++){
        int bit=y%8;
        int Xi=x+i;
        int Yi=y/8;
        if(i>=24){
            Yi+=2;
            Xi-=24;
        }else if(i>=12){
            Yi+=1;
            Xi-=12;
        }
        
        if(!bit){
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]=ascii_12x24[chr][i];
            }
        }else{
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]&=~hmask[7-bit];
                GRAM[Yi][Xi]|=ascii_12x24[chr][i]<<bit;
            }
            if(Yi+1<8 && Xi<128){
                GRAM[Yi+1][Xi]&=~lmask[bit-1];
                GRAM[Yi+1][Xi]|=ascii_12x24[chr][i]>>(8-bit);
            }
        }
    }
}

void oled_show_char_16x32(uint8_t x,uint8_t y,uint8_t chr)
{
    chr-=' ';
    for(int i=0;i<64;i++){
        int bit=y%8;
        int Xi=x+i;
        int Yi=y/8;
        if(i>=48){
            Yi+=3;
            Xi-=48;
        }else if(i>=32){
            Yi+=2;
            Xi-=32;
        }else if(i>=16){
            Yi+=1;
            Xi-=16;
        }
        
        if(!bit){
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]=ascii_16x32[chr][i];
            }
        }else{
            if(Yi<8 && Xi<128){
                GRAM[Yi][Xi]&=~hmask[7-bit];
                GRAM[Yi][Xi]|=ascii_16x32[chr][i]<<bit;
            }
            if(Yi+1<8 && Xi<128){
                GRAM[Yi+1][Xi]&=~lmask[bit-1];
                GRAM[Yi+1][Xi]|=ascii_16x32[chr][i]>>(8-bit);
            }
        }
    }
}

void oled_show_char_extend(uint8_t x,uint8_t y,uint8_t chr)
{
    for(int i=0;i<6;i++){
        int bit=y%8;
        int Yi=y/8;
        if(!bit){
            if(Yi<8 && x+i<128){
                GRAM[Yi][x+i]=extend_6x8[chr][i];
            }
        }else{
            if(Yi<8 && x+i<128){
                GRAM[Yi][x+i]&=~hmask[7-bit];
                GRAM[Yi][x+i]|=extend_6x8[chr][i]<<bit;
            }
            if(Yi+1<8 && x+i<128){
                GRAM[Yi+1][x+i]&=~lmask[bit-1];
                GRAM[Yi+1][x+i]|=extend_6x8[chr][i]>>(8-bit);
            }
        }
    }
}


void oled_show_string(uint8_t x,uint8_t y,char *str,uint8_t size)
{
	int i=0;
	while(str[i]){
        switch(size){
            case FontSize_6x8:
                oled_show_char_6x8(x+6*i,y,(uint8_t)str[i]);
                break;
            case FontSize_8x16:
                oled_show_char_8x16(x+8*i,y,(uint8_t)str[i]);
                break;
            case FontSize_12x24:
                oled_show_char_12x24(x+12*i,y,(uint8_t)str[i]);
                break;
            case FontSize_16x32:
                oled_show_char_16x32(x+16*i,y,(uint8_t)str[i]);
                break;
            default:
                break;
        }
        i++;
	}
}

void oled_draw_dot(int x,int y)
{
    if(y/8<8 && x<128){
        GRAM[y/8][x]|=1<<(y%8);
    }
}

void oled_clear_dot(int x,int y)
{
    if(y/8<8 && x<128){
        GRAM[y/8][x]&=~(1<<(y%8));
    }
}


void oled_draw_line(int x1, int y1, int x2, int y2)
{
    unsigned int t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;

    delta_x=x2-x1;
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    
    if(delta_x>0)
        incx=1;
    else if(delta_x==0)
        incx=0;
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    
    if(delta_y>0)
        incy=1;
    else if(delta_y==0)
        incy=0;
    else{
        incy=-1;delta_y=-delta_y;
    }
    if(delta_x>delta_y)
        distance=delta_x;
    else
        distance=delta_y;
    
    for(t=0;t<=distance+1;t++){
        oled_draw_dot(uRow,uCol);
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance){
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance){
            yerr-=distance;
            uCol+=incy;
        }
    }
}

void oled_flush(void)
{
	for(uint8_t i=0;i<8;i++){
		oled_write_cmd(0xb0+i);         //设置页地址(0~7)
		oled_write_cmd(0x00);           //设置显示位置—列低地址
		oled_write_cmd(0x10);           //设置显示位置—列高地址
		oled_write_data(GRAM[i],128);   //传输显示数据
	}
}

void oled_clear(void)
{
    memset(GRAM,0,sizeof(GRAM));
}

//OLED配置
void oled_config(void)
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
    oled_write_cmds(config,sizeof(config));
}

#define PIN_NUM_DC   4
#define PIN_NUM_RST  5

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

int oled_init(void)
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

    oled_config();//配置OLED
    oled_display_rotate(0);

    return 0;
}

void oled_task(void *param)
{
    while(1){
        show_alert();
        oled_flush();
        vTaskDelay(50);
    }
}

