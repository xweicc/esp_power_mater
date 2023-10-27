
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "llist.h"
#include "timer.h"


#define TVN_BITS 	6
#define TVR_BITS 	8
#define TVN_SIZE 	(1 << TVN_BITS)
#define TVR_SIZE 	(1 << TVR_BITS)
#define TVN_MASK 	(TVN_SIZE - 1)
#define TVR_MASK 	(TVR_SIZE - 1)

typedef struct tvec_s {
	struct list_head vec[TVN_SIZE];
} tvec_t;

typedef struct tvec_root_s {
	struct list_head vec[TVR_SIZE];
} tvec_root_t;

typedef struct tvec_t_base_s {
	
	struct timer_list 		*running_timer;
	unsigned long 			timer_jiffies;
	tvec_root_t 			tv1;
	tvec_t 				tv2;
	tvec_t 				tv3;
	tvec_t 				tv4;
	tvec_t 				tv5;
}tvec_base_t;

tvec_base_t boot_tvec_bases;


int  init_timers_cpu(void)
{
	int j;
	tvec_base_t *base = &boot_tvec_bases;
	memset((void*)base, 0, sizeof(*base));

	for (j = 0; j < TVN_SIZE; j++) {
		INIT_LIST_HEAD(base->tv5.vec + j);
		INIT_LIST_HEAD(base->tv4.vec + j);
		INIT_LIST_HEAD(base->tv3.vec + j);
		INIT_LIST_HEAD(base->tv2.vec + j);
	}
	for (j = 0; j < TVR_SIZE; j++)
		INIT_LIST_HEAD(base->tv1.vec + j);

	base->timer_jiffies = jiffies;
	return 0;
}


void  init_timer(struct timer_list *timer)
{
	timer->entry.next = NULL;	
}

static inline void detach_timer(struct timer_list *timer)
{
	struct list_head *entry = &timer->entry;

	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = LIST_POISON2;
}



int del_timer(struct timer_list *timer)
{
	int ret = 0;
			
	if (timer_pending(timer)) {
		detach_timer(timer);
		ret = 1;
	}	
	
	return ret;
}






static void internal_add_timer(tvec_base_t *base, struct timer_list *timer)
{
	unsigned long expires = timer->expires;
	unsigned long idx = expires - base->timer_jiffies;
	struct list_head *vec;

	if (idx < TVR_SIZE) {
		int i = expires & TVR_MASK;
		vec = base->tv1.vec + i;
	} else if (idx < 1 << (TVR_BITS + TVN_BITS)) {
		int i = (expires >> TVR_BITS) & TVN_MASK;
		vec = base->tv2.vec + i;
	} else if (idx < 1 << (TVR_BITS + 2 * TVN_BITS)) {
		int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
		vec = base->tv3.vec + i;
	} else if (idx < 1 << (TVR_BITS + 3 * TVN_BITS)) {
		int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
		vec = base->tv4.vec + i;
	} else if ((signed long) idx < 0) {
		/*
		 * Can happen if you add a timer with expires == jiffies,
		 * or you set a timer to go off in the past
		 */
		vec = base->tv1.vec + (base->timer_jiffies & TVR_MASK);
	} else {
		int i;
		/* If the timeout is larger than 0xffffffff on 64-bit
		 * architectures then we use the maximum timeout:
		 */
		if (idx > 0xffffffffUL) {
			idx = 0xffffffffUL;
			expires = idx + base->timer_jiffies;
		}
		i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
		vec = base->tv5.vec + i;
	}
	/*
	 * Timers are FIFO:
	 */
	list_add_tail(&timer->entry, vec);
}


static int cascade(tvec_base_t *base, tvec_t *tv, int index)
{
	/* cascade all the timers from tv up one level */
	struct timer_list *timer, *tmp;
	struct list_head tv_list;

	list_replace_init(tv->vec + index, &tv_list);

	/*
	 * We are removing _all_ timers from the list, so we
	 * don't have to detach them individually.
	 */
	list_for_each_entry_safe(timer, tmp, &tv_list, entry) {		
		internal_add_timer(base, timer);
	}

	return index;
}

#define INDEX(N) ((base->timer_jiffies >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

/**
 * __run_timers - run all expired timers (if any) on this CPU.
 * @base: the timer vector to be processed.
 *
 * This function cascades all vectors and executes all expired timer
 * vectors.
 */
static inline void __run_timers(tvec_base_t *base)
{
	struct timer_list *timer;
	
	while (time_after_eq(jiffies, base->timer_jiffies)) {
		struct list_head work_list;
		struct list_head *head = &work_list;
 		int index = base->timer_jiffies & TVR_MASK;

		/*
		 * Cascade timers:
		 */
		if (!index &&
			(!cascade(base, &base->tv2, INDEX(0))) &&
				(!cascade(base, &base->tv3, INDEX(1))) &&
					!cascade(base, &base->tv4, INDEX(2)))
			cascade(base, &base->tv5, INDEX(3));
		++base->timer_jiffies;
		list_replace_init(base->tv1.vec + index, &work_list);
		while (!list_empty(head)) {
			void (*fn)(unsigned long);
			unsigned long data;

			timer = list_entry(head->next,struct timer_list,entry);
 			fn = timer->function;
 			data = timer->data;			
			detach_timer(timer);
			if(fn){
				fn(data);
			}else{
				printf("[run_timers] timer:%p not init!\n",timer);
			}
		}
	}
	
}


void run_timers(void)
{
	__run_timers(&boot_tvec_bases);
}
	


int __mod_timer(struct timer_list *timer, unsigned long expires)
{
	int ret = 0;
	if (timer_pending(timer)) {
		detach_timer(timer);
		ret = 1;
	}
	
	timer->expires = expires;
	internal_add_timer(&boot_tvec_bases, timer);
	return ret;
}

void add_timer(struct timer_list *timer)
{	
	__mod_timer(timer,timer->expires);
}

int mod_timer(struct timer_list *timer, unsigned long expires)
{	
	/*
	 * This is a common optimization triggered by the
	 * networking code - if the timer is re-modified
	 * to be the same thing then just return:
	 */
	if (timer->expires == expires && timer_pending(timer))
		return 1;

	if(!timer->function){
		printf("[mod_timer] timer:%p not init!\n",timer);
		return -1;
	}
	return __mod_timer(timer, expires);
}

