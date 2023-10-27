
#include "main.h"

meter_var_t mvar;

store_t defaults={
    .low_volt=5000,
    .over_curt=30000,
    .over_temp=50,
    .voice=5,
    .res=2,
    .range=32,
};

char *time_str(void)
{
    time_t now;
    struct tm tm;
    static char str[16]={0};
    
    time(&now);
    localtime_r(&now, &tm);
    snprintf(str, sizeof(str), "%02d:%02d", tm.tm_min, tm.tm_sec);

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
    if(mA>=1000){
        snprintf(S,sizeof(S),"%.2fA",(double)mA/1000);
    } else {
        snprintf(S,sizeof(S),"%dmA",mA);
    }
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

char *power_str2(char *unit)
{
    static char S[16]={0};
    char W[16]={0};
    if(mvar.msr.mW>=100000){
        snprintf(W,sizeof(W),"%.1f",(double)mvar.msr.mW/1000);
        sprintf(unit,"%5s","W");
    } else if(mvar.msr.mW>=1000){
        snprintf(W,sizeof(W),"%.2f",(double)mvar.msr.mW/1000);
        sprintf(unit,"%5s","W");
    } else {
        snprintf(W,sizeof(W),"%d",mvar.msr.mW);
        sprintf(unit,"%5s","mW");
    }
    snprintf(S,sizeof(S),"%5s",W);
    return S;
}

char *elec_str2(char *unit)
{
    static char S[16]={0};
    char W[16]={0};
    if(mvar.msr.mWH>=100000){
        snprintf(W,sizeof(W),"%.1f",(double)mvar.msr.mW/1000);
        sprintf(unit,"%5s","WH");
    } else if(mvar.msr.mWH>=1000){
        snprintf(W,sizeof(W),"%.2f",(double)mvar.msr.mWH/1000);
        sprintf(unit,"%5s","WH");
    } else {
        snprintf(W,sizeof(W),"%d",mvar.msr.mWH);
        sprintf(unit,"%5s","mWH");
    }
    snprintf(S,sizeof(S),"%5s",W);
    return S;
}

char *temp_str(void)
{
    static char S[16]={0};
    char V[16]={0};
    snprintf(V,sizeof(V),"%d C",(int)mvar.temp);
    snprintf(S,sizeof(S),"%5s",V);
    return S;
}

void show_box(int x1, int y1, int x2, int y2)
{
    oled_draw_line(x1, y1, x2, y1);
    oled_draw_line(x1, y2, x2, y2);
    oled_draw_line(x1, y1, x1, y2);
    oled_draw_line(x2, y1, x2, y2);
    
    oled_draw_line(x1+1, y1+1, x2-1, y1+1);
    oled_draw_line(x1+1, y2-1, x2-1, y2-1);
    oled_draw_line(x1+1, y1+1, x1, y2-1);
    oled_draw_line(x2-1, y1+1, x2-1, y2-1);
    for(int x=x1+2;x<x2-1;x++){
        for(int y=y1+2;y<y2-1;y++){
            oled_clear_dot(x, y);
        }
    }
}

void view_show_main(void)
{
    oled_clear();
    oled_show_string(0, 0, voltage_str(), FontSize_16x32);
    oled_show_string(0, 32, current_str(mvar.msr.mA), FontSize_16x32);
    oled_draw_line(96, 0, 96, 63);

    oled_show_string(97, 1, time_str(), FontSize_6x8);
    oled_draw_line(97, 10, 127, 10);

    char unit[8];
    oled_show_string(97, 12, power_str2(unit), FontSize_6x8);
    oled_show_string(97, 20, unit, FontSize_6x8);
    oled_draw_line(97, 30, 127, 30);

    oled_show_string(97, 33, elec_str2(unit), FontSize_6x8);
    oled_show_string(97, 41, unit, FontSize_6x8);
    oled_draw_line(97, 51, 127, 51);

    oled_show_string(97, 55, temp_str(), FontSize_6x8);
    oled_show_char_extend(115, 55, 0);
}

void view_show_main2(void)
{
    oled_clear();
    oled_show_text(0, 5, "voltage");
    oled_show_text(0, 25, "current");
    oled_show_text(0, 45, "power");
    oled_show_string(34, 5, ":", FontSize_8x16);
    oled_show_string(34, 25, ":", FontSize_8x16);
    oled_show_string(34, 45, ":", FontSize_8x16);
    oled_show_string(42, 40, power_str(), FontSize_12x24);
    oled_show_string(42, 20, current_str(mvar.msr.mA), FontSize_12x24);
    oled_show_string(42, 0, voltage_str(), FontSize_12x24);
}

void show_alert(void)
{
    char *type=NULL;
    static int alert=0;

    if(mvar.msr.mV < mvar.store.low_volt){
        type="low_volt";
    }else if(mvar.msr.mA > mvar.store.over_curt){
        type="over_curt";
    }else if(mvar.temp > mvar.store.over_temp){
        type="over_temp";
    }

    if(!type){
        return ;
    }

    if(mvar.view!=view_show_main
        && mvar.view!=view_show_main2){
        return ;
    }
    
    if(alert<3){
        alert++;
        show_box(24,16,112,48);
        oled_show_text(32, 24, type);
        buzzer_set(buzzerShort);
    }else if(alert<6){
        alert++;
    }else{
        alert=0;
    }
}

static view_show main_views[]={
    view_show_main,
    view_show_main2
};

//电量统计，1秒执行一次
void elec_timer_fun(unsigned long data)
{
    static int mW=0;
    mW+=mvar.msr.mW;
    if(mW>3600){
        mvar.msr.mWH+=mW/3600;
        mW%=3600;
    }
    mvar.history.list[mvar.history.len++]=mvar.msr.mA;
    if(mvar.history.len==HISTORY_MAX){
        memmove(mvar.history.list,mvar.history.list+HISTORY_DROP,(HISTORY_MAX-HISTORY_DROP)*4);
        mvar.history.len-=HISTORY_DROP;
    }
    mod_timer(&mvar.elec_timer, jiffies+HZ);
}

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

void low_volt_set(void)
{
    char S[32]={0};
    int x=0;
    
    oled_clear();
    oled_show_text(32, 8, "low_volt");
    if(mvar.store.low_volt>=10000){
        snprintf(S,sizeof(S),"[%.2fV]",(double)mvar.store.low_volt/1000);
    }else{
        snprintf(S,sizeof(S),"[0%.2fV]",(double)mvar.store.low_volt/1000);
    }
    if(mvar.set_idx>=2){
        x=mvar.set_idx+2;
    }else{
        x=mvar.set_idx+1;
    }
    if(mvar.seting){
        static int blink=0;
        if(blink){
            S[x]=' ';
        }
        blink=!blink;
    }
    oled_show_string(40, 26, S, FontSize_8x16);
    oled_draw_line(40+x*8, 41, 40+x*8+7, 41);
}


void voice_set(void)
{
    char S[32]={0};
    
    oled_clear();
    oled_show_text(32, 8, "voice_set");
    snprintf(S,sizeof(S),"[ %d ]",mvar.store.voice);
    oled_show_string(48, 26, S, FontSize_8x16);
}

void key_fun_low_volt(int event)
{
    switch(event){
        case keyLeftShort:
            if(!mvar.seting){
                if(mvar.set_idx>0){
                    mvar.set_idx--;
                }
            }else{
                int dec=pow(10,4-mvar.set_idx);
                if(mvar.store.low_volt>=dec){
                    mvar.store.low_volt-=dec;
                }
            }
            break;
        case keyRightShort:
            if(!mvar.seting){
                if(mvar.set_idx<3){
                    mvar.set_idx++;
                }
            }else{
                int dec=pow(10,4-mvar.set_idx);
                if(mvar.store.low_volt<30000){
                    mvar.store.low_volt+=dec;
                }
            }
            break;
        case keySetShort:
            mvar.seting=!mvar.seting;
            break;
        case keySetLong:
            mvar.view=view_show_menu;
            save();
            break;
        default:
            break;
    }
}

void key_fun_over_curt(int event)
{
    switch(event){
        case keyLeftShort:
            if(!mvar.seting){
                if(mvar.set_idx>0){
                    mvar.set_idx--;
                }
            }else{
                int dec=pow(10,4-mvar.set_idx);
                if(mvar.store.over_curt>=dec){
                    mvar.store.over_curt-=dec;
                }
            }
            break;
        case keyRightShort:
            if(!mvar.seting){
                if(mvar.set_idx<3){
                    mvar.set_idx++;
                }
            }else{
                int dec=pow(10,4-mvar.set_idx);
                if(mvar.store.over_curt<64000){
                    mvar.store.over_curt+=dec;
                }
            }
            break;
        case keySetShort:
            mvar.seting=!mvar.seting;
            break;
        case keySetLong:
            mvar.view=view_show_menu;
            save();
            break;
        default:
            break;
    }
}

void key_fun_over_temp(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.store.over_temp>20){
                mvar.store.over_temp--;
            }
            break;
        case keyRightShort:
            if(mvar.store.over_temp<80){
                mvar.store.over_temp++;
            }
            break;
        case keySetShort:
        case keySetLong:
            mvar.view=view_show_menu;
            save();
            break;
        default:
            break;
    }
}


