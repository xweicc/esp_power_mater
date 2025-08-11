#ifndef __BUZZER_H__
#define __BUZZER_H__

enum{
    buzzerNone,
    buzzerShort,
    buzzerLong,
    buzzerFlash,
};

typedef struct {
    int state;
    int flash;
    struct timer_list timer;
} buzzer_var_t;

void buzzer_init(void);
void buzzer_set(int state);
void buzzer_stop(void);

#endif
