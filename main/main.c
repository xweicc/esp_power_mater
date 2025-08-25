
#include "main.h"

meter_var_t mvar;

store_t defaults={
    .low_volt=5000,
    .over_curt=30000,
    .over_temp=80,
    .voice=5,
#ifdef CONFIG_IDF_TARGET_ESP32C3
    .res=1,
#else
    .res=2,
#endif
#ifdef CONFIG_LCD
    .range=64,
#else
    .range=32,
#endif
    .main=0,
    .rotate=0,
    .lang=languageCN,
#ifdef CONFIG_LCD
    .low_volt_off=5000,
    .over_curt_off=50000,
#else
    .contrast=contrastMid,
#endif
#ifdef CONFIG_ESC
    .hz=1000,
#endif
};

#ifdef CONFIG_ESC
const int hz_list[]={100,200,500,1000,2000,5000,10000,20000,30000,40000,50000};
const int hz_list_len = ASIZE(hz_list);
int hz_list_id(int hz)
{
    for(int i=0;i<ASIZE(hz_list);i++){
        if(hz_list[i]==hz){
            return i;
        }
    }
    return -1;
}
#endif

const int volt_cal_point[]={5000,10000,15000,20000};    //mV
const int curt_cal_point[]={1000,2000,5000,10000,20000,50000};  //mA

//电量统计，1秒执行一次
void elec_timer_fun(unsigned long data)
{
    static int mW=0;
    mW+=mvar.msr.mW;
    if(mW>=3600){
        mvar.msr.mWH+=mW/3600;
        mW%=3600;
        save_mWH();
    }
    mod_timer(&mvar.elec_timer, jiffies+HZ);
}

#ifdef CONFIG_IDF_TARGET_ESP32C3
void temp_timer_fun(unsigned long data)
{
    temperature_sensor_get_celsius(mvar.temp_handle, &mvar.temp);
    
    mod_timer(&mvar.temp_timer, jiffies+HZ);
}

void temp_init(void)
{
    temperature_sensor_config_t temp_sensor = TEMPERATURE_SENSOR_CONFIG_DEFAULT(0, 80);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &mvar.temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_enable(mvar.temp_handle));

    setup_timer(&mvar.temp_timer, temp_timer_fun, 0);
    mod_timer(&mvar.temp_timer, jiffies+HZ);
}
#endif

int nvram_set_data(char *name, void *data, int len)
{
	int ret=-1;
	nvs_handle_t nvram;
	esp_err_t err=nvs_open("nvram", NVS_READWRITE, &nvram);
	if(err!=ESP_OK){
		Printf("nvs_open:%s(%X)\n", esp_err_to_name(err),err);
		return -1;
	}

	err=nvs_set_blob(nvram, name, data, len);
	if(err!=ESP_OK){
		Printf("nvs_set_blob %s:%s(%X)\n", name,esp_err_to_name(err),err);
		goto out;
	}
	
	err=nvs_commit(nvram);
	if(err!=ESP_OK){
		Printf("nvs_commit:%s(%X)\n", esp_err_to_name(err),err);
		goto out;
	}
	ret=0;
	
out:
	nvs_close(nvram);
	return ret;
}

int nvram_get_data(char *name, void *data, int *size)
{
	int ret=-1;
	nvs_handle_t nvram;
	esp_err_t err=nvs_open("nvram", NVS_READONLY, &nvram);
	if(err!=ESP_OK){
		Printf("nvs_open:%s(%X)\n", esp_err_to_name(err),err);
		return -1;
	}
	err=nvs_get_blob(nvram, name, data, (size_t *)size);
	if(err!=ESP_OK){
		Printf("nvs_get_blob %s:%s(%X)\n", name,esp_err_to_name(err),err);
		goto out;
	}
	ret=0;
	
out:
	nvs_close(nvram);
	return ret;
}