void key_fun_voice_set(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.store.voice>0){
                mvar.store.voice--;
            }
            break;
        case keyRightShort:
            if(mvar.store.voice<10){
                mvar.store.voice++;
            }
            break;
        case keySetShort:
        case keySetLong:
            mvar.view=view_show_menu;
            save();
            break;
        default:
            break;
    }
}

void key_fun_range_set(int event)
{
    switch(event){
        case keyLeftShort:
        case keyRightShort:
            if(mvar.store.res==1){
                if(mvar.store.range==32){
                    mvar.store.range=64;
                }else{
                    mvar.store.range=32;
                }
            }else if(mvar.store.res==2){
                if(mvar.store.range==32){
                    mvar.store.range=64;
                }else{
                    mvar.store.range=32;
                }
            }
            break;
        case keySetShort:
        case keySetLong:
            mvar.view=view_show_menu;
            save();
            ina226_set_cal();
            break;
        default:
            break;
    }
}

void over_curt_set(void)
{
    char S[32]={0};
    int x=0;
    
    oled_clear();
    oled_show_text(32, 8, "over_curt");
    if(mvar.store.over_curt>=10000){
        snprintf(S,sizeof(S),"[%.2fA]",(double)mvar.store.over_curt/1000);
    }else{
        snprintf(S,sizeof(S),"[0%.2fA]",(double)mvar.store.over_curt/1000);
    }
    if(mvar.set_idx>=2){
        x=mvar.set_idx+2;
    }else{
        x=mvar.set_idx+1;
    }
    if(mvar.seting){
        static int blink=0;
        if(blink){
            S[x]=' ';
        }
        blink=!blink;
    }
    oled_show_string(40, 26, S, FontSize_8x16);
    oled_draw_line(40+x*8, 41, 40+x*8+7, 41);
}


