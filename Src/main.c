/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#include "at32f403a_407_clock.h"
#include "lcd.h"
#include "lv_port_disp_template.h"
#include "lvgl.h"
#include "spim.h"
#include "stdio.h"
#include "timer.h"
#include "ui.h"
#include "usart.h"
#include "usercode_to_bootloader.h"
/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void) {
  UserSystemInit();
  system_clock_config();
  delay_init();
  usart1_dma_init(115200);
  spim_init();
  
  lv_tick_timer_init();
  lv_init();
  lv_port_disp_init();
  ui_init();
  while (1) {
    lv_timer_periodic_handler();
  }
}

/**
 * @}
 */

/**
 * @}
 */