int nvram_set_int(char *name, int val)
{
	int ret=-1;
	nvs_handle_t nvram;
	esp_err_t err=nvs_open("nvram", NVS_READWRITE, &nvram);
	if(err!=ERR_OK){
		Printf("nvs_open:%s(%X)\n",esp_err_to_name(err),err);
		return -1;
	}

	err=nvs_set_i32(nvram, name, val);
	if(err!=ERR_OK){
		Printf("nvs_set_i32 %s:%s(%X)\n",name,esp_err_to_name(err),err);
		goto out;
	}
	
	err=nvs_commit(nvram);
	if(err!=ERR_OK){
		Printf("nvs_commit:%s(%X)\n",esp_err_to_name(err),err);
		goto out;
	}
	ret=0;
	
out:
	nvs_close(nvram);
	return ret;
}

int nvram_get_int(char *name, int *val)
{
	int ret=-1;
	nvs_handle_t nvram;
	esp_err_t err=nvs_open("nvram", NVS_READONLY, &nvram);
	if(err!=ERR_OK){
		Printf("nvs_open:%s(%X)\n",esp_err_to_name(err),err);
		return -1;
	}
	err=nvs_get_i32(nvram, name, (int32_t *)val);
	if(err!=ERR_OK){
		goto out;
	}
	ret=0;
	
out:
	nvs_close(nvram);
	return ret;
}

void save(void)
{
    nvram_set_data("store", &mvar.store, sizeof(mvar.store));
}

void save_cal(void)
{
    nvram_set_data("cal", &mvar.cal, sizeof(mvar.cal));
}

void save_mWH(void)
{
    nvram_set_int("mWH", mvar.msr.mWH);
}


void hist_data_init(void)
{
    hist_data_t *data=&mvar.hist;

    data->lv[0].div=50; //50ms
    data->lv[0].comp=20;

    data->lv[1].div=1000;   //1S
    data->lv[1].comp=5;

    data->lv[2].div=5000;  //5S
    data->lv[2].comp=2;

    data->lv[3].div=10000;  //10S
    data->lv[3].comp=6;

    data->lv[4].div=60000;  //60S
    data->lv[4].comp=2;

    data->lv[5].div=120000;  //120S
}

void hist_data_update(int lv, int mA)
{
    hist_data_t *data=&mvar.hist;
    hist_level_t *lv_t=&data->lv[lv];
    
    lv_t->list[lv_t->len++]=mA;
    lv_t->num++;
    if(lv_t->comp && lv_t->num==lv_t->comp){
        int sum=0;
        for(int i=lv_t->len-lv_t->comp;i<lv_t->len;i++){
            sum+=lv_t->list[i];
        }
        sum/=lv_t->comp;
        lv_t->num=0;
        hist_data_update(lv+1, sum);
    }
    
    if(lv_t->len==HDATA_SIZE){
        lv_t->len-=HDATA_DROP;
        memmove(lv_t->list,lv_t->list+HDATA_DROP,(HDATA_SIZE-HDATA_DROP)*4);
    }
}

void key_handler_fun(int event)
{
    #ifdef CONFIG_LCD
    lcd_view_key_handler(event);
    #else
    oled_view_key_handler(event);
    #endif
}


void show_timer_fun(unsigned long data)
{
#ifdef CONFIG_LCD
    lcd_view_main_update();
#else
    mvar.view();
#endif
    mod_timer(&mvar.show_timer, jiffies+50);
}


