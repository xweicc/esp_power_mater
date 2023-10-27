#ifndef __OLED_H
#define __OLED_H

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#include <stdint.h>

extern uint8_t GRAM[8][128];

//OLED控制用函数
void oled_display_rotate(uint8_t i);
void oled_show_string(uint8_t x,uint8_t y,char *str,uint8_t size);
void oled_show_chinese(uint8_t x,uint8_t y,uint8_t no);
void oled_write_cmd(uint8_t cmd);
void oled_show_text(uint8_t x,uint8_t y,char *text);
void oled_clear(void);
void oled_draw_line(int x1, int y1, int x2, int y2);
void oled_show_char_extend(uint8_t x,uint8_t y,uint8_t chr);
void oled_clear_dot(int x,int y);

int oled_init(void);
void oled_task(void *param);

#endif  
	 



