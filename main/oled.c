
#include "oled.h"
#include "oledfont.h"  	
#include "main.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
void delay_ms(unsigned int ms)
{                         
	vTaskDelay(ms);
}

//反显函数
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

spi_device_handle_t spi;
void OLED_WR_Byte(u8 dat,int cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=1*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=&dat;               //Data
    t.user=(void*)cmd;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
} 

//坐标设置

void OLED_Set_Pos(u8 x, u8 y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63				 
//sizey:选择字体 6x8 8x16
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey)
{      	
	int c=0,sizex=sizey/2;
	int i=0,size1;
	if(sizey==8)size1=6;
	else size1=(sizey/8+((sizey%8)?1:0))*(sizey/2);
	c=chr-' ';//得到偏移后的值
	OLED_Set_Pos(x,y);
	for(i=0;i<size1;i++)
	{
		if(i%sizex==0&&sizey!=8) OLED_Set_Pos(x,y++);
		if(sizey==8) OLED_WR_Byte(asc2_0806[c][i],OLED_DATA);//6X8字号
		else if(sizey==16) OLED_WR_Byte(asc2_1608[c][i],OLED_DATA);//8x16字号
//		else if(sizey==xx) OLED_WR_Byte(asc2_xxxx[c][i],OLED_DATA);//用户添加字号
		else return;
	}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示数字
//x,y :起点坐标
//num:要显示的数字
//len :数字的位数
//sizey:字体大小		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey)
{         	
	u8 t,temp,m=0;
	u8 enshow=0;
	if(sizey==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(sizey/2+m)*t,y,' ',sizey);
				continue;
			}else enshow=1;
		}
	 	OLED_ShowChar(x+(sizey/2+m)*t,y,temp+'0',sizey);
	}
}
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 sizey)
{
	int j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j++],sizey);
		if(sizey==8)x+=6;
		else x+=sizey/2;
	}
}
//显示汉字
void OLED_ShowChinese(u8 x,u8 y,u8 no,u8 sizey)
{
	int i,size1=(sizey/8+((sizey%8)?1:0))*sizey;
	for(i=0;i<size1;i++)
	{
		if(i%sizey==0) OLED_Set_Pos(x,y++);
		if(sizey==16) OLED_WR_Byte(CN[no][i],OLED_DATA);//16x16字号
//		else if(sizey==xx) OLED_WR_Byte(xxx[c][i],OLED_DATA);//用户添加字号
		else return;
	}				
}


//显示图片
//x,y显示坐标
//sizex,sizey,图片长宽
//BMP：要显示的图片
void OLED_DrawBMP(u8 x,u8 y,u8 sizex, u8 sizey,u8 BMP[])
{ 	
  u16 j=0;
	u8 i,m;
	sizey=sizey/8+((sizey%8)?1:0);
	for(i=0;i<sizey;i++)
	{
		OLED_Set_Pos(x,i+y);
    for(m=0;m<sizex;m++)
		{      
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
} 



//初始化				    
void OLED_Init(void)
{
    OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/ 
    OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/ 
    OLED_WR_Byte(0x10,OLED_CMD); /*set higher column address*/
    OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/ 
    OLED_WR_Byte(0x40,OLED_CMD); /*set display start lines*/ 
    OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/ 
    OLED_WR_Byte(0x88,OLED_CMD); /*4d*/ 
    OLED_WR_Byte(0x82,OLED_CMD); /* iref resistor set and adjust ISEG*/ 
    OLED_WR_Byte(0x00,OLED_CMD); 
    OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap 0xA0*/ 
    OLED_WR_Byte(0xA2,OLED_CMD); /*set seg pads hardware configuration*/ 
    OLED_WR_Byte(0xA4,OLED_CMD); /*Disable Entire Display On (0xA4/0xA5)*/ 
    OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/ 
    OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/ 
    OLED_WR_Byte(0x3F,OLED_CMD); /*duty = 1/64*/ 
    OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction 0XC0*/ 
    OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
    OLED_WR_Byte(0x00,OLED_CMD); /* */ 
    OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/ 
    OLED_WR_Byte(0xa0,OLED_CMD); 
    OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/ 
    OLED_WR_Byte(0x22,OLED_CMD); 
    OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/ 
    OLED_WR_Byte(0x40,OLED_CMD); 
    OLED_WR_Byte(0x31,OLED_CMD); /* Set pump 7.4v */ 
    OLED_WR_Byte(0xad,OLED_CMD); /*set charge pump enable*/ 
    OLED_WR_Byte(0x8b,OLED_CMD); /*Set DC-DC enable (0x8a=disable; 0x8b=enable) */ 
    OLED_Clear();
    OLED_WR_Byte(0xAF,OLED_CMD); /*display on*/
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

#define PARALLEL_LINES 16
    spi_bus_config_t buscfg={
        .miso_io_num=-1,
        .mosi_io_num=7,
        .sclk_io_num=6,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=PARALLEL_LINES*320*2+8
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
    vTaskDelay(1);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100);

    Printf("init start\n");
    OLED_Init();//初始化OLED
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(1);//0正常显示 1 屏幕翻转显示
    Printf("init ok\n");
    return 0;
}

extern int mV,mA,mW;
void oled_task(void *param)
{
    while(1){
        OLED_ShowChinese(0,0,0,16);
        OLED_ShowChinese(20,0,1,16);
        OLED_ShowChinese(0,3,0,16);
        OLED_ShowChinese(20,3,2,16);
        OLED_ShowChinese(0,6,3,16);
        OLED_ShowChinese(20,6,4,16);

        char A[32]={0},V[32]={0},W[32]={0};
        if(mA>1000){
            snprintf(A,sizeof(A),":%.2fA  ",(double)mA/1000);
        } else {
            snprintf(A,sizeof(A),":%dmA  ",mA);
        }
        snprintf(V,sizeof(V),":%.2fV  ",(double)mV/1000);
        if(mW>1000){
            snprintf(W,sizeof(W),":%.2fW  ",(double)mW/1000);
        } else {
            snprintf(W,sizeof(W),":%dmW   ",mW);
        }

        OLED_ShowString(40,0,(u8 *)A,16);
        OLED_ShowString(40,3,(u8 *)V,16);
        OLED_ShowString(40,6,(u8 *)W,16);
        vTaskDelay(100);
    }
}

