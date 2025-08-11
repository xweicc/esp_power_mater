#include "main.h"
#include "lvgl.h"
#include "lcd_view.h"

struct main_view_t{
    lv_obj_t *scr;
    lv_obj_t *voltage;
    lv_obj_t *current;
    lv_obj_t *power;
    lv_obj_t *info;
    lv_obj_t *time;
    lv_obj_t *power_stat;
    lv_obj_t *temp;
    lv_obj_t *duty;
    lv_style_t txt_style1;
    lv_style_t txt_style2;
    lv_style_t value_style;
} main_view;

struct menu_view_t{
    lv_obj_t *scr;
} menu_view;

void main_txt_style1(lv_style_t *style)
{
    lv_style_init(style);
    lv_style_set_radius(style, 6);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_bg_color(style, lv_color_hex(0x333333));
    lv_style_set_text_color(style, lv_color_hex(0xffffff));
    if(mvar.store.lang==languageCN){
        lv_style_set_text_font(style, &lv_font_cn_16);
    }else{
        lv_style_set_text_font(style, &lv_font_montserrat_24);
    }
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_pad_all(style, 4);
}

void main_txt_style2(lv_style_t *style)
{
    lv_style_init(style);
    lv_style_set_radius(style, 6);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_bg_color(style, lv_color_hex(0x283c63));
    lv_style_set_text_color(style, lv_color_hex(0xffffff));
    lv_style_set_text_font(style, &lv_font_cn_16);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_size(style, lv_pct(24), lv_pct(24));
    lv_style_set_pad_all(style, 2);
}

void main_value_style(lv_style_t *style)
{
    lv_style_init(style);
    lv_style_set_radius(style, 8);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_text_color(style, lv_color_hex(0xffffff));
    lv_style_set_text_font(style, &lv_font_montserrat_48);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_size(style, lv_pct(66), lv_pct(32));
}

void lcd_view_main_init(void)
{
    //创建一个屏幕
    main_view.scr = lv_obj_create(NULL);
    lv_obj_set_size(main_view.scr, lv_pct(100), lv_pct(100));
    lv_obj_center(main_view.scr);

    //设置背景色
    lv_obj_set_style_bg_color(main_view.scr, lv_color_hex(0x624E88), LV_PART_MAIN);

    // 将创建的屏幕设置为活动屏幕
    lv_screen_load(main_view.scr);

    // 初始化样式
    main_txt_style1(&main_view.txt_style1);
    main_txt_style2(&main_view.txt_style2);
    main_value_style(&main_view.value_style);

    //创建一个标签，显示电压
    lv_obj_t *label = lv_label_create(main_view.scr);
    lv_obj_add_style(label, &main_view.txt_style1, 0);
    if(mvar.store.lang==languageCN){
        lv_label_set_text(label, "电\n压");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, lv_pct(0), lv_pct(4));
    }else{
        lv_label_set_text(label, "U");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, lv_pct(0), lv_pct(6));
    }

    //创建一个标签，显示电流
    lv_obj_t *label2 = lv_label_create(main_view.scr);
    lv_obj_add_style(label2, &main_view.txt_style1, 0);
    if(mvar.store.lang==languageCN){
        lv_label_set_text(label2, "电\n流");
        lv_obj_align(label2, LV_ALIGN_TOP_LEFT, lv_pct(0), lv_pct(38));
    }else{
        lv_label_set_text(label2, " I ");
        lv_obj_align(label2, LV_ALIGN_TOP_LEFT, lv_pct(0), lv_pct(40));
    }

    //创建一个标签，显示功率
    lv_obj_t *label3 = lv_label_create(main_view.scr);
    lv_obj_add_style(label3, &main_view.txt_style1, 0);
    if(mvar.store.lang==languageCN){
        lv_label_set_text(label3, "功\n率");
        lv_obj_align(label3, LV_ALIGN_TOP_LEFT, lv_pct(0), lv_pct(71));
    }else{
        lv_label_set_text(label3, "P");
        lv_obj_align(label3, LV_ALIGN_TOP_LEFT, lv_pct(0), lv_pct(73));
    }

    //创建一个标签，显示电压值
    main_view.voltage = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.voltage, &main_view.value_style, 0);
    lv_label_set_text(main_view.voltage, "00.00V");
    lv_obj_align(main_view.voltage, LV_ALIGN_TOP_LEFT, lv_pct(9), lv_pct(0));
    lv_obj_set_style_bg_color(main_view.voltage, lv_color_hex(0xff5470), LV_PART_MAIN);

    //创建一个标签，显示电流值
    main_view.current = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.current, &main_view.value_style, 0);
    lv_label_set_text(main_view.current, "00.00A");
    lv_obj_align(main_view.current, LV_ALIGN_TOP_LEFT, lv_pct(9), lv_pct(34));
    lv_obj_set_style_bg_color(main_view.current, lv_color_hex(0xF0A04B), LV_PART_MAIN);

    //创建一个标签，显示功率值
    main_view.power = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.power, &main_view.value_style, 0);
    lv_label_set_text(main_view.power, "00.00W");
    lv_obj_align(main_view.power, LV_ALIGN_TOP_LEFT, lv_pct(9), lv_pct(68));
    lv_obj_set_style_bg_color(main_view.power, lv_color_hex(0x00adb5), LV_PART_MAIN);

    //创建一个标签，显示时间
    main_view.time = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.time, &main_view.txt_style2, 0);
    lv_label_set_text(main_view.time, "\n00:00");
    lv_obj_align(main_view.time, LV_ALIGN_TOP_LEFT, lv_pct(76), lv_pct(0));

    //创建一个标签，显示电量统计
    main_view.power_stat = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.power_stat, &main_view.txt_style2, 0);
    lv_label_set_text(main_view.power_stat, "\n0000wh");
    lv_obj_align(main_view.power_stat, LV_ALIGN_TOP_LEFT, lv_pct(76), lv_pct(25));

    //创建一个标签，显示核心温度
    main_view.temp = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.temp, &main_view.txt_style2, 0);
    lv_label_set_text(main_view.temp, "\n00℃");
    lv_obj_align(main_view.temp, LV_ALIGN_TOP_LEFT, lv_pct(76), lv_pct(50));

    //创建一个标签，显示占空比
    main_view.duty = lv_label_create(main_view.scr);
    lv_obj_add_style(main_view.duty, &main_view.txt_style2, 0);
