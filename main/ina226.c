#include "main.h"
#include "ina226.h"

static struct {
    int shuntResistor;  //分流电阻，mΩ
    int currentLSB;     //电流分辨率，mA
    int calRegister;    //校准寄存器值
} calTable[] = {
    {1,2,2560},
    {2,1,2560},
    {0,0,0}
};
static int calIdx = 0;

int mV,mA,mW;


void i2c_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 1,
        .scl_io_num = 0,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };

    i2c_param_config(0, &conf);
    
    ESP_ERROR_CHECK(i2c_driver_install(0, conf.mode, 0, 0, 0));
}

static esp_err_t ina226_register_read(uint8_t reg_addr, uint16_t *data)
{
    uint8_t buf[2] = {0};
    
    esp_err_t ret = i2c_master_write_read_device(0, INA226_ADDR, &reg_addr, 1, buf, 2, 100);
    if (ret != ESP_OK) {
        Printf("read %02X failed\n",reg_addr);
        return ret;
    }

    *data=buf[0]<<8|buf[1];

    return ESP_OK;
}

static esp_err_t ina226_register_write(uint8_t reg_addr, uint16_t data)
{
    uint8_t write_buf[3] = {reg_addr, data>>8, data&0xFF};

    esp_err_t ret = i2c_master_write_to_device(0, INA226_ADDR, write_buf, sizeof(write_buf), 100);
    if (ret != ESP_OK) {
        Printf("write %02X failed\n",reg_addr);
        return ret;
    }

    return ESP_OK;
}


//获取电压，mV
int ina226_get_voltage(void)
{
    uint16_t data = 0;
    ina226_register_read(INA226_REG_BV, &data);
    return data * 125 / 100;
}

//获取电流，mA
int ina226_get_current(void)
{
    uint16_t data = 0;
    ina226_register_read(INA226_REG_CUR, &data);
    if (data>0xFF00) {
        return 0;
    }
    return data * calTable[calIdx].currentLSB;
}

//获取功率，mW
int ina226_get_power(void)
{
    uint16_t data = 0;
    ina226_register_read(INA226_REG_PWR, &data);
    return data * calTable[calIdx].currentLSB * 25;
    return 0;
}


//获取ID
int ina226_get_id(void)
{
    uint16_t id = 0;
    
    ina226_register_read(INA226_REG_ID, &id);
    Printf("id:%04X\n",id);
    
    return id;
}

//设置采样电阻，毫欧
int ina226_set_resistor(int resistor)
{
    uint16_t data;
    
    for (int i=0;calTable[i].shuntResistor;i++){
        if (calTable[i].shuntResistor == resistor) {
            data = (uint16_t)calTable[i].calRegister;
            calIdx = i;
            ina226_register_write(INA226_REG_CAL, data);
            break;
        }
    }
    return 0;
}


int ina226_init(void)
{
    i2c_init();

    if (ina226_get_id() != INA226_ID_NUM) {
        Printf("Not find ina226\n");
        return -1;
    }

    ina226_register_write(INA226_REG_CONF, 0x4927); //AGV:128,VBUSCT:1.1ms
    ina226_set_resistor(1); //1mΩ

    return 0;
}


void ina226_task(void *param)
{
    while (1) {
        vTaskDelay(100);
        int v=ina226_get_voltage();
        int a=ina226_get_current();
        int w=ina226_get_power();
        mV=v;mA=a;mW=w;
        Printf("v:%dmV a:%dmA w:%dmW\n",v,a,w);
    }
}
