#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <time.h>
#include <math.h>
#include <driver/i2c.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_system.h>
#include <lwip/sys.h>
#include <driver/temperature_sensor.h>
#include <freertos/portmacro.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <lvgl.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <driver/spi_master.h>
#include <esp_timer.h>
#include <esp_lcd_panel_interface.h>
#include <esp_lcd_panel_commands.h>

#include "llist.h"
#include "timer.h"
#include "buzzer.h"
#include "key.h"
#ifdef CONFIG_80V
#include "ina238.h"
#else
#include "ina226.h"
#endif

#ifdef CONFIG_ESC
#include "esc.h"
#endif
#ifdef CONFIG_LCD
#include "lcd.h"
#include "lcd_view.h"
#else
#include "oled.h"
#include "oled_view.h"
#include "font.h"
#endif

#define Printf(format,args...) do{\
        printf("[%s:%d]:"format,__ASSERT_FUNC,__LINE__,##args);\
    }while(0)

#ifdef CONFIG_LCD
#define HDATA_SIZE 50
#define HDATA_DROP 5
#else
#define HDATA_SIZE 30
#define HDATA_DROP 3
#endif
#define HDATA_LV 6
#ifndef ASIZE
#define ASIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

enum{
    languageCN,
    languageEN,
};

typedef void(*view_show)(void);
typedef void(*menu_fun)(void);

typedef struct {
    int raw_mV;
    int raw_mA;
    int mV;
    int mA;
    int mW;
    int max_mA;
    int max_mW;
    int mWH;
    int dir;
} measure_t;

typedef struct {
    int list[HDATA_SIZE];
    int len;
    int div;    //ms
    int num;
    int comp;
}hist_level_t;

typedef struct {
    hist_level_t lv[HDATA_LV];
}hist_data_t;

typedef struct {
    int low_volt;
    int over_curt;
    int over_temp;
    int voice;
    int res;
    int range;
    int main;
    int rotate;
    int contrast;
    int lang;
#ifdef CONFIG_LCD
    int low_volt_off;
    int over_curt_off;
#endif
#ifdef CONFIG_ESC
    int hz;
#endif
} store_t;

typedef struct {
    struct {
        int raw;
        float val;
    } volt[4];  //5V 10V 15V 20V
    struct {
        int raw;
        float val;
    } curt[6];  //1A 2A 5A 10A 20A 50A
    int zero;
    int adc_min;
    int adc_max;
    char reserved[32];
} calibration_t;

typedef struct {
    float temp;
    int menuStart;
    int menuIdx;
    int main_idx;
    int seting;
    int set_idx;
    int cal_idx;
    int hlist_lv;
    store_t store;
    calibration_t cal;
    int cal_point;
    view_show view;
    measure_t msr;
    buzzer_var_t buzzer;
    hist_data_t hist;
    key_trigger_t keys[keyNumMax];
#ifdef CONFIG_ADC_KEY
    adc_cali_handle_t adc1_cali_handle;
    adc_oneshot_unit_handle_t adc1_handle;
    int keyLastId;
#endif
#ifdef CONFIG_ESC
    esc_data_t esc;
#endif
#ifdef CONFIG_LCD
    lcd_data_t lcd;
    int over_curt_off;
#endif
    temperature_sensor_handle_t temp_handle;
    struct timer_list temp_timer;
    struct timer_list key_timer;
    struct timer_list show_timer;
    struct timer_list elec_timer;
    struct timer_list ina_timer;
    struct timer_list oled_timer;
} meter_var_t;

extern meter_var_t mvar;
extern store_t defaults;
extern const int volt_cal_point[];
extern const int curt_cal_point[];
extern const int hz_list[];
extern const int hz_list_len;

void view_show_menu(void);
void show_alert(void);
void save(void);
void save_cal(void);
void save_mWH(void);
void hist_data_update(int lv, int mA);
char *time_str(void);
char *voltage_str(void);
char *current_str(int mA);
char *power_str(void);
char *elec_str(void);
int hz_list_id(int hz);

#endif

