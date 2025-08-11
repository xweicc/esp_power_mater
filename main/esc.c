#include "main.h"


#define EN_IO GPIO_NUM_2
#define PWM_IO GPIO_NUM_1
#define PWM_CH LEDC_CHANNEL_1
#define ADC_IO GPIO_NUM_4
#define ADC_CH ADC_CHANNEL_4
#define ADC_MAX 4600

static void pwm_init(void)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((1ULL<<PWM_IO)|(1ULL<<EN_IO));
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);
    gpio_set_level(PWM_IO, 0);
    gpio_set_level(EN_IO, 0);
    
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_1,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .freq_hz          = mvar.store.hz,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel[1] = {
        {
            .speed_mode     = LEDC_LOW_SPEED_MODE,
            .channel        = PWM_CH,
            .timer_sel      = LEDC_TIMER_1,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = PWM_IO,
            .duty           = 0,
            .hpoint         = 0
        }
    };

    for(int i=0;i<1;i++){
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[i]));
    }
}

char *duty_str(void)
{
    static char S[16]={0};
    snprintf(S,sizeof(S),"%5d",mvar.esc.val);
    return S;
}


static void update_pwm(int val)
{
    if(mvar.store.low_volt_off && mvar.msr.mV < mvar.store.low_volt_off){
        val=0;
    }
    if(mvar.over_curt_off || (mvar.store.over_curt_off && mvar.msr.mA > mvar.store.over_curt_off)){
        mvar.over_curt_off=1;
        val=0;
    }
    if(mvar.store.over_temp && mvar.temp > mvar.store.over_temp){
        val=0;
    }
    if(val >=0 && val <=1024){
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CH, val));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CH));
        mvar.esc.val=val;
    }else{
        Printf("val:%d error\n",val);
    }

    gpio_set_level(EN_IO, 1);
}

int pwm_set_hz(int hz)
{
    esp_err_t ret=ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, hz);
    if(ret){
        Printf("ledc_set_freq error:%d\n",ret);
    }
    return ret;
}

int get_ch_adc(int ch)
{
    int adc_raw[10]={0};
    int max_idx=0,min_idx=0;
    int sum=0,adc=0;

    for(int c=0;c<10;c++){
        esp_err_t ret=adc_oneshot_read(mvar.adc1_handle, ch, &adc_raw[c]);
        if(ret){
            Printf("adc_oneshot_read error:%d\n",ret);
            return -1;
        }
    }
    for(int c=0;c<10;c++){
        if(adc_raw[c]>adc_raw[max_idx]){
            max_idx=c;
        }else if(adc_raw[c]<adc_raw[min_idx]){
            min_idx=c;
        }
    }
    for(int c=0;c<10;c++){
        if (c!=max_idx && c!=min_idx)
        sum+=adc_raw[c];
    }

    if(max_idx==min_idx){
        adc=sum/9;
    }else{
        adc=sum/8;
    }

    return adc;
}


static void adc_timer_fun(unsigned long data)
{
    int adc_raw=get_ch_adc(ADC_CH);
    if(adc_raw<0){
        goto out;
    }
    adc_raw*=2;
    mvar.esc.adc_raw=adc_raw;

    adc_raw-=mvar.cal.adc_min;
    int max=mvar.cal.adc_max-mvar.cal.adc_min;
    if(adc_raw>max){
        adc_raw=max;
    }
    adc_raw=adc_raw*1024/max;
    adc_raw=1024-adc_raw;
    if(adc_raw<20){
        adc_raw=0;
    }
    if(adc_raw>1000){
        adc_raw=1024;
    }
    
    Printf("adc_raw:%d\n",adc_raw);
    update_pwm(1024-adc_raw);
    
out:
    mod_timer(&mvar.esc.timer, jiffies+50);
}

int esc_init(void)
{
    pwm_init();

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    int ret=adc_oneshot_config_channel(mvar.adc1_handle, ADC_CH, &config);
    if(ret){
        Printf("=============error:%d\n",ret);
    }
    setup_timer(&mvar.esc.timer, adc_timer_fun, 0);
	mod_timer(&mvar.esc.timer, jiffies+50);
    return 0;
}

