#ifndef __ESC_H__
#define __ESC_H__

typedef struct {
    struct timer_list timer;
    int adc_raw;
    int val;
} esc_data_t;

int esc_init(void);
int pwm_set_hz(int hz);
char *duty_str(void);
bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);

#endif

