
#include "main.h"

meter_var_t mvar;

store_t defaults={
    .low_volt=5000,
    .over_curt=30000,
    .over_temp=50,
    .voice=5,
    .res=2,
    .range=32,
    .main=0,
    .rotate=0,
    .contrast=contrastMid,
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
        snprintf(W,sizeof(W),"%.1f",(double)mvar.msr.mWH/1000);
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

#ifdef CONFIG_IDF_TARGET_ESP32C3
char *temp_str(void)
{
    static char S[16]={0};
    char V[16]={0};
    snprintf(V,sizeof(V),"%d C",(int)mvar.temp);
    snprintf(S,sizeof(S),"%5s",V);
    return S;
}
#endif

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

char *resistor_str(void)
{
    static char S[16]={0};
    char str[16]={0};
    int R=mvar.msr.mV*1000/mvar.msr.mA;
    
    if(R<1000){
        snprintf(str,sizeof(str),"%dm",R);
    }else if(R<10000){
        snprintf(str,sizeof(str),"%.2f",(double)R/1000);
    }else if(R<100000){
        snprintf(str,sizeof(str),"%.1f",(double)R/1000);
    }else if(R<1000000){
        snprintf(str,sizeof(str),"%d",R/1000);
    }else if(R<10000000){
        snprintf(str,sizeof(str),"%.1fK",(double)R/1000000);
    }else if(R<1000000000){
        snprintf(str,sizeof(str),"%dK",R/1000000);
    }
    snprintf(S,sizeof(S),"%4s",str);
    return S;
}

void view_show_main(void)
{
    oled_clear();
    oled_show_string(0, 0, voltage_str(), FontSize_16x32);
    oled_show_string(0, 32, current_str(mvar.msr.mA), FontSize_16x32);
    oled_draw_dot_line(96, 0, 96, 63);

    oled_show_string(97, 1, time_str(), FontSize_6x8);
    oled_draw_dot_line(97, 10, 127, 10);

    char unit[8];
    oled_show_string(97, 12, power_str2(unit), FontSize_6x8);
    oled_show_string(97, 20, unit, FontSize_6x8);
    oled_draw_dot_line(97, 30, 127, 30);

    oled_show_string(97, 33, elec_str2(unit), FontSize_6x8);
    oled_show_string(97, 41, unit, FontSize_6x8);
    oled_draw_dot_line(97, 51, 127, 51);

#ifdef CONFIG_IDF_TARGET_ESP32C3
    oled_show_string(97, 55, temp_str(), FontSize_6x8);
    oled_show_char_extend(115, 55, 0);
#else
    oled_show_string(97, 55, resistor_str(), FontSize_6x8);
    oled_show_char_extend(121, 55, 4);
#endif
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


//量程范围定义
const static struct {
    int max;    //最大量程
    int div;    //每格值
    int base;   //基数
} rangeTab [] = {
    {50,10,1},
    {100,20,1},
    {250,50,1},
    {500,100,1},
    {1000,200,1},
    {2000,400,1},
    {5,1,1000},
    {10,2,1000},
    {15,3,1000},
    {20,4,1000},
    {25,5,1000},
    {30,6,1000},
    {35,7,1000},
    {40,8,1000},
    {45,9,1000},
    {50,10,1000},
    {60,12,1000},
    {70,14,1000}
};

int timeDivList[] = {1,5,10,60,120};

#define xDivNum 26   //X轴格数
#define yDivNum 40   //Y轴格数

void curt_curve_show(void)
{
    int i;
    int div=timeDivList[mvar.timeDiv];
    char S[32]={0};
    
    oled_clear();
    //X轴
    oled_draw_line(21, 51, 127, 51);
    oled_draw_line(124, 49, 127, 51);
    oled_draw_line(124, 53, 127, 51);

    //Y轴
    oled_draw_line(21, 0, 21, 51);
    oled_draw_line(21, 0, 19, 3);
    oled_draw_line(21, 0, 23, 3);

    //X坐标
    for(int i=21;i<124;i+=4){
        oled_draw_line(i, 52, i, 53);
    }
    int div_t,all_t,all;
    char *div_u,*all_u;
    if(div<60){
        div_t=div;
        div_u="s";
    }else{
        div_t=div/60;
        div_u="m";
    }
    all=div*xDivNum;
    if(all<60){
        all_t=all;
        all_u="s";
    }else{
        all_t=all/60;
        all_u="m";
    }
    int len=snprintf(S,sizeof(S),"%d%s/div t:%d%s",div_t,div_u,all_t,all_u);
    
    oled_show_string(127-len*6, 56, S, FontSize_6x8);
    
    int n=0;
    int rid=0;
    int max=0,min=-1;
    int start;
    if(mvar.history.len>xDivNum*div){
        start=mvar.history.len-xDivNum*div;
    }else{
        start=mvar.history.len-mvar.history.len/div*div;
    }

    for(int i=start;i<mvar.history.len;i+=div){
        int val=0;
        for(int j=0;j<div;j++){
            val+=mvar.history.list[i+j];
        }
        val/=div;
        if(val>max){
            max=val;
        }
        if(min==-1 || val<min){
            min=val;
        }
    }

    //量程
    for(int i=0;i<ASIZE(rangeTab);i++){
        if((rangeTab[i].max*rangeTab[i].base)>max){
            rid=i;
            break;
        }
    }
    
    //显示最大最小值
    len=snprintf(S,sizeof(S),"%s",current_str(max));
    oled_show_char_extend(37, 0, 1);
    oled_show_string(43, 0, S, FontSize_6x8);
    oled_show_char_extend(49+len*6, 0, 2);
    snprintf(S,sizeof(S),"%s",current_str(min));
    oled_show_string(55+len*6, 0, S, FontSize_6x8);
    
    //Y坐标
    for(int i=51;i>0 && n<=5;i-=8,n++){
        oled_draw_line(19, i, 20, i);
        if(rangeTab[rid].max>=1000){
            snprintf(S,sizeof(S),"%.1f",(double)(rangeTab[rid].div*n)/1000);
        }else{
            snprintf(S,sizeof(S),"%3d",rangeTab[rid].div*n);
        }
        oled_show_string(0, i-4, S, FontSize_6x8);
    }
    snprintf(S,sizeof(S),"%3s",(rangeTab[rid].base==1 && rangeTab[rid].max<1000)?"mA":"A");
    oled_show_string(0, 55, S, FontSize_6x8);

    int last=0;
    for(i=start,n=0;i<mvar.history.len && n<xDivNum;i+=div,n++){
        int val=0;
        for(int j=0;j<div;j++){
            val+=mvar.history.list[i+j];
        }
        val/=div;
        val=yDivNum-(val*yDivNum/(rangeTab[rid].max*rangeTab[rid].base));
        val+=11;
        if(n==0){
            last=val;
            continue;
        }else{
            oled_draw_line(21+(n-1)*4, last, 21+n*4, val);
            last=val;
        }
    }
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
        && mvar.view!=view_show_main2
        && mvar.view!=curt_curve_show){
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
    if(mW>=3600){
        mvar.msr.mWH+=mW/3600;
        mW%=3600;
        save_mWH();
    }
    mvar.history.list[mvar.history.len++]=mvar.msr.mA;
    if(mvar.history.len==HISTORY_MAX){
        memmove(mvar.history.list,mvar.history.list+HISTORY_DROP,(HISTORY_MAX-HISTORY_DROP)*4);
        mvar.history.len-=HISTORY_DROP;
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

void contrast_set(void)
{
    oled_clear();
    oled_show_text(32, 8, "contrast");
    if(mvar.store.contrast==contrastHigh){
        oled_show_text(56, 30, "high");
    }else if(mvar.store.contrast==contrastMid){
        oled_show_text(56, 30, "mid");
    }else if(mvar.store.contrast==contrastLow){
        oled_show_text(56, 30, "low");
    }
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

#ifdef CONFIG_IDF_TARGET_ESP32C3
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
#endif

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

void key_fun_contrast_set(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.store.contrast>contrastLow){
                mvar.store.contrast--;
            }
            oled_contrast(mvar.store.contrast);
            break;
        case keyRightShort:
            if(mvar.store.contrast<contrastHigh){
                mvar.store.contrast++;
            }
            oled_contrast(mvar.store.contrast);
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
            if(mvar.store.range>32){
                mvar.store.range/=2;
            }
            break;
        case keyRightShort:
            if(mvar.store.range<128){
                mvar.store.range*=2;
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

void key_fun_res_set(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.store.res>1){
                mvar.store.res/=2;
            }
            break;
        case keyRightShort:
            if(mvar.store.res<2){
                mvar.store.res*=2;
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

#ifdef CONFIG_IDF_TARGET_ESP32C3
void over_temp_set(void)
{
    char S[32]={0};
    
    oled_clear();
    oled_show_text(32, 8, "over_temp");
    snprintf(S,sizeof(S),"[ %d ]",mvar.store.over_temp);
    oled_show_string(44, 26, S, FontSize_8x16);
}
#endif

void range_set(void)
{
    char S[32]={0};
    
    oled_clear();
    oled_show_text(32, 8, "range_set");
    snprintf(S,sizeof(S),"[ %dA ]",mvar.store.range);
    oled_show_string(40, 26, S, FontSize_8x16);
}

void res_set(void)
{
    char S[32]={0};
    
    oled_clear();
    oled_show_text(32, 8, "res_set");
    snprintf(S,sizeof(S),"[ %dm  ]",mvar.store.res);
    oled_show_string(40, 26, S, FontSize_8x16);
    oled_show_char_extend_8x16(72, 26, 0);
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

const int volt_cal_point[]={5000,10000,15000,20000};    //mV
const int curt_cal_point[]={1000,2000,5000,10000,20000,50000};  //mA

void volt_cal(void)
{
    char S[32]={0};
    
    oled_clear();
    
    oled_show_text(20, 10, "cal_dot");
    snprintf(S,sizeof(S),"<%dV>",volt_cal_point[mvar.cal_point]/1000);
    oled_show_string(76, 10, S, FontSize_8x16);
    
    oled_show_text(20, 30, "voltage");
    oled_show_string(60, 30, voltage_str(), FontSize_8x16);

    float cal=mvar.cal.volt[mvar.cal_point].val;
    if(cal>1.0){
        snprintf(S,sizeof(S),"+%f",cal-1.0);
    }else{
        snprintf(S,sizeof(S),"-%f",1.0-cal);
    }
    oled_show_string(40, 50, S, FontSize_6x8);
}


void curt_cal(void)
{
    char S[32]={0};
    
    oled_clear();
    
    oled_show_text(20, 10, "cal_dot");
    snprintf(S,sizeof(S),"<%dA>",curt_cal_point[mvar.cal_point]/1000);
    oled_show_string(76, 10, S, FontSize_8x16);
    
    oled_show_text(20, 30, "current");
    oled_show_string(60, 30, current_str(mvar.msr.mA), FontSize_8x16);

    float cal=mvar.cal.curt[mvar.cal_point].val;
    if(cal>1.0){
        snprintf(S,sizeof(S),"+%f",cal-1.0);
    }else{
        snprintf(S,sizeof(S),"-%f",1.0-cal);
    }
    oled_show_string(40, 50, S, FontSize_6x8);
}

void zero_cal(void)
{
    char S[32]={0};
    
    oled_clear();

    oled_show_text(32, 8, "zero_cal");
    snprintf(S,sizeof(S),"<%d>",mvar.cal.zero);
    oled_show_string(48, 26, S, FontSize_8x16);
    oled_show_string(48, 42, current_str(mvar.msr.raw_mA), FontSize_8x16);
}


static struct {
    char *name;
    menu_fun fun;
}menus[]={
    {"curt_curve",curt_curve_show},
    {"low_volt",low_volt_set},
    {"over_curt",over_curt_set},
#ifdef CONFIG_IDF_TARGET_ESP32C3
    {"over_temp",over_temp_set},
    {"range_set",range_set},
    {"res_set",res_set},
#endif
    {"voice_set",voice_set},
    {"contrast",contrast_set},
    {"curt_cal",curt_cal},
    {"volt_cal",volt_cal},
    {"zero_cal",zero_cal},
    {"def_set",def_set},
};


void key_fun_volt_cal(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.cal_point>0){
                mvar.cal_point--;
            }
            break;
        case keyRightShort:
            if(mvar.cal_point<ASIZE(mvar.cal.volt)-1){
                mvar.cal_point++;
            }
            break;
        case keySetShort:
            float cal=(float)volt_cal_point[mvar.cal_point]/(float)mvar.msr.raw_mV;
            if(cal>0.8 && cal<1.2){
                mvar.cal.volt[mvar.cal_point].raw=mvar.msr.raw_mV;
                mvar.cal.volt[mvar.cal_point].val=cal;
            }
            break;
        case keySetLong:
            save_cal();
            mvar.view=view_show_menu;
            break;
        default :
            break;
    }
}

void key_fun_curt_cal(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.cal_point>0){
                mvar.cal_point--;
            }
            break;
        case keyRightShort:
            if(mvar.cal_point<ASIZE(mvar.cal.curt)-1){
                mvar.cal_point++;
            }
            break;
        case keySetShort:
            float cal=(float)curt_cal_point[mvar.cal_point]/(float)mvar.msr.raw_mA;
            if(cal>0.6 && cal<1.4){
                mvar.cal.curt[mvar.cal_point].raw=mvar.msr.raw_mA;
                mvar.cal.curt[mvar.cal_point].val=cal;
            }
            break;
        case keySetLong:
            save_cal();
            mvar.view=view_show_menu;
            break;
        default :
            break;
    }
}

void key_fun_zero_cal(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.cal.zero>-50){
                mvar.cal.zero--;
            }
            break;
        case keyRightShort:
            if(mvar.cal.zero<50){
                mvar.cal.zero++;
            }
            break;
        case keySetShort:
        case keySetLong:
            save_cal();
            mvar.view=view_show_menu;
            break;
        default :
            break;
    }
}


void key_fun_curt_curve(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.timeDiv>0){
                mvar.timeDiv--;
            }
            break;
        case keyRightShort:
            if(mvar.timeDiv<ASIZE(timeDivList)-1){
                mvar.timeDiv++;
            }
            break;
        case keySetShort:
        case keySetLong:
            mvar.view=view_show_menu;
            break;
        default :
            break;
    }
}

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
            if(mvar.store.main<ASIZE(main_views)-1){
                mvar.store.main++;
            }else{
                mvar.store.main=0;
            }
            mvar.view=main_views[mvar.store.main];
            save();
            break;
        case keyLeftLong:
            mvar.msr.mWH=0;
            save_mWH();
            break;
        case keyRightShort:
            mvar.store.rotate=!mvar.store.rotate;
            oled_display_rotate(mvar.store.rotate);
            save();
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
    }else if(mvar.view==curt_curve_show){
        return key_fun_curt_curve(event);
    }else if(mvar.view==low_volt_set){
        return key_fun_low_volt(event);
    }else if(mvar.view==over_curt_set){
        return key_fun_over_curt(event);
    }
