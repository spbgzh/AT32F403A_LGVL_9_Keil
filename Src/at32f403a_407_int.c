/**
 **************************************************************************
 * @file     at32f403a_407_int.c
 * @version  v2.0.9
 * @date     2022-04-25
 * @brief    main interrupt service routines.
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */

/* includes ------------------------------------------------------------------*/
#include "at32f403a_407_int.h"
#include "at32f403a_407.h"
#include "lvgl.h"
#include "timer.h"
#include "usart.h"

/** @addtogroup AT32F403A_periph_template
 * @{
 */

/** @addtogroup 403A_LED_toggle
 * @{
 */

/**
 * @brief  this function handles nmi exception.
 * @param  none
 * @retval none
 */
void NMI_Handler(void) {}

/**
 * @brief  this function handles hard fault exception.
 * @param  none
 * @retval none
 */
void HardFault_Handler(void) {
  /* go to infinite loop when hard fault exception occurs */
  while (1) {
  }
}

/**
 * @brief  this function handles memory manage exception.
 * @param  none
 * @retval none
 */
void MemManage_Handler(void) {
  /* go to infinite loop when memory manage exception occurs */
  while (1) {
  }
}

/**
 * @brief  this function handles bus fault exception.
 * @param  none
 * @retval none
 */
void BusFault_Handler(void) {
  /* go to infinite loop when bus fault exception occurs */
  while (1) {
  }
}

/**
 * @brief  this function handles usage fault exception.
 * @param  none
 * @retval none
 */
void UsageFault_Handler(void) {
  /* go to infinite loop when usage fault exception occurs */
  while (1) {
  }
}

/**
 * @brief  this function handles svcall exception.
 * @param  none
 * @retval none
 */
void SVC_Handler(void) {}

/**
 * @brief  this function handles debug monitor exception.
 * @param  none
 * @retval none
 */
void DebugMon_Handler(void) {}

/**
 * @brief  this function handles pendsv_handler exception.
 * @param  none
 * @retval none
 */
void PendSV_Handler(void) {}

/**
 * @brief  this function handles systick handler.
 * @param  none
 * @retval none
 */
void SysTick_Handler(void) {}

void USART1_IRQHandler(void) {
  uint8_t clear;
  if (usart_flag_get(USART1, USART_IDLEF_FLAG) != RESET) // USART1总线空闲
  {
    clear = USART1->sts; // USART1清除空闲中断标志位
    clear = USART1->dt;  // USART1清除空闲中断标志位
    clear &= 0;
    Receiver.state = 1; // USART1接收完成标志位

    Receiver.len = USART_RECEVIE_LEN - dma_data_number_get(DMA1_CHANNEL5);

    usartdmasend(Receiver.rxbuf, Receiver.len);
    Receiver.len = 0;
    Receiver.state = 0;

    usartdmarecv(Receiver.rxbuf, USART_RECEVIE_LEN);
  }
}
  /**
   * @}
   */

  /**
   * @}
   */
