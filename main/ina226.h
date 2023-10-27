#ifndef __INA226_H__
#define __INA226_H__

#define INA226_ADDR     0x40
#define INA226_ID_NUM   0x2260

#define INA226_REG_CONF 0x00    //Configuration Register
#define INA226_REG_SV   0x01    //Shunt Voltage Register
#define INA226_REG_BV   0x02    //Bus Voltage Register
#define INA226_REG_PWR  0x03    //Power Register
#define INA226_REG_CUR  0x04    //Current Register
#define INA226_REG_CAL  0x05    //Calibration Register
#define INA226_REG_ID   0xFF    //ID:2260

int ina226_init(void);
void ina226_task(void *param);
int ina226_set_cal(void);

#endif
