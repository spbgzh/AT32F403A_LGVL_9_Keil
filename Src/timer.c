#include "timer.h"
#include "lvgl.h"
#include <at32f403a_407.h>

void lv_tick_timer_init(void) {
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  tmr_base_init(TMR1, 10 - 1, 18000 - 1);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);
  nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);
  tmr_counter_enable(TMR1, TRUE);
}

__IO uint16_t tmr3cut;
void timer_start(void) {
  tmr3cut = 0;
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE); // 开始tmr3时钟
  // 设置TMR3的预分频为180=179+1；重载值1000=999+1；参数要减去1
  tmr_base_init(TMR3, 999, 179);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP); // 设置为向上计数方式
  nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);
  tmr_interrupt_enable(TMR3, TMR_OVF_INT, TRUE);
  tmr_counter_enable(TMR3, TRUE);
}

uint16_t timer_end(void) {
  tmr_counter_enable(TMR3, FALSE);
  return tmr3cut; // 返回tmr3的计数值，即时间片的时间
}

void TMR3_GLOBAL_IRQHandler(void) {
  if (tmr_flag_get(TMR3, TMR_OVF_FLAG) != RESET) {
    tmr_flag_clear(TMR3, TMR_OVF_FLAG);
    tmr3cut++;
  }
}

void TMR1_OVF_TMR10_IRQHandler(void) {
  if (tmr_flag_get(TMR1, TMR_OVF_FLAG) != RESET) {
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
    lv_tick_inc(1);
  }
}