char *time_str(void)
{
    time_t now;
    struct tm tm;
    static char str[16]={0};
    
    time(&now);
    localtime_r(&now, &tm);
    #ifdef CONFIG_LCD
    snprintf(str, sizeof(str), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    #else
    snprintf(str, sizeof(str), "%02d:%02d", tm.tm_min, tm.tm_sec);
    #endif

    return str;
}

char *voltage_str(void)
{
    static char S[16]={0};
    snprintf(S,sizeof(S),"%.2fV",(double)mvar.msr.mV/1000);
    return S;
}

char *current_str(int mA)
{
    static char S[16]={0};
#ifdef CONFIG_LCD
    if(mA>=1000){
        snprintf(S,sizeof(S),"%s%.2fA",mvar.msr.dir==-1?"-":"",(double)mA/1000);
    } else {
        snprintf(S,sizeof(S),"%s%dmA",mvar.msr.dir==-1?"-":"",mA);
    }
#else
    if(mA>=1000){
        snprintf(S,sizeof(S),"%.2fA",(double)mA/1000);
    } else {
        snprintf(S,sizeof(S),"%dmA",mA);
    }
#endif
    return S;
}

char *power_str(void)
{
    static char S[16]={0};
    if(mvar.msr.mW>=100000){
        snprintf(S,sizeof(S),"%.1fW",(double)mvar.msr.mW/1000);
    } else if(mvar.msr.mW>=1000){
        snprintf(S,sizeof(S),"%.2fW",(double)mvar.msr.mW/1000);
    } else {
        snprintf(S,sizeof(S),"%dmW",mvar.msr.mW);
    }
    return S;
}


char *elec_str(void)
{
    static char W[16]={0};
    if(mvar.msr.mWH>=1000000){
        snprintf(W,sizeof(W),"%dWH",mvar.msr.mWH/1000);
    } else if(mvar.msr.mWH>=100000){
        snprintf(W,sizeof(W),"%.1fWH",(double)mvar.msr.mWH/1000);
    } else if(mvar.msr.mWH>=1000){
        snprintf(W,sizeof(W),"%.2fWH",(double)mvar.msr.mWH/1000);
    } else {
        snprintf(W,sizeof(W),"%dmWH",mvar.msr.mWH);
    }
    return W;
}

void app_main(void *arg)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    struct timeval tv={0};
	tv.tv_sec=0;
	settimeofday(&tv, NULL);

    int size=sizeof(mvar.store);
    if(nvram_get_data("store", &mvar.store, &size)!=ESP_OK
        || size!=sizeof(mvar.store)){
        Printf("set defaults\n");
        mvar.store=defaults;
    }

    size=sizeof(mvar.cal);
    if(nvram_get_data("cal", &mvar.cal, &size)!=ESP_OK
        || size!=sizeof(mvar.cal)){
        for(int i=0;i<ASIZE(mvar.cal.volt);i++){
            mvar.cal.volt[i].val=1.0;
        }
        for(int i=0;i<ASIZE(mvar.cal.curt);i++){
            mvar.cal.curt[i].val=1.0;
        }
    }

    nvram_get_int("mWH",&mvar.msr.mWH);
    
    init_timers_cpu();
    key_init();
#ifdef CONFIG_ESC
    esc_init();
#endif
#ifdef CONFIG_IDF_TARGET_ESP32C3
    temp_init();
#endif

    buzzer_init();
    buzzer_set(buzzerShort);

#ifdef CONFIG_80V
    ina238_init();
#else
    ina226_init();
#endif
#ifdef CONFIG_LCD
    lcd_init();
    lvgl_init();
#else
    oled_init();
#endif
    hist_data_init();
    
#ifdef CONFIG_IDF_TARGET_ESP32C3
	xTaskCreate(
	    #ifdef CONFIG_80V
        ina238_task, "ina238_task"
        #else
	    ina226_task, "ina226_task"
	    #endif
	    , 2048, NULL, 5, NULL);
    #ifndef CONFIG_LCD
	xTaskCreate(oled_task, "oled_task", 2048, NULL, 3, NULL);
    #endif
#else
    setup_timer(&mvar.ina_timer, ina226_timer_fun, 0);
    mod_timer(&mvar.ina_timer, jiffies+50);
    setup_timer(&mvar.oled_timer, oled_timer_fun, 0);
    mod_timer(&mvar.oled_timer, jiffies+50);
#endif

    key_register_fun(keyNumSet, key_handler_fun);
    key_register_fun(keyNumLeft, key_handler_fun);
    key_register_fun(keyNumRight, key_handler_fun);

#ifdef CONFIG_LCD
    lcd_view_main_init();
    lcd_view_menu_init();
    lcd_view_alert_init();
#else
    oled_view_init();
#endif
    setup_timer(&mvar.show_timer, show_timer_fun, 0);
    mod_timer(&mvar.show_timer, jiffies+100);

    setup_timer(&mvar.elec_timer, elec_timer_fun, 0);
    mod_timer(&mvar.elec_timer, jiffies+HZ);
    
    while(1){
        run_timers();
        #ifdef CONFIG_LCD
        lv_timer_handler();
        #endif
        vTaskDelay(1);
    }
}