void over_temp_set(void)
{
    char S[32]={0};
    
    oled_clear();
    oled_show_text(32, 8, "over_temp");
    snprintf(S,sizeof(S),"[ %d ]",mvar.store.over_temp);
    oled_show_string(48, 26, S, FontSize_8x16);
}


void range_set(void)
{
    char S[32]={0};
    
    oled_clear();
    oled_show_text(32, 8, "range_set");
    snprintf(S,sizeof(S),"[ %dA ]",mvar.store.range);
    oled_show_string(48, 26, S, FontSize_8x16);
}

void def_set(void)
{
    oled_clear();
    oled_show_text(8, 24, "def_ok");
    mvar.store=defaults;
    save();
    vTaskDelay(500);
    mvar.view=view_show_menu;
}

static struct {
    char *name;
    menu_fun fun;
}menus[]={
    {"low_volt",low_volt_set},
    {"over_curt",over_curt_set},
    {"over_temp",over_temp_set},
    {"range_set",range_set},
    {"voice_set",voice_set},
    {"def_set",def_set},
};



void view_show_menu(void)
{
    oled_clear();
    for(int i=0;i<4;i++){
        oled_show_text(0, i*16, menus[i+mvar.menuStart].name);
    }
    int row=mvar.menuIdx-mvar.menuStart;
    for(int i=0;i<128;i++){
        GRAM[row*2][i]=~GRAM[row*2][i];
        GRAM[row*2+1][i]=~GRAM[row*2+1][i];
    }
}


