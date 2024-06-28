/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#include "at32f403a_407_clock.h"

#include "delay.h"
#include "lcd.h"
#include "timer.h"
#include "lv_port_disp_template.h"
#include "lvgl.h"
#include "ui.h"
/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void)
{
  system_clock_config();
  delay_init();
	System_Timer_Init();
	
	lv_init();
  lv_port_disp_init();
  ui_init();
  while (1)
  {
		 lv_timer_periodic_handler();
  }
}

/**
 * @}
 */

/**
 * @}
 */
