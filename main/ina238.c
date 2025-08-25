#include "main.h"

#define CurrentLSB 2    //电流分辨率，mA
#define CurrentCal 1638 //校准寄存器值

void i2c_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        #ifdef CONFIG_LCD
        .sda_io_num = 19,
        .scl_io_num = 18,
        #else
        .sda_io_num = 1,
        .scl_io_num = 0,
        #endif
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };

    i2c_param_config(0, &conf);
    
    ESP_ERROR_CHECK(i2c_driver_install(0, conf.mode, 0, 0, 0));
}

static esp_err_t ina238_register_read(uint8_t reg_addr, uint16_t *data)
{
    uint8_t buf[2] = {0};
    
    esp_err_t ret = i2c_master_write_read_device(0, INA238_ADDR, &reg_addr, 1, buf, 2, 10);
    if (ret != ESP_OK) {
        Printf("read %02X failed\n",reg_addr);
        return ret;
    }

    *data=buf[0]<<8|buf[1];

    return ESP_OK;
}

static esp_err_t ina238_register_write(uint8_t reg_addr, uint16_t data)
{
    uint8_t write_buf[3] = {reg_addr, data>>8, data&0xFF};

    esp_err_t ret = i2c_master_write_to_device(0, INA238_ADDR, write_buf, sizeof(write_buf), 10);
    if (ret != ESP_OK) {
        Printf("write %02X failed\n",reg_addr);
        return ret;
    }

    return ESP_OK;
}


//获取电压，mV
int ina238_get_voltage(void)
{
    uint16_t data = 0;
    ina238_register_read(INA238_REG_VBUS, &data);
    return (int)((float)data * 3.125);
}

//获取电流，mA
int ina238_get_current(int *dir)
{
    int16_t data = 0;
    ina238_register_read(INA238_REG_CURRENT, (uint16_t*)&data);
    //Printf("data:%d\n",data);
    if(data<=2 && data>=-2){
        data=0;
    }
    data+=mvar.cal.zero;
    if(data<0){
        *dir=-1;
        data=0-data;
    }else if(data>0){
        *dir=1;
    }else{
        *dir=0;
    }
    return data * CurrentLSB;
}

//获取功率，mW
int ina238_get_power(void)
{
    uint16_t data = 0;
    ina238_register_read(INA238_REG_PWR, &data);
    return (int)((float)data * CurrentLSB *0.2);
}


//获取ID
int ina238_get_id(void)
{
    uint16_t id = 0;
    
    ina238_register_read(INA238_REG_ID, &id);
    Printf("id:%04X\n",id);
    
    return id;
}

int ina238_init(void)
{
    i2c_init();

    if (ina238_get_id() != INA238_ID_NUM) {
        Printf("Not find ina238\n");
        return -1;
    }

    ina238_register_write(INA238_REG_CONF, 0x8000); //reset
    ina238_register_write(INA238_REG_ADC, 0xFFFA); //AVG 16
    ina238_register_write(INA238_REG_CAL, CurrentCal);

    return 0;
}

static inline int cal_volt_raw(int i)
{
    if(!mvar.cal.volt[i].raw){
        return volt_cal_point[i];
    }else{
        return mvar.cal.volt[i].raw;
    }
}

static inline int cal_curt_raw(int i)
{
    if(!mvar.cal.curt[i].raw){
        return curt_cal_point[i];
    }else{
        return mvar.cal.curt[i].raw;
    }
}


int get_cal_volt(int mV)
{
    int a=0,b=0;
    for(int i=0;i<ASIZE(mvar.cal.volt);i++){
        if(mV<cal_volt_raw(i)){
            b=i;
            break;
        }
        a=i;
        b=i;
    }
    if(mvar.cal.volt[a].val==mvar.cal.volt[b].val){
        return mV*mvar.cal.volt[a].val;
    }else{
        if(!mvar.cal.volt[a].raw){
            return mV*mvar.cal.volt[b].val;
        }else if(!mvar.cal.volt[b].raw){
            return mV*mvar.cal.volt[a].val;
        }else{
            float cal=(mvar.cal.volt[b].val-mvar.cal.volt[a].val)*(mV-cal_volt_raw(a))/(cal_volt_raw(b)-cal_volt_raw(a));
            cal=mvar.cal.volt[a].val+cal;
            return mV*cal;
        }
    }
}

int get_cal_curt(int mA)
{
    int a=0,b=0;
    for(int i=0;i<ASIZE(mvar.cal.curt);i++){
        if(mA<cal_curt_raw(i)){
            b=i;
            break;
        }
        a=i;
        b=i;
    }
    if(mvar.cal.curt[a].val==mvar.cal.curt[b].val){
        return mA*mvar.cal.curt[a].val;
    }else{
        if(!mvar.cal.curt[a].raw){
            return mA*mvar.cal.curt[b].val;
        }else if(!mvar.cal.curt[b].raw){
            return mA*mvar.cal.curt[a].val;
        }else{
            float cal=(mvar.cal.curt[b].val-mvar.cal.curt[a].val)*(mA-cal_curt_raw(a))/(cal_curt_raw(b)-cal_curt_raw(a));
            cal=mvar.cal.curt[a].val+cal;
            return mA*cal;
        }
    }
}

void ina238_run(void)
{
    int v=ina238_get_voltage();
    int a=ina238_get_current(&mvar.msr.dir);
    mvar.msr.raw_mV=v;
    mvar.msr.raw_mA=a;
    mvar.msr.mV=get_cal_volt(v);
    mvar.msr.mA=get_cal_curt(a);
    mvar.msr.mW=mvar.msr.mV*mvar.msr.mA/1000;
    if(mvar.msr.mA>mvar.msr.max_mA){
        mvar.msr.max_mA=mvar.msr.mA;
    }
    if(mvar.msr.mW>mvar.msr.max_mW){
        mvar.msr.max_mW=mvar.msr.mW;
    }
    #ifndef CONFIG_LCD
    hist_data_update(0,mvar.msr.mA);
    #endif
}

void ina238_timer_fun(unsigned long data)
{
    ina238_run();
    mod_timer(&mvar.ina_timer, jiffies+50);
}


void ina238_task(void *param)
{
    while (1) {
        ina238_run();
        vTaskDelay(50);
    }
}

