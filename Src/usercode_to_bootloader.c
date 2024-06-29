#include "usercode_to_bootloader.h"
void (*pfTarget)(void);
void bpr_write_flag(void) {
  /* enable pwc and bpr clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);
  /* enable write access to bpr domain */
  pwc_battery_powered_domain_access(TRUE);
  /* clear tamper pin event pending flag */
  bpr_flag_clear(BPR_TAMPER_EVENT_FLAG);
  bpr_data_write(BPR_DATA1, BKP_JUMP_FLAG);
  pwc_battery_powered_domain_access(FALSE);
  /*System Reset*/
  NVIC_SystemReset();
}

uint8_t bpr_check_flag(void) {
  uint8_t ret_val = 0;
  /* enable pwc and bpr clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);
  /* enable write access to bpr domain */
  pwc_battery_powered_domain_access(TRUE);
  /* clear tamper pin event pending flag */
  bpr_flag_clear(BPR_TAMPER_EVENT_FLAG);
  if (bpr_data_read(BPR_DATA1) == BKP_JUMP_FLAG) {
    bpr_data_write(BPR_DATA1, 0x00); // write 00 to bkp
    ret_val = 1;
  }
  pwc_battery_powered_domain_access(FALSE);
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, FALSE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, FALSE);
  return ret_val;
}

void app_jump_to_bootloader(void) {
  uint32_t dwStkPtr, dwJumpAddr;
  dwStkPtr = *(uint32_t *)BOOTLOADER_ADDRESS;
  dwJumpAddr = *(uint32_t *)(BOOTLOADER_ADDRESS + sizeof(uint32_t));

  /*跳转之前,需要保证所有外设Clock 关闭,PLL
   * 关闭,关闭所有中断,清除所有的中断挂起标志*/
  SET_MSP(dwStkPtr);
  pfTarget = (void (*)(void))dwJumpAddr;
  pfTarget();
}

void UserSystemInit(void) {
#if defined(__FPU_USED) && (__FPU_USED == 1U)
  SCB->CPACR |= ((3U << 10U * 2U) | /* set cp10 full access */
                 (3U << 11U * 2U)); /* set cp11 full access */
#endif
  /*check if need to go into bootloader*/
  if (bpr_check_flag() == 1) {
    app_jump_to_bootloader();
  }
  /* reset the crm clock configuration to the default reset state(for debug
   * purpose) */
  /* set hicken bit */
  CRM->ctrl_bit.hicken = TRUE;
}
