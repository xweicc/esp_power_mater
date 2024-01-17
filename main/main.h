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

#include "llist.h"
#include "timer.h"
#include "buzzer.h"
#include "key.h"
#include "ina226.h"
#include "oled.h"
#include "font.h"

#define Printf(format,args...) do{\
        printf("[%s:%d]:"format,__ASSERT_FUNC,__LINE__,##args);\
    }while(0)

#define HISTORY_MAX 3600
#define HISTORY_DROP 360

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
} measure_t;

typedef struct {
    int list[HISTORY_MAX];
    int len;
} history_t;

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
    char reserved[32];
} calibration_t;

typedef struct {
    float temp;
    int menuStart;
    int menuIdx;
    int main_idx;
    int timeDiv;
    int seting;
    int set_idx;
    int cal_idx;
    store_t store;
    calibration_t cal;
    int cal_point;
    view_show view;
    measure_t msr;
    buzzer_var_t buzzer;
    history_t history;
    key_trigger_t keys[keyNumMax];
    temperature_sensor_handle_t temp_handle;
    struct timer_list temp_timer;
    struct timer_list key_timer;
    struct timer_list show_timer;
    struct timer_list elec_timer;
    struct timer_list ina226_timer;
    struct timer_list oled_timer;
} meter_var_t;

extern meter_var_t mvar;
extern const int volt_cal_point[];
extern const int curt_cal_point[];

void view_show_menu(void);
void show_alert(void);
void save(void);
void save_cal(void);
void save_mWH(void);

#endif