#ifdef CONFIG_ESC
    lv_label_set_text(main_view.duty, "\n00%");
#else
    lv_label_set_text(main_view.duty, " ");
#endif
    lv_obj_align(main_view.duty, LV_ALIGN_TOP_LEFT, lv_pct(76), lv_pct(75));
}


#define VIEW_MENU
#ifdef VIEW_MENU
void menu_current_curve(void)
{
    static lv_obj_t *scr =NULL, *chart = NULL;
    static lv_chart_series_t *ser1 = NULL;
    static lv_obj_t *Ydiv[6] = {NULL};
    static lv_obj_t *Ylabel = NULL;
    static lv_obj_t *Xlabel = NULL;
    static lv_obj_t *Maxlabel = NULL;
    static lv_obj_t *Minlabel = NULL;

    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);
        lv_obj_set_style_bg_color(scr, lv_color_hex(0xcbd2df), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

        /*Create a chart*/
        chart = lv_chart_create(scr);
        lv_obj_set_size(chart, lv_pct(92), lv_pct(90));
        lv_obj_center(chart);
        lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(chart, 100);
        lv_obj_set_style_width(chart, 0, LV_PART_INDICATOR);
        lv_obj_set_style_height(chart, 0, LV_PART_INDICATOR);
        lv_obj_align(chart, LV_ALIGN_TOP_LEFT, lv_pct(8), lv_pct(0));
        lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);  
        lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);

        lv_chart_set_div_line_count(chart, 6, 11);
        lv_obj_set_style_line_color(chart, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

        for(int i = 0; i < 6; i++){
            Ydiv[i] = lv_label_create(scr);
            lv_obj_set_style_text_font(Ydiv[i], &lv_font_montserrat_16, LV_PART_MAIN);
            lv_obj_set_size(Ydiv[i], lv_pct(10), lv_pct(10));
            lv_obj_set_style_text_align(Ydiv[i], LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
            lv_label_set_text(Ydiv[i], "0");
            lv_obj_set_style_text_color(Ydiv[i], lv_color_hex(0x52616b), LV_PART_MAIN);
            lv_obj_align(Ydiv[i], LV_ALIGN_TOP_LEFT, lv_pct(1), lv_pct(i * 16));
        }
        Ylabel = lv_label_create(scr);
        lv_obj_set_style_text_font(Ylabel, &lv_font_montserrat_16, LV_PART_MAIN);
        lv_obj_set_size(Ylabel, lv_pct(10), lv_pct(10));
        lv_obj_set_style_text_align(Ylabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
        lv_label_set_text(Ylabel, "mA");
        lv_obj_set_style_text_color(Ylabel, lv_color_hex(0x52616b), LV_PART_MAIN);
        lv_obj_align(Ylabel, LV_ALIGN_TOP_LEFT, lv_pct(2), lv_pct(89));

        Maxlabel = lv_label_create(scr);
        lv_obj_set_style_text_font(Maxlabel, &lv_font_montserrat_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(Maxlabel, lv_color_hex(0xfc5185), LV_PART_MAIN);
        lv_obj_align(Maxlabel, LV_ALIGN_TOP_LEFT, lv_pct(15), lv_pct(90));

        Minlabel = lv_label_create(scr);
        lv_obj_set_style_text_font(Minlabel, &lv_font_montserrat_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(Minlabel, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_align(Minlabel, LV_ALIGN_TOP_LEFT, lv_pct(50), lv_pct(90));

        Xlabel = lv_label_create(scr);
        lv_obj_set_style_text_font(Xlabel, &lv_font_montserrat_16, LV_PART_MAIN);
        lv_label_set_text(Xlabel, "1S/Div");
        lv_obj_set_style_text_color(Xlabel, lv_color_hex(0x52616b), LV_PART_MAIN);
        lv_obj_align(Xlabel, LV_ALIGN_TOP_LEFT, lv_pct(81), lv_pct(90));

        /*Add data series*/
        ser1 = lv_chart_add_series(chart, lv_color_hex(0xF0A04B), LV_CHART_AXIS_PRIMARY_Y);
    }
    
    lv_screen_load(scr);
    lv_chart_set_next_value(chart, ser1, mvar.msr.mA);

    int max=0,min=0;
    int32_t *y_points = lv_chart_get_series_y_array(chart, ser1);
    for(int i=0;i<100;i++){
        if(y_points[i] == LV_CHART_POINT_NONE) {
            continue;
        }
        if(y_points[i]>max){
            max=y_points[i];
        }
        if(min == 0 || y_points[i]<min){
            min=y_points[i];
        }
    }

    int ranges[] = {50,100,250,500,1000,2000,5000,10000,15000,20000,30000,40000,50000,60000,70000};
    int range=0;
    for(int i=0;i<ASIZE(ranges);i++){
        if(ranges[i]>max){
            range=ranges[i];
            break;
        }
    }

    for(int i=0;i<5;i++){
        int val=range-i*(range/5);
        if(range<1000){
            lv_label_set_text_fmt(Ydiv[i], "%d", val);
        }else if(range<5000){
            char S[8]={0};
            snprintf(S,sizeof(S),"%.1f",(double)val/1000);
            lv_label_set_text_fmt(Ydiv[i], "%s",S);
        }else{
            lv_label_set_text_fmt(Ydiv[i], "%d", val/1000);
        }
    }

    if(range<1000){
        lv_label_set_text(Ylabel, "mA");
    }else{
        lv_label_set_text(Ylabel, "A");
    }

    lv_label_set_text_fmt(Maxlabel, "Max:%s", current_str(max));
    lv_label_set_text_fmt(Minlabel, "Min:%s", current_str(min));

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, range);
    lv_chart_refresh(chart);
}



void update_setting_value(lv_obj_t *obj, int value, int index, char *unit)
{
    int digits[4];
    digits[0] = (value / 10000) % 10;
    digits[1] = (value / 1000) % 10;
    digits[2] = (value / 100) % 10;
    digits[3] = (value / 10) % 10;

    char fmt[64] = "";
    for(int i=0; i<4; i++) {
        if(i == index) {
            char tmp[16];
            if(mvar.seting){
                static int blink=0;
                if(blink){
                    snprintf(tmp, sizeof(tmp), "#ff0000 %d#", digits[i]);
                }else{
                    snprintf(tmp, sizeof(tmp), "%d", digits[i]);
                }
                blink=!blink;
            }else{
                snprintf(tmp, sizeof(tmp), "#ff0000 %d#", digits[i]);
            }
            strcat(fmt, tmp);
        } else {
            char tmp[4];
            snprintf(tmp, sizeof(tmp), "%d", digits[i]);
            strcat(fmt, tmp);
        }
        if(i==1) strcat(fmt, ".");
    }
    strcat(fmt, unit);

    lv_label_set_text(obj, fmt);
    lv_label_set_recolor(obj, true);
}

void menu_low_volt(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置低压报警");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set low-voltage alarm");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "低于此设置电压时, 蜂鸣器会报警");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "When the voltage is lower than this setting\nthe buzzer will sound an alarm");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    update_setting_value(value, mvar.store.low_volt, mvar.set_idx, "V");
}