void key_fun_menu(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.menuIdx>0){
                mvar.menuIdx--;
                if(mvar.menuIdx<mvar.menuStart){
                    mvar.menuStart=mvar.menuIdx;
                }
            }
            break;
        case keyRightShort:
            if(mvar.menuIdx<ASIZE(menus)-1){
                mvar.menuIdx++;
                Printf("mvar.menuIdx:%d\n",mvar.menuIdx);
                if(mvar.menuIdx>mvar.menuStart+3){
                    mvar.menuStart=mvar.menuIdx-3;
                    Printf("mvar.menuStart:%d\n",mvar.menuStart);
                }
            }
            break;
        case keySetShort:
            mvar.seting=0;
            mvar.set_idx=0;
            mvar.cal_point=0;
            mvar.view=menus[mvar.menuIdx].fun;
            break;
        case keySetLong:
            mvar.view=main_views[0];
            break;
        default :
            break;
    }
}


void key_fun_main(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.main_idx<ASIZE(main_views)-1){
                mvar.main_idx++;
            }else{
                mvar.main_idx=0;
            }
            mvar.view=main_views[mvar.main_idx];
            break;
        case keyRightShort:
            mvar.disp_dir=!mvar.disp_dir;
            oled_display_rotate(mvar.disp_dir);
            break;
        case keySetShort:
            mvar.menuIdx=0;
            mvar.menuStart=0;
            mvar.view=view_show_menu;
            break;
        default :
            break;
    }
}


void key_handler_fun(int event)
{
    if(mvar.view==view_show_main || mvar.view==view_show_main2){
        return key_fun_main(event);
    }else if(mvar.view==view_show_menu){
        return key_fun_menu(event);
    }else if(mvar.view==low_volt_set){
        return key_fun_low_volt(event);
    }else if(mvar.view==over_curt_set){
        return key_fun_over_curt(event);
    }else if(mvar.view==over_temp_set){
        return key_fun_over_temp(event);
    }else if(mvar.view==voice_set){
        return key_fun_voice_set(event);
    }else if(mvar.view==range_set){
        return key_fun_range_set(event);
    }
}


void show_timer_fun(unsigned long data)
{
    mvar.view();
    mod_timer(&mvar.show_timer, jiffies+100);
}

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

void save(void)
{
    nvram_set_data("store", &mvar.store, sizeof(mvar.store));
}

void app_main(void)
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
    
    init_timers_cpu();
    key_init();
    temp_init();

    buzzer_init();
    buzzer_set(buzzerShort);
    
    ina226_init();
	xTaskCreate(ina226_task, "ina226_task", 2048, NULL, 5, NULL);

    oled_init();
	xTaskCreate(oled_task, "oled_task", 2048, NULL, 3, NULL);

    key_register_fun(keyNumSet, key_handler_fun);
    key_register_fun(keyNumLeft, key_handler_fun);
    key_register_fun(keyNumRight, key_handler_fun);

    setup_timer(&mvar.show_timer, show_timer_fun, 0);
    mod_timer(&mvar.show_timer, jiffies+100);

    setup_timer(&mvar.elec_timer, elec_timer_fun, 0);
    mod_timer(&mvar.elec_timer, jiffies+HZ);
    
    mvar.view=main_views[0];
    while(1){
        run_timers();
        vTaskDelay(1);
    }
}

