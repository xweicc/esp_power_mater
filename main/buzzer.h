#ifndef __BUZZER_H__
#define __BUZZER_H__

enum{
    buzzerShort,
    buzzerLong,
};

typedef struct {
    int state;
    struct timer_list timer;
} buzzer_var_t;

void buzzer_init(void);
void buzzer_set(int state);

#endif
