#include "usart.h"
#include "stdio.h"
#include "string.h"

#if (__ARMCC_VERSION > 6000000)
__asm(".global __use_no_semihosting\n\t");
void _sys_exit(int x) { x = x; }
/* __use_no_semihosting was requested, but _ttywrch was */
void _ttywrch(int ch) { ch = ch; }
FILE __stdout;
#else
#ifdef __CC_ARM
#pragma import(__use_no_semihosting)
struct __FILE {
  int handle;
};
FILE __stdout;
void _sys_exit(int x) { x = x; }
#endif
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
  while (usart_flag_get(USART1, USART_TDBE_FLAG) == RESET)
    ;
  usart_data_transmit(USART1, ch);
  return ch;
}

struct Receiver_Struct Receiver;

void usart1_dma_init(uint32_t bound) {
  gpio_init_type gpio_init_struct;
  dma_init_type dma_init_struct;

  /*Enable the UART Clock*/
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE); // 开启GPIOA的时钟
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE); // 开启USART1的时钟
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);   // 开启DMA1的时钟

  gpio_default_para_init(&gpio_init_struct);
  /* Configure the UART1 TX pin */
  gpio_init_struct.gpio_drive_strength =
      GPIO_DRIVE_STRENGTH_STRONGER; // 较大电流推动/吸入能力
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL; // 推挽输出
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;             // 复用
  gpio_init_struct.gpio_pins = GPIO_PINS_9;               // PA9
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;            // 无上下拉
  gpio_init(GPIOA, &gpio_init_struct);

  /* Configure the UART1 RX pin */
  gpio_init_struct.gpio_drive_strength =
      GPIO_DRIVE_STRENGTH_STRONGER; // 较大电流推动/吸入能力
  gpio_init_struct.gpio_out_type =
      GPIO_OUTPUT_PUSH_PULL; // 推挽输出(输入模式,无效)
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT; // 输入模式
  gpio_init_struct.gpio_pins = GPIO_PINS_10;    // PA10
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;    // 上拉
  gpio_init(GPIOA, &gpio_init_struct);

  dma_reset(DMA1_CHANNEL4);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 0; // 内存大小
  dma_init_struct.direction =
      DMA_DIR_MEMORY_TO_PERIPHERAL;               // 外设地址为目的地址
  dma_init_struct.memory_base_addr = (uint32_t)0; // 内存地址
  dma_init_struct.memory_data_width =
      DMA_MEMORY_DATA_WIDTH_BYTE;           // 内存数据的宽度
  dma_init_struct.memory_inc_enable = TRUE; // 内存地址递增打开
  dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt; // 外设地址
  dma_init_struct.peripheral_data_width =
      DMA_PERIPHERAL_DATA_WIDTH_BYTE;             // 外设数据的宽度
  dma_init_struct.peripheral_inc_enable = FALSE;  // 外设地址递增关闭
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM; // 中等优先级
  dma_init_struct.loop_mode_enable = FALSE;       // 不循环
  dma_init(DMA1_CHANNEL4, &dma_init_struct);

  dma_reset(DMA1_CHANNEL5);
  dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt; // 外设地址
  dma_init_struct.memory_base_addr = (uint32_t)Receiver.rxbuf;  // 内存地址
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY; // 外设地址为源地址
  dma_init_struct.buffer_size = USART_RECEVIE_LEN;
  dma_init(DMA1_CHANNEL5, &dma_init_struct);

  usart_dma_receiver_enable(USART1, TRUE); // 使能串口dma接收
  dma_channel_enable(DMA1_CHANNEL5, TRUE); // 使能通道5

  nvic_irq_enable(USART1_IRQn, 0, 0); // 使能串口1中断,优先级0,次优先级0

  /*Configure UART param*/
  usart_init(USART1, bound, USART_DATA_8BITS,
             USART_STOP_1_BIT); // 波特率,8数据位,1停止位
  usart_hardware_flow_control_set(USART1,
                                  USART_HARDWARE_FLOW_NONE); // 无硬件流操作
  usart_parity_selection_config(USART1, USART_PARITY_NONE); // 无校验
  usart_transmitter_enable(USART1, TRUE);                   // 使能发送
  usart_receiver_enable(USART1, TRUE);                      // 使能接收

  usart_interrupt_enable(USART1, USART_IDLE_INT, TRUE); // 使能串口空闲中断
  usart_enable(USART1, TRUE);                           // 使能串口
}

void usartdmasend(uint8_t *data, uint16_t len) {

  DMA1_CHANNEL4->dtcnt = len;                 // 发送的数据长度
  DMA1_CHANNEL4->maddr = (uint32_t)data;      // 数据buf地址
  usart_dma_transmitter_enable(USART1, TRUE); // 开启USART1 DMA 发送
  dma_channel_enable(DMA1_CHANNEL4, TRUE);    // 开启通道4(开始发送)
  while (dma_flag_get(DMA1_FDT4_FLAG) == RESET)
    ;                                          // 等待传输完成
  dma_flag_clear(DMA1_FDT4_FLAG);              // 清标志
  dma_channel_enable(DMA1_CHANNEL4, FALSE);    // 关闭通道4
  usart_dma_transmitter_enable(USART1, FALSE); // 关闭USART1 DMA 发送
}
void usartdmarecv(uint8_t *data, uint16_t len) {

  dma_flag_clear(DMA1_FDT5_FLAG);           // 清标志
  dma_channel_enable(DMA1_CHANNEL5, FALSE); // 关闭USART1 DMA 接收
  usart_dma_receiver_enable(USART1, FALSE); // 关闭通道5
  DMA1_CHANNEL5->dtcnt = len;               // 接收的数据长度
  DMA1_CHANNEL5->maddr = (uint32_t)data;    // 存放数据buf地址
  usart_dma_receiver_enable(USART1, TRUE);  // 开启USART1 DMA 接收
  dma_channel_enable(DMA1_CHANNEL5, TRUE);  // 开启通道5(开始接收)
}
