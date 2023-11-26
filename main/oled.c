
#include "main.h"

static oled_driver_t *oled;
uint8_t GRAM[8][128];   //显存
const uint8_t hmask[8]={0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
const uint8_t lmask[8]={0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

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

void oled_draw_dot_line(int x1, int y1, int x2, int y2)
{
    int dot=1;
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
        if(dot){
            dot=0;
            oled_draw_dot(uRow,uCol);
        }else{
            dot=1;
        }
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


void oled_clear(void)
{
    memset(GRAM,0,sizeof(GRAM));
}

void oled_display_rotate(int dir)
{
    oled->rotate(dir);
}

void oled_contrast(int i)
{
    oled->contrast(i);
}

int oled_init(void)
{
#ifdef CONFIG_IDF_TARGET_ESP32C3
    oled=ch1115_new();
#else
    oled=ssd1312_new();
#endif

    if(!oled){
        return -1;
    }

    if(oled->init()){
        return -1;
    }

    oled->rotate(mvar.store.rotate);
    oled->contrast(mvar.store.contrast);
    
    return 0;
}

void oled_run(void)
{
    show_alert();
    oled->flush();
}

void oled_timer_fun(unsigned long data)
{
    oled_run();
    mod_timer(&mvar.oled_timer, jiffies+50);
}

void oled_task(void *param)
{
    while(1){
        oled_run();
        vTaskDelay(50);
    }
}

