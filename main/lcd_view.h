#ifndef __LCD_VIEW_H__
#define __LCD_VIEW_H__

extern const lv_font_t lv_font_cn_16;
extern const lv_font_t lv_font_cn_24;
extern const lv_font_t lv_font_cn_32;
extern const lv_font_t lv_font_cn_40;
extern const lv_image_dsc_t img_curt_curve;
extern const lv_image_dsc_t img_low_volt;
extern const lv_image_dsc_t img_over_curt;
extern const lv_image_dsc_t img_over_temp;
extern const lv_image_dsc_t img_volt_cal;
extern const lv_image_dsc_t img_curt_cal;
extern const lv_image_dsc_t img_zero_cal;
extern const lv_image_dsc_t img_low_volt_off;
extern const lv_image_dsc_t img_out_hz;
extern const lv_image_dsc_t img_def_set;
extern const lv_image_dsc_t img_rotary;
extern const lv_image_dsc_t img_over_curt_off;
extern const lv_image_dsc_t img_voice_set;


void lcd_show_main(void);
void lcd_view_main_init(void);
void lcd_view_main_new(void);
void lcd_view_main_update(void);
void lcd_view_menu_init(void);
void lcd_view_return_main(void);
void lcd_view_key_handler(int key_event);
void lcd_view_menu_update(void);
void lcd_view_alert_init(void);

#endif