#ifdef CONFIG_IDF_TARGET_ESP32C3
    else if(mvar.view==over_temp_set){
        return key_fun_over_temp(event);
    }
#endif
    else if(mvar.view==voice_set){
        return key_fun_voice_set(event);
    }else if(mvar.view==contrast_set){
        return key_fun_contrast_set(event);
    }else if(mvar.view==range_set){
        return key_fun_range_set(event);
    }else if(mvar.view==res_set){
        return key_fun_res_set(event);
    }else if(mvar.view==volt_cal){
        return key_fun_volt_cal(event);
    }else if(mvar.view==curt_cal){
        return key_fun_curt_cal(event);
    }else if(mvar.view==zero_cal){
        return key_fun_zero_cal(event);
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
#ifdef CONFIG_IDF_TARGET_ESP32C3
    temp_init();
#endif

    buzzer_init();
    buzzer_set(buzzerShort);
    
    ina226_init();
    oled_init();
    
#ifdef CONFIG_IDF_TARGET_ESP32C3
	xTaskCreate(ina226_task, "ina226_task", 2048, NULL, 5, NULL);
	xTaskCreate(oled_task, "oled_task", 2048, NULL, 3, NULL);
#else
    setup_timer(&mvar.ina226_timer, ina226_timer_fun, 0);
    mod_timer(&mvar.ina226_timer, jiffies+50);
    setup_timer(&mvar.oled_timer, oled_timer_fun, 0);
    mod_timer(&mvar.oled_timer, jiffies+50);
#endif

    key_register_fun(keyNumSet, key_handler_fun);
    key_register_fun(keyNumLeft, key_handler_fun);
    key_register_fun(keyNumRight, key_handler_fun);

    setup_timer(&mvar.show_timer, show_timer_fun, 0);
    mod_timer(&mvar.show_timer, jiffies+100);

    setup_timer(&mvar.elec_timer, elec_timer_fun, 0);
    mod_timer(&mvar.elec_timer, jiffies+HZ);
    
    mvar.view=main_views[mvar.store.main];
    while(1){
        run_timers();
        vTaskDelay(1);
    }
}

