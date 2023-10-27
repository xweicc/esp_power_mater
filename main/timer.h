
#ifndef __TIMER_H__
#define __TIMER_H__

#include "llist.h"

struct timer_list {
	struct list_head entry;	
	unsigned long expires;
	void (*function)(unsigned long);
	unsigned long data;
};

//a>b
#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))
#define time_before(a,b)	time_after(b,a)

//b>a
#define time_after_eq(a,b)	\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(a) - (long)(b) >= 0))
#define time_before_eq(a,b)	time_after_eq(b,a)


void  init_timer(struct timer_list * timer);
static inline void setup_timer(struct timer_list * timer,
				void (*function)(unsigned long),
				unsigned long data)
{
	timer->function = function;
	timer->data = data;
	init_timer(timer);
}



static inline int timer_pending(const struct timer_list * timer)
{
	return timer->entry.next != NULL;
}

int  init_timers_cpu(void);
void run_timers(void);

void add_timer(struct timer_list *timer);
int del_timer(struct timer_list * timer);
int __mod_timer(struct timer_list *timer, unsigned long expires);
int mod_timer(struct timer_list *timer, unsigned long expires);


#include <sdkconfig.h>
#if CONFIG_FREERTOS_HZ != 1000
#error "CONFIG_FREERTOS_HZ != 1000"
#else
#undef HZ
#define HZ CONFIG_FREERTOS_HZ
#endif

uint32_t xTaskGetTickCount(void);
#define jiffies ((unsigned long)xTaskGetTickCount())

#endif /*TIMER_H__*/

