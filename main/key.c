#include "main.h"

int ttl_key_is_down(key_trigger_t *key)
{
	int val=gpio_get_level(key->io);

	if(!val){
		if(key->state==keyStateNone){
			key->state=keyStateDowning;
			key->tm=jiffies;
		}else if(key->state==keyStateDowning && time_after(jiffies,key->tm+keyDelayTime)){
			key->state=keyStateDowned;
			key->tm=jiffies;
		}else if(key->state==keyStateDowned){
			if(time_after(jiffies,key->tm+keyLongTime)){
				key->state=keyStateDownedLong;
                return keyEventLong;
			}
		}
	}else{
		if(key->state==keyStateDowned){
            key->state=keyStateUping;
            key->tm=jiffies;
        }else if(key->state==keyStateUping && time_after(jiffies,key->tm+keyDelayTime)){
            key->state=keyStateNone;
            return keyEventShort;
        }else if(key->state==keyStateDownedLong && time_after(jiffies,key->tm+keyDelayTime)){
            key->state=keyStateNone;
        }
	}

    return keyEventNone;
}


#ifdef CONFIG_ADC_KEY
int adc_key_is_down(key_trigger_t *keys, uint8_t *keyId)
{
	int val=0;
    int voltage=0;

	for(int i=0;i<10;i++){
        int tmp;
		esp_err_t ret=adc_oneshot_read(mvar.adc1_handle, ADC_CHANNEL_3, &tmp);
        if(ret){
            Printf("adc_oneshot_read error:%d\n",ret);
            return keyEventNone;
        }
        val+=tmp;
	}
	val/=10;
    esp_err_t ret=adc_cali_raw_to_voltage(mvar.adc1_cali_handle, val, &voltage);
    if(ret){
        Printf("adc_cali_raw_to_voltage err:%d\n",ret);
        return keyEventNone;
    }

	for(int i=0;i<keyNumMax;i++){
        if(keys[i].type!=keyTypeADC){
            continue;
        }
		if(val>=keys[i].valMin && val<=keys[i].valMax){
			if(mvar.keyLastId!=i){
				mvar.keyLastId=i;
				return keyEventNone;
			}
			mvar.keyLastId=i;

            //Printf("Down: i:%d valMin:%d valMax:%d val:%d\n",i,keys[i].valMin,keys[i].valMax,val);
			if(keys[i].state==keyStateNone){
    			keys[i].state=keyStateDowning;
    			keys[i].tm=jiffies;
    		}else if(keys[i].state==keyStateDowning && time_after(jiffies,keys[i].tm+keyDelayTime)){
    			keys[i].state=keyStateDowned;
    			keys[i].tm=jiffies;
    		}else if(keys[i].state==keyStateDowned){
    			if(time_after(jiffies,keys[i].tm+keyLongTime)){
    				keys[i].state=keyStateDownedLong;
                    *keyId=i;
                    return keyEventLong;
    			}
    		}
		}
        else{
			if(keys[i].state==keyStateDowned){
                keys[i].state=keyStateUping;
                keys[i].tm=jiffies;
            }else if(keys[i].state==keyStateUping && time_after(jiffies,keys[i].tm+keyDelayTime)){
                keys[i].state=keyStateNone;
                *keyId=i;
                return keyEventShort;
            }else if(keys[i].state==keyStateDownedLong && time_after(jiffies,keys[i].tm+keyDelayTime)){
                keys[i].state=keyStateNone;
            }
		}
	}

    return keyEventNone;
}
#endif