void menu_over_curt(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置过流报警");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set overcurrent alarm");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "高于此设置电流时, 蜂鸣器会报警");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "When the current exceeds this set value\nthe buzzer will sound an alarm");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    update_setting_value(value, mvar.store.over_curt, mvar.set_idx, "A");
}

void menu_volt_cal(void)
{
    static lv_obj_t *scr, *value, *value2, *value3;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置电压校准");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set voltage calibration");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_22, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, lv_pct(-15));
        lv_obj_set_style_text_color(value, lv_color_hex(0xe23e57), LV_PART_MAIN);

        value2 = lv_label_create(scr);
        lv_obj_set_style_text_font(value2, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value2, LV_ALIGN_CENTER, 0, lv_pct(2));
        lv_obj_set_style_text_color(value2, lv_color_hex(0x46cdcf), LV_PART_MAIN);

        value3 = lv_label_create(scr);
        lv_obj_set_style_text_font(value3, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value3, LV_ALIGN_CENTER, 0, lv_pct(19));
        lv_obj_set_style_text_color(value3, lv_color_hex(0xf08a5d), LV_PART_MAIN);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "并联标准电压表, 调整负载\n使电压达到校准点, 然后按Set键保存");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "Connect a standard voltage meter in parallel\nadjust the load to reach the calibration point,\nthen press the Set key to save");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_10, LV_PART_MAIN);
        }   
        lv_obj_set_style_text_color(label, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(40));
    }

    lv_screen_load(scr);
    if(mvar.store.lang==languageCN){
        lv_label_set_text_fmt(value, "校准点:<%dV>", volt_cal_point[mvar.cal_point]/1000);
        lv_label_set_text_fmt(value2, "当前电压:%s", voltage_str());
    }else{
        lv_label_set_text_fmt(value, "Point:<%dV>", volt_cal_point[mvar.cal_point]/1000);
        lv_label_set_text_fmt(value2, "Current:%s", voltage_str());
    }
    float cal=mvar.cal.volt[mvar.cal_point].val;
    char S[32]={0};
    if(cal>1.0){
        snprintf(S,sizeof(S),"+%f",cal-1.0);
    }else{
        snprintf(S,sizeof(S),"-%f",1.0-cal);
    }
    if(mvar.store.lang==languageCN){
        lv_label_set_text_fmt(value3, "校准偏移值:%s", S);
    }else{
        lv_label_set_text_fmt(value3, "Offset:%s", S);
    }
}


