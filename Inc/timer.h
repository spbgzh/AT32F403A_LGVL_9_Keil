#ifndef _SYSTEM_TIMER_H_
#define _SYSTEM_TIMER_H_

#include <stdint.h>

void lv_tick_timer_init(void);
void timer_start(void);
uint16_t timer_end(void);

#endif