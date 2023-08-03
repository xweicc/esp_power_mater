#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <time.h>
#include <driver/i2c.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>


#define Printf(format,args...) do{\
        printf("[%s:%d]:"format,__ASSERT_FUNC,__LINE__,##args);\
    }while(0)



#endif