void menu_curt_cal(void)
{
    static lv_obj_t *scr, *value, *value2, *value3;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置电流校准");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set current calibration");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_22, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, lv_pct(-15));
        lv_obj_set_style_text_color(value, lv_color_hex(0xe23e57), LV_PART_MAIN);

        value2 = lv_label_create(scr);
        lv_obj_set_style_text_font(value2, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value2, LV_ALIGN_CENTER, 0, lv_pct(2));
        lv_obj_set_style_text_color(value2, lv_color_hex(0x46cdcf), LV_PART_MAIN);

        value3 = lv_label_create(scr);
        lv_obj_set_style_text_font(value3, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value3, LV_ALIGN_CENTER, 0, lv_pct(19));
        lv_obj_set_style_text_color(value3, lv_color_hex(0xf08a5d), LV_PART_MAIN);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "串联标准电流表, 调整负载\n使电流达到校准点, 然后按Set键保存");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "Connect a standard current meter in series\nadjust the load to reach the calibration point,\nthen press the Set key to save");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_10, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(40));
    }

    lv_screen_load(scr);
    if(mvar.store.lang==languageCN){
        lv_label_set_text_fmt(value, "校准点:<%dA>", curt_cal_point[mvar.cal_point]/1000);
        lv_label_set_text_fmt(value2, "当前电流:%s", current_str(mvar.msr.mA));
    }else{
        lv_label_set_text_fmt(value, "Point:<%dA>", curt_cal_point[mvar.cal_point]/1000);
        lv_label_set_text_fmt(value2, "Current:%s", current_str(mvar.msr.mA));
    }
    float cal=mvar.cal.curt[mvar.cal_point].val;
    char S[32]={0};
    if(cal>1.0){
        snprintf(S,sizeof(S),"+%f",cal-1.0);
    }else{
        snprintf(S,sizeof(S),"-%f",1.0-cal);
    }
    if(mvar.store.lang==languageCN){
        lv_label_set_text_fmt(value3, "校准偏移值:%s", S);
    }else{
        lv_label_set_text_fmt(value3, "Offset:%s", S);
    }
}


void menu_zero_cal(void)
{
    static lv_obj_t *scr, *value, *value2;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置零点校准");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set zero calibration");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_22, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, lv_pct(-10));
        lv_obj_set_style_text_color(value, lv_color_hex(0xe23e57), LV_PART_MAIN);

        value2 = lv_label_create(scr);
        lv_obj_set_style_text_font(value2, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value2, LV_ALIGN_CENTER, 0, lv_pct(10));
        lv_obj_set_style_text_color(value2, lv_color_hex(0x46cdcf), LV_PART_MAIN);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "断开负载, 左右调节偏移量\n使校准后电流为0, 然后按Set键保存");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "Disconnect the load, adjust the offset\nso that the current after calibration is 0,\nthen press the Set key to save");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_10, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(40));
    }

    lv_screen_load(scr);
    if(mvar.store.lang==languageCN){
        lv_label_set_text_fmt(value, "偏移量:<%d>", mvar.cal.zero);
        lv_label_set_text_fmt(value2, "校准后电流:%s", current_str(mvar.msr.raw_mA));
    }else{
        lv_label_set_text_fmt(value, "Offset:<%d>", mvar.cal.zero);
        lv_label_set_text_fmt(value2, "Calibrated:%s", current_str(mvar.msr.raw_mA));
    }
}


void menu_over_temp(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            #ifdef CONFIG_ESC
            lv_label_set_text(title, "设置过热保护");
            #else
            lv_label_set_text(title, "设置过热报警");
            #endif
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            #ifdef CONFIG_ESC
            lv_label_set_text(title, "Set over-temperature protection");
            #else
            lv_label_set_text(title, "Set over-temperature alarm");
            #endif
            lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_cn_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            #ifdef CONFIG_ESC
            lv_label_set_text(label, "当温度高于此设置温度时, 会关闭输出");
            #else
            lv_label_set_text(label, "当温度高于此设置温度时, 蜂鸣器会报警");
            #endif
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            #ifdef CONFIG_ESC
            lv_label_set_text(label, "When the temperature is higher than this setting\nthe output will be closed");
            #else
            lv_label_set_text(label, "When the temperature is higher than this setting\nthe buzzer will alarm");
            #endif
            lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    lv_label_set_text_fmt(value, "%d℃", mvar.store.over_temp);
}

#ifdef CONFIG_ESC
void menu_low_volt_off(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置低压保护");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set low voltage protection");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "低于此设置电压时, 会关闭输出");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "When the voltage is lower than this setting\nthe output will be closed");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    update_setting_value(value, mvar.store.low_volt_off, mvar.set_idx, "V");
}


void menu_over_curt_off(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置过流保护");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set over current protection");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "高于此设置电流时, 会关闭输出");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "When the current is higher than this setting\nthe output will be closed");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    update_setting_value(value, mvar.store.over_curt_off, mvar.set_idx, "A");
}

void menu_out_hz(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置输出频率");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set output frequency");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "左右调节输出频率, 按Set键保存");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "Adjust the output frequency, press the Set key to save");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    if(mvar.store.hz<1000){
        lv_label_set_text_fmt(value, "%dHZ", mvar.store.hz);
    }else{
        lv_label_set_text_fmt(value, "%dKHZ", mvar.store.hz);
    }
}


void menu_rotary(void)
{
    static lv_obj_t *scr, *value, *value2;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "旋钮校准");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set rotary calibration");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, lv_pct(-10));

        value2 = lv_label_create(scr);
        lv_obj_set_style_text_font(value2, &lv_font_cn_24, LV_PART_MAIN);
        lv_obj_align(value2, LV_ALIGN_CENTER, 0, lv_pct(10));

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "旋钮调至最低, 按左键保存\n旋钮调至最高, 按右键保存");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "Turn the knob to the lowest, press the left key to save\nTurn the knob to the highest, press the right key to save");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(40));
    }

    lv_screen_load(scr);
    if(mvar.store.lang==languageCN){
        lv_label_set_text_fmt(value, "当前值:%d", mvar.esc.adc_raw);
        lv_label_set_text_fmt(value2, "最小值:%d 最大值:%d", mvar.cal.adc_min, mvar.cal.adc_max);
    }else{
        lv_label_set_text_fmt(value, "Current:%d", mvar.esc.adc_raw);
        lv_label_set_text_fmt(value2, "Minimum:%d Maximum:%d", mvar.cal.adc_min, mvar.cal.adc_max);
    }
}
#endif

