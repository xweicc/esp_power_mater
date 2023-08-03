
#include "main.h"
#include "ina226.h"
#include "buzzer.h"
#include "oled.h"



void app_main(void)
{
    buzzer_init();
	xTaskCreate(buzzer_task, "buzzer_task", 2048, NULL, 3, NULL);
    
    buzzer_set(BUZZER_ON);
    vTaskDelay(50);
    buzzer_set(BUZZER_OFF);
    
    ina226_init();
	xTaskCreate(ina226_task, "ina226_task", 2048, NULL, 3, NULL);
    
    oled_init();
	xTaskCreate(oled_task, "oled_task", 2048, NULL, 3, NULL);
}

