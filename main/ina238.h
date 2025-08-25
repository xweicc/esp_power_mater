#ifndef __INA238_H__
#define __INA238_H__

#define INA238_ADDR     0x40
#define INA238_ID_NUM   0x2381

#define INA238_REG_CONF     0x00    //配置
#define INA238_REG_ADC      0x01    //ADC配置
#define INA238_REG_CAL      0x02    //分流校准
#define INA238_REG_VSHUNT   0x04    //分流电压测量
#define INA238_REG_VBUS     0x05    //总线电压测量
#define INA238_REG_CURRENT  0x07    //电流结果
#define INA238_REG_PWR      0x08    //功率结果
#define INA238_REG_ID       0x3F    //器件ID

int ina238_init(void);
void ina238_task(void *param);
void ina238_timer_fun(unsigned long data);

#endif