void menu_voice_set(void)
{
    static lv_obj_t *scr, *value;
    if(!scr){
        scr = lv_obj_create(NULL);
        lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
        lv_obj_center(scr);

        lv_obj_t *title = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(title, "设置音量");
            lv_obj_set_style_text_font(title, &lv_font_cn_32, LV_PART_MAIN);
        }else{
            lv_label_set_text(title, "Set volume");
            lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
        }
        lv_obj_set_size(title, lv_pct(100), lv_pct(22));
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_color(title, lv_color_hex(0x61c0bf), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(title, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, lv_pct(5));

        value = lv_label_create(scr);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_32, LV_PART_MAIN);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(scr);
        if(mvar.store.lang==languageCN){
            lv_label_set_text(label, "左右调节音量, 按Set键保存");
            lv_obj_set_style_text_font(label, &lv_font_cn_16, LV_PART_MAIN);
        }else{
            lv_label_set_text(label, "Adjust the volume, press the Set key to save");
            lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
        }
        lv_obj_set_style_text_color(label, lv_color_hex(0x00b8a9), LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, lv_pct(30));
    }

    lv_screen_load(scr);
    lv_label_set_text_fmt(value, "%d", mvar.store.voice);
}


void menu_def_set(void)
{
    mvar.over_curt_off=0;
    mvar.store=defaults;
    for(int i=0;i<ASIZE(mvar.cal.volt);i++){
        mvar.cal.volt[i].val=1.0;
    }
    for(int i=0;i<ASIZE(mvar.cal.curt);i++){
        mvar.cal.curt[i].val=1.0;
    }
    save();
    save_cal();
    
    vTaskDelay(500);
    lcd_view_menu_update();
}
#if 0
void menu_sys_info(void)
{
    uint32_t byte=esp_get_free_heap_size()/1024;
	TaskStatus_t taskList[16];
	static const char *taskState[]={"Run","Ready","Block","Suspend","Delete"};
	uint32_t totalTime,num,cpu;

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);

    printf("LVGL Total: %d bytes\n", mon.total_size);
    printf("LVGL Used: %d bytes\n", mon.total_size - mon.free_size);
    printf("LVGL Free: %d bytes\n", mon.free_size);
    printf("LVGL Big: %d bytes\n", mon.free_biggest_size);
    printf("LVGL: %.1f%%\n", 
           (float)(mon.total_size - mon.free_size) / mon.total_size * 100);

	num=uxTaskGetSystemState(taskList,16,&totalTime);

    printf("free:%luKB num:%lu totalTime:%lu\n",byte,num,totalTime);
	printf("%-16sState\tID\tPrio\tFree\tCPU\n","Name");
	if(totalTime){
		for(int i=0;i<num;i++){
			cpu=(unsigned long long)taskList[i].ulRunTimeCounter*100/totalTime;
			printf("%-16s%s\t%u\t%u\t%lu\t%s%lu%%"
			, taskList[i].pcTaskName
			, taskList[i].eCurrentState<ASIZE(taskState)?taskState[taskList[i].eCurrentState]:"Unknow"
			, taskList[i].xTaskNumber, taskList[i].uxCurrentPriority
			, taskList[i].usStackHighWaterMark, cpu?"":"<", cpu?cpu:1
			);

			if(taskList[i].xCoreID==tskNO_AFFINITY){
				printf(" (-)\n");
			}else{
				printf(" (%d)\n", taskList[i].xCoreID);
			}
		}
	}
    
    
}
#endif
typedef struct {
    const void *img;
    lv_obj_t *btn;
    const char *text;
    const char *text_EN;
    void (*view)(void);
} menu_item_t;

menu_item_t menus[] = {
    {&img_curt_curve, NULL, "电流曲线", "Current Curve", menu_current_curve},
    {&img_low_volt, NULL, "低压报警", "Low-voltage alarm", menu_low_volt},
    {&img_over_curt, NULL, "过流报警", "Over-current alarm", menu_over_curt},
    {&img_volt_cal, NULL, "电压校准", "Voltage calibration", menu_volt_cal},
    {&img_curt_cal, NULL, "电流校准", "Current calibration", menu_curt_cal},
    {&img_zero_cal, NULL, "零点校准", "Zero calibration", menu_zero_cal},
    #ifdef CONFIG_ESC
    {&img_over_temp, NULL, "过热保护", "Over-temperature protection", menu_over_temp},
    {&img_low_volt_off, NULL, "低压保护", "Low-voltage protection", menu_low_volt_off},
    {&img_over_curt_off, NULL, "过流保护", "Over-current protection", menu_over_curt_off},
    {&img_out_hz, NULL, "输出频率", "Output frequency", menu_out_hz},
    {&img_rotary, NULL, "旋钮校准", "Knob calibration", menu_rotary},
    #else
    {&img_over_temp, NULL, "过热报警", "Over-temperature alarm", menu_over_temp},
    #endif
    {&img_voice_set, NULL, "音量设置", "Volume setting", menu_voice_set},
    {&img_def_set, NULL, "恢复默认", "Restore default", menu_def_set}
    #if 0
    ,{&img_def_set, NULL, "系统信息", "System info", menu_sys_info}
    #endif
};