void key_check_timer_fun(unsigned long data)
{
    for(int i=0;i<keyNumMax;i++)
    {
        if(mvar.keys[i].type!=keyTypeTTL){
            continue;
        }
        int event=ttl_key_is_down(&mvar.keys[i]);
        if(event==keyEventNone){
            continue;
        }else if(event==keyEventShort){
            buzzer_set(buzzerShort);
            if(i==keyNumSet){
                mvar.keys[i].fun(keySetShort);
            }else if(i==keyNumLeft){
                mvar.keys[i].fun(keyLeftShort);
            }else if(i==keyNumRight){
                mvar.keys[i].fun(keyRightShort);
            }
            break;
        }else if(event==keyEventLong){
            buzzer_set(buzzerShort);
            if(i==keyNumSet){
                mvar.keys[i].fun(keySetLong);
            }else if(i==keyNumLeft){
                mvar.keys[i].fun(keyLeftLong);
            }else if(i==keyNumRight){
                mvar.keys[i].fun(keyRightLong);
            }
            break;
        }
    }

#ifdef CONFIG_ADC_KEY
    {
        uint8_t keyId=0;
        int event=adc_key_is_down(mvar.keys, &keyId);
        if(event==keyEventShort){
            buzzer_set(buzzerShort);
            if(keyId==keyNumSet){
                mvar.keys[keyId].fun(keySetShort);
            }else if(keyId==keyNumLeft){
                mvar.keys[keyId].fun(keyLeftShort);
            }else if(keyId==keyNumRight){
                mvar.keys[keyId].fun(keyRightShort);
            }
        }else if(event==keyEventLong){
            buzzer_set(buzzerShort);
            if(keyId==keyNumSet){
                mvar.keys[keyId].fun(keySetLong);
            }else if(keyId==keyNumLeft){
                mvar.keys[keyId].fun(keyLeftLong);
            }else if(keyId==keyNumRight){
                mvar.keys[keyId].fun(keyRightLong);
            }
        }
    }
#endif

    mod_timer(&mvar.key_timer, jiffies+keyCheckTime);
}


int key_register_fun(int key, key_fun_t fun)
{
    if(key>=keyNumMax){
        return -1;
    }
    mvar.keys[key].fun=fun;

    return 0;
}

#ifdef CONFIG_ADC_KEY

bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        Printf("calibration scheme version is %s", "Curve Fitting\n");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        Printf("calibration scheme version is %s", "Line Fitting\n");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        Printf("Calibration Success\n");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        Printf("eFuse not burnt, skip software calibration\n");
        esp_restart();
    } else {
        Printf("Invalid arg or no memory\n");
        esp_restart();
    }

    return calibrated;
}

static int adc_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &mvar.adc1_handle));

    adc_calibration_init(ADC_UNIT_1, ADC_ATTEN_DB_12, &mvar.adc1_cali_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    int ret=adc_oneshot_config_channel(mvar.adc1_handle, ADC_CHANNEL_3, &config);
    if(ret){
        Printf("=============error:%d\n",ret);
    }
    return 0;
}
#endif


int key_init(void)
{
#ifdef CONFIG_ADC_KEY
    adc_init();

    mvar.keys[keyNumRight].type=keyTypeADC;
    mvar.keys[keyNumRight].valMin=0;
    mvar.keys[keyNumRight].valMax=200;
    mvar.keys[keyNumLeft].type=keyTypeADC;
    mvar.keys[keyNumLeft].valMin=1000;
    mvar.keys[keyNumLeft].valMax=3000;
#endif
    gpio_config_t io_conf = {};
#ifdef CONFIG_ADC_KEY
    io_conf.pin_bit_mask = (1ULL<<keyIoSet);
#else
    io_conf.pin_bit_mask = ((1ULL<<keyIoSet) | (1ULL<<keyIoLeft) | (1ULL<<keyIoRight));
#endif
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);

    mvar.keys[keyNumSet].type=keyTypeTTL;
    mvar.keys[keyNumSet].io=keyIoSet;
#ifndef CONFIG_ADC_KEY
    mvar.keys[keyNumLeft].type=keyTypeTTL;
    mvar.keys[keyNumLeft].io=keyIoLeft;
    mvar.keys[keyNumRight].type=keyTypeTTL;
    mvar.keys[keyNumRight].io=keyIoRight;
#endif

    setup_timer(&mvar.key_timer, key_check_timer_fun, 0);
    mod_timer(&mvar.key_timer, jiffies+keyCheckTime);
    
    return 0;
}

