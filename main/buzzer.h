#ifndef __BUZZER_H__
#define __BUZZER_H__

enum{
    BUZZER_IDLE,
    BUZZER_OFF,
    BUZZER_ON,
    BUZZER_1HZ,
};

void buzzer_init(void);
void buzzer_task(void *param);
void buzzer_set(int state);

#endif