lv_obj_t *lv_list_add_item(lv_obj_t * list, menu_item_t *item)
{
    item->btn = lv_obj_create(list);
    lv_obj_set_size(item->btn, lv_pct(100), lv_pct(24));
    lv_obj_set_flex_flow(item->btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_opa(item->btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(item->btn, lv_color_hex(0xa6e3e9), LV_PART_MAIN|LV_STATE_CHECKED);
    
    lv_obj_set_style_border_width(item->btn, 1, LV_PART_MAIN);  
    lv_obj_set_style_border_color(item->btn, lv_color_hex(0xBBBBBB), LV_PART_MAIN);
    lv_obj_set_style_border_side(item->btn, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
    lv_obj_set_style_pad_all(item->btn, 1, LV_PART_MAIN);

    lv_obj_t *icon = lv_image_create(item->btn);
    lv_image_set_src(icon, item->img);
    lv_obj_set_style_radius(icon, 6, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(item->btn);
    if(mvar.store.lang==languageCN){
        lv_label_set_text(label, item->text);
    }else{
        lv_label_set_text(label, item->text_EN);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    }
    lv_obj_set_style_text_font(label, &lv_font_cn_32, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_size(label, lv_pct(80), lv_pct(100));

    lv_obj_t *right_icon = lv_label_create(item->btn);
    lv_label_set_text(right_icon, LV_SYMBOL_RIGHT);
    lv_obj_set_size(right_icon, lv_pct(4), lv_pct(100));
    lv_obj_set_style_text_font(right_icon, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(right_icon, lv_color_hex(0x888888), LV_PART_MAIN);

    return item->btn;
}

void lcd_view_menu_init(void)
{
    if(menu_view.scr){
        return ;
    }
    menu_view.scr = lv_obj_create(NULL);
    lv_obj_set_size(menu_view.scr, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(menu_view.scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(menu_view.scr, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_row(menu_view.scr, 2, LV_PART_MAIN);
    lv_obj_set_style_width(menu_view.scr, 0, LV_PART_SCROLLBAR);
    lv_obj_center(menu_view.scr);
    
    for (int i = 0; i < sizeof(menus) / sizeof(menus[0]); i++) {
        menu_item_t *item = &menus[i];
        item->btn = lv_list_add_item(menu_view.scr, item);
    }
    lv_obj_add_state(menus[0].btn, LV_STATE_CHECKED);
}

void lcd_view_menu_update(void)
{
    lv_screen_load(menu_view.scr);
    for(int i=0;i<ASIZE(menus);i++){
        lv_obj_clear_state(menus[i].btn, LV_STATE_CHECKED);
    }
    lv_obj_add_state(menus[mvar.menuIdx].btn, LV_STATE_CHECKED);
    lv_obj_scroll_to_view(menus[mvar.menuIdx].btn, LV_ANIM_ON); 
}
#endif

#define VIEW_KEY
#ifdef VIEW_KEY


void key_fun_main(int event)
{
    switch(event){
        case keyLeftShort:
            mvar.msr.mWH=0;
            save_mWH();
            break;
        case keyRightShort:
            if(mvar.over_curt_off){
                mvar.over_curt_off=0;
                break;
            }
            mvar.store.rotate=!mvar.store.rotate;
            lcd_display_rotate(mvar.store.rotate);
            lv_obj_invalidate(lv_screen_active());
            save();
            break;
        case keySetShort:
            mvar.menuIdx=0;
            lcd_view_menu_init();
            lcd_view_menu_update();
            break;
        default :
            break;
    }
}


void key_fun_menu(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.menuIdx>0){
                mvar.menuIdx--;
            }else{
                mvar.menuIdx=ASIZE(menus)-1;
            }
            lcd_view_menu_update();
            break;
        case keyRightShort:
            if(mvar.menuIdx<ASIZE(menus)-1){
                mvar.menuIdx++;
            }else{
                mvar.menuIdx=0;
            }
            lcd_view_menu_update();
            break;
        case keySetShort:
            mvar.seting=0;
            mvar.set_idx=0;
            mvar.cal_point=0;
            menus[mvar.menuIdx].view();
            break;
        case keySetLong:
            lv_screen_load(main_view.scr);
            break;
        default :
            break;
    }
}

void key_fun_current_curve(int event)
{
    switch(event){
        case keyLeftShort:
            break;
        case keyRightShort:
            break;
        case keySetShort:
        case keySetLong:
            lcd_view_menu_update();
            break;
        default :
            break;
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
            save();
            lcd_view_menu_update();
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
            save();
            lcd_view_menu_update();
            break;
        default:
            break;
    }
}


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
            lcd_view_menu_update();
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
            lcd_view_menu_update();
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
            lcd_view_menu_update();
            break;
        default :
            break;
    }
}

void key_fun_low_volt_off(int event)
{
    switch(event){
        case keyLeftShort:
            if(!mvar.seting){
                if(mvar.set_idx>0){
                    mvar.set_idx--;
                }
            }else{
                int dec=pow(10,4-mvar.set_idx);
                if(mvar.store.low_volt_off>=dec){
                    mvar.store.low_volt_off-=dec;
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
                if(mvar.store.low_volt_off<30000){
                    mvar.store.low_volt_off+=dec;
                }
            }
            break;
        case keySetShort:
            mvar.seting=!mvar.seting;
            break;
        case keySetLong:
            save();
            lcd_view_menu_update();
            break;
        default:
            break;
    }
}

void key_fun_over_curt_off(int event)
{
    switch(event){
        case keyLeftShort:
            if(!mvar.seting){
                if(mvar.set_idx>0){
                    mvar.set_idx--;
                }
            }else{
                int dec=pow(10,4-mvar.set_idx);
                if(mvar.store.over_curt_off>=dec){
                    mvar.store.over_curt_off-=dec;
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
                if(mvar.store.over_curt_off<64000){
                    mvar.store.over_curt_off+=dec;
                }
            }
            break;
        case keySetShort:
            mvar.seting=!mvar.seting;
            break;
        case keySetLong:
            save();
            lcd_view_menu_update();
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
            save();
            lcd_view_menu_update();
            break;
        default:
            break;
    }
}

#ifdef CONFIG_ESC
void key_fun_out_hz(int event)
{
    int id=hz_list_id(mvar.store.hz);
    switch(event){
        case keyLeftShort:
            if(id>0){
                mvar.store.hz=hz_list[id-1];
            }
            break;
        case keyRightShort:
            if(id<hz_list_len-1){
                mvar.store.hz=hz_list[id+1];
            }
            break;
        case keySetShort:
        case keySetLong:
            save();
            pwm_set_hz(mvar.store.hz);
            lcd_view_menu_update();
            break;
        default:
            break;
    }
}

void key_fun_rotary(int event)
{
    switch(event){
        case keyLeftShort:
            if(mvar.esc.adc_raw<500){
                mvar.cal.adc_min=mvar.esc.adc_raw;
            }
            break;
        case keyRightShort:
            if(mvar.esc.adc_raw>4000){
                mvar.cal.adc_max=mvar.esc.adc_raw;
            }
            break;
        case keySetShort:
        case keySetLong:
            save_cal();
            lcd_view_menu_update();
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
            save();
            lcd_view_menu_update();
            break;
        default:
            break;
    }
}

// 按键处理函数
void lcd_view_key_handler(int event)
{
    lv_obj_t *scr = lv_screen_active();
    if(scr==main_view.scr){
        return key_fun_main(event);
    }else if(scr==menu_view.scr){
        return key_fun_menu(event);
    }else if(menus[mvar.menuIdx].view==menu_current_curve){
        return key_fun_current_curve(event);
    }else if(menus[mvar.menuIdx].view==menu_low_volt){
        return key_fun_low_volt(event);
    }else if(menus[mvar.menuIdx].view==menu_over_curt){
        return key_fun_over_curt(event);
    }else if(menus[mvar.menuIdx].view==menu_volt_cal){
        return key_fun_volt_cal(event);
    }else if(menus[mvar.menuIdx].view==menu_curt_cal){
        return key_fun_curt_cal(event);
    }else if(menus[mvar.menuIdx].view==menu_zero_cal){
        return key_fun_zero_cal(event);
    }else if(menus[mvar.menuIdx].view==menu_over_temp){
        return key_fun_over_temp(event);
    }
    #ifdef CONFIG_ESC
    else if(menus[mvar.menuIdx].view==menu_low_volt_off){
        return key_fun_low_volt_off(event);
    }else if(menus[mvar.menuIdx].view==menu_over_curt_off){
        return key_fun_over_curt_off(event);
    }else if(menus[mvar.menuIdx].view==menu_out_hz){
        return key_fun_out_hz(event);
    }else if(menus[mvar.menuIdx].view==menu_rotary){
        return key_fun_rotary(event);
    }
    #endif
    else if(menus[mvar.menuIdx].view==menu_voice_set){
        return key_fun_voice_set(event);
    }
}
#endif

struct {
    lv_obj_t *obj;
    lv_obj_t *icon;
    lv_obj_t *label;
    lv_obj_t *info;
    lv_anim_t anim;
    bool visible;
} alert_popup;

static void alert_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = (lv_obj_t *)var;
    lv_obj_set_style_bg_opa(obj, v, LV_PART_MAIN);
    lv_obj_set_style_border_opa(obj, v, LV_PART_MAIN);
    lv_obj_set_style_text_opa(alert_popup.label, v, LV_PART_MAIN);
    lv_obj_set_style_text_opa(alert_popup.icon, v, LV_PART_MAIN);
    lv_obj_set_style_text_opa(alert_popup.info, v, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(alert_popup.obj, v, LV_PART_MAIN);
}

void lcd_view_alert_init(void)
{
    // 创建弹窗容器
    alert_popup.obj = lv_obj_create(main_view.scr);
    lv_obj_set_size(alert_popup.obj, lv_pct(70), lv_pct(60));
    lv_obj_center(alert_popup.obj);
    lv_obj_add_flag(alert_popup.obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_width(alert_popup.obj, 0, LV_PART_SCROLLBAR);
    
    // 设置弹窗样式
    lv_obj_set_style_radius(alert_popup.obj, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(alert_popup.obj, lv_color_hex(0xFF0000), LV_PART_MAIN);  // 红色背景
    lv_obj_set_style_bg_opa(alert_popup.obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(alert_popup.obj, 3, LV_PART_MAIN);
    lv_obj_set_style_border_color(alert_popup.obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(alert_popup.obj, 20, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(alert_popup.obj, lv_color_hex(0xFF0000), LV_PART_MAIN);
    
    // 创建警告图标
    alert_popup.icon = lv_label_create(alert_popup.obj);
    lv_label_set_text(alert_popup.icon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_font(alert_popup.icon, &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_text_color(alert_popup.icon, lv_color_hex(0xFFD600), LV_PART_MAIN); // 黄色
    lv_obj_set_style_text_align(alert_popup.icon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(alert_popup.icon, LV_ALIGN_TOP_MID, 0, -10); // 顶部居中
    
    // 创建警告文本标签
    alert_popup.label = lv_label_create(alert_popup.obj);
    lv_obj_set_style_text_font(alert_popup.label, &lv_font_cn_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(alert_popup.label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(alert_popup.label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(alert_popup.label, LV_ALIGN_TOP_MID, 0, 30); // 图标下方

    // 创建信息文本标签
    alert_popup.info = lv_label_create(alert_popup.obj);
    lv_obj_set_style_text_font(alert_popup.info, &lv_font_cn_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(alert_popup.info, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(alert_popup.info, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(alert_popup.info, LV_ALIGN_TOP_MID, 0, 60); // 图标下方
    
    // 设置弹窗层级为最高
    lv_obj_move_foreground(alert_popup.obj);
    
    // 创建闪烁动画
    lv_anim_init(&alert_popup.anim);
    lv_anim_set_var(&alert_popup.anim, alert_popup.obj);
    lv_anim_set_values(&alert_popup.anim, LV_OPA_0, LV_OPA_80);  // 从0%透明度到80%
    lv_anim_set_time(&alert_popup.anim, 500);  // 500ms动画时间
    lv_anim_set_delay(&alert_popup.anim, 0);
    lv_anim_set_exec_cb(&alert_popup.anim, alert_anim_cb);
    lv_anim_set_playback_time(&alert_popup.anim, 500);  // 回放时间500ms
    lv_anim_set_repeat_count(&alert_popup.anim, LV_ANIM_REPEAT_INFINITE);  // 无限重复

    alert_popup.visible = false;
}

void lcd_view_alert_update(void)
{
    char *type=NULL, *msg="";

    if(mvar.store.low_volt_off && mvar.msr.mV < mvar.store.low_volt_off){
        if(mvar.store.lang==languageCN){
            type="低压保护";
            msg="已关闭输出";
        }else{
            type="Low-voltage";
            msg="Output closed";
        }
    }else if(mvar.over_curt_off || (mvar.store.over_curt_off && mvar.msr.mA > mvar.store.over_curt_off)){
        if(mvar.store.lang==languageCN){
            type="过流保护";
            msg="已关闭输出, 按右键恢复输出";
        }else{
            type="Over-current";
            msg="Output closed, press the right key to restore output";
        }
    }else if(mvar.store.low_volt && mvar.msr.mV < mvar.store.low_volt){
        if(mvar.store.lang==languageCN){
            type="电压过低";
        }else{
            type="Low-voltage";
        }
    }else if(mvar.store.over_curt && mvar.msr.mA > mvar.store.over_curt){
        if(mvar.store.lang==languageCN){
            type="电流过大";
        }else{
            type="Over-current";
        }
    }else if(mvar.store.over_temp && mvar.temp > mvar.store.over_temp){
        if(mvar.store.lang==languageCN){
            type="温度过高";
            #ifdef CONFIG_ESC
            msg="已关闭输出, 冷却后恢复输出";
            #endif
        }else{
            type="Over-temperature";
            #ifdef CONFIG_ESC
            msg="Output closed, restore after cooling";
            #endif
        }
    }

    if(!type){
        if(alert_popup.visible){
            lv_anim_del(&alert_popup.anim, alert_anim_cb);  // 停止动画
            lv_obj_add_flag(alert_popup.obj, LV_OBJ_FLAG_HIDDEN); // 隐藏弹窗
            alert_popup.visible = false;
            buzzer_stop();
        }
    }else{
        lv_label_set_text(alert_popup.label, type);
        lv_label_set_text(alert_popup.info, msg);
        if(!alert_popup.visible){
            lv_anim_start(&alert_popup.anim);   // 开始动画
            lv_obj_clear_flag(alert_popup.obj, LV_OBJ_FLAG_HIDDEN); // 显示弹窗
            alert_popup.visible = true;
            buzzer_set(buzzerFlash);
        }
    }
}

#ifndef CONFIG_ESC
char *resistor_str(void)
{
    static char S[16]={0};
    char str[16]={0};

    if(!mvar.msr.mA){
        return "  -  ";
    }
    
    int R=mvar.msr.mV*1000/mvar.msr.mA;
    
    if(R<1000){
        snprintf(str,sizeof(str),"%dmΩ",R);
    }else if(R<10000){
        snprintf(str,sizeof(str),"%.2fΩ",(double)R/1000);
    }else if(R<100000){
        snprintf(str,sizeof(str),"%.1fΩ",(double)R/1000);
    }else if(R<1000000){
        snprintf(str,sizeof(str),"%dΩ",R/1000);
    }else if(R<10000000){
        snprintf(str,sizeof(str),"%.1fKΩ",(double)R/1000000);
    }else if(R<1000000000){
        snprintf(str,sizeof(str),"%dKΩ",R/1000000);
    }
    snprintf(S,sizeof(S),"%4s",str);
    return S;
}
#endif

void lcd_view_main_update(void)
{
    lv_obj_t *scr = lv_screen_active();

    if(scr==main_view.scr){
        lv_label_set_text(main_view.voltage, voltage_str());
        lv_label_set_text(main_view.current, current_str(mvar.msr.mA));
        lv_label_set_text(main_view.power, power_str());

        if(mvar.store.lang==languageCN){
            lv_label_set_text_fmt(main_view.time, "运行时间\n%s", time_str());
            lv_label_set_text_fmt(main_view.power_stat, "电量统计\n%s", elec_str());
            lv_label_set_text_fmt(main_view.temp, "核心温度\n%d℃", (int)mvar.temp);
            #ifdef CONFIG_ESC
            lv_label_set_text_fmt(main_view.duty, "占空比\n%s", duty_str());
            #else
            lv_label_set_text_fmt(main_view.duty, "负载电阻\n%s", resistor_str());
            #endif
        }else{
            lv_label_set_text_fmt(main_view.time, "Time\n%s", time_str());
            lv_label_set_text_fmt(main_view.power_stat, "Total\n%s", elec_str());
            lv_label_set_text_fmt(main_view.temp, "Temp\n%d℃", (int)mvar.temp);
            #ifdef CONFIG_ESC
            lv_label_set_text_fmt(main_view.duty, "Duty\n%s", duty_str());
            #else
            lv_label_set_text_fmt(main_view.duty, "Load res\n%s", resistor_str());
            #endif
        }
        lcd_view_alert_update();
    }
    else if(scr!=menu_view.scr){
        menus[mvar.menuIdx].view();
    }
}

