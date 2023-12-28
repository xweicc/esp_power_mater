#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>

typedef int(*oled_init_f)(void);
typedef void (*oled_flush_f)(void);
typedef void (*oled_rotate_f)(int);
typedef void (*oled_contrast_f)(int);

enum{
    contrastLow,
    contrastMid,
    contrastHigh,
};

typedef struct {
    oled_init_f init;
    oled_flush_f flush;
    oled_rotate_f rotate;
    oled_contrast_f contrast;
} oled_driver_t;

extern uint8_t GRAM[8][128];

void oled_show_string(uint8_t x,uint8_t y,char *str,uint8_t size);
void oled_show_chinese(uint8_t x,uint8_t y,uint8_t no);
void oled_show_text(uint8_t x,uint8_t y,char *text);
void oled_clear(void);
void oled_draw_line(int x1, int y1, int x2, int y2);
void oled_draw_dot_line(int x1, int y1, int x2, int y2);
void oled_show_char_extend(uint8_t x,uint8_t y,uint8_t chr);
void oled_show_char_extend_8x16(uint8_t x,uint8_t y,uint8_t chr);
void oled_clear_dot(int x,int y);
void oled_display_rotate(int dir);
void oled_contrast(int i);

int oled_init(void);
void oled_task(void *param);
void oled_timer_fun(unsigned long data);
oled_driver_t *ch1115_new(void);
oled_driver_t *ssd1312_new(void);

#endif  
	 



