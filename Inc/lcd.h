#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "delay.h"
#include "stdint.h"

#define USE_HORIZONTAL 1

extern uint8_t BLK_FALG;

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 128
#define LCD_H 128

#else
#define LCD_W 128
#define LCD_H 128
#endif

// SPI1
// SPI1_MOSI
#define GPIO_LCD_MOSI_PIN GPIO_PINS_7
#define GPIO_LCD_MOSI_PORT GPIOA
// SPI1_CLK
#define GPIO_LCD_CLK_PIN GPIO_PINS_5
#define GPIO_LCD_CLK_PORT GPIOA

// lcd_res
#define GPIO_LCD_RES_PIN GPIO_PINS_0
#define GPIO_LCD_RES_PORT GPIOB
#define _GPIO_LCD_RES_H() gpio_bits_set(GPIO_LCD_RES_PORT, GPIO_LCD_RES_PIN)
#define _GPIO_LCD_RES_L() gpio_bits_reset(GPIO_LCD_RES_PORT, GPIO_LCD_RES_PIN)
// lcd_dc
#define GPIO_LCD_DC_PIN GPIO_PINS_6
#define GPIO_LCD_DC_PORT GPIOA
#define _GPIO_LCD_DC_H() gpio_bits_set(GPIO_LCD_DC_PORT, GPIO_LCD_DC_PIN)
#define _GPIO_LCD_DC_L() gpio_bits_reset(GPIO_LCD_DC_PORT, GPIO_LCD_DC_PIN)
// lcd_cs
#define GPIO_LCD_CS_PIN GPIO_PINS_2
#define GPIO_LCD_CS_PORT GPIOB
#define _GPIO_LCD_CS_H() gpio_bits_set(GPIO_LCD_CS_PORT, GPIO_LCD_CS_PIN)
#define _GPIO_LCD_CS_L() gpio_bits_reset(GPIO_LCD_CS_PORT, GPIO_LCD_CS_PIN)
// lcd_blk
#define GPIO_LCD_BLK_PIN GPIO_PINS_3
#define GPIO_LCD_BLK_PORT GPIOA
#define _GPIO_LCD_BLK_H() gpio_bits_set(GPIO_LCD_BLK_PORT, GPIO_LCD_BLK_PIN)
#define _GPIO_LCD_BLK_L() gpio_bits_reset(GPIO_LCD_BLK_PORT, GPIO_LCD_BLK_PIN)

#define LCD_RES_Clr() _GPIO_LCD_RES_L()
#define LCD_RES_Set() _GPIO_LCD_RES_H()
#define LCD_DC_Clr() _GPIO_LCD_DC_L()
#define LCD_DC_Set() _GPIO_LCD_DC_H()
#define LCD_CS_Clr() _GPIO_LCD_CS_L()
#define LCD_CS_Set() _GPIO_LCD_CS_H()
#define LCD_BLK_Clr() _GPIO_LCD_BLK_L()
#define LCD_BLK_Set() _GPIO_LCD_BLK_H()
#define LCD_SPIX SPI1

void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend,
              uint16_t color);
void LCD_Init(void);
void ConvHL(uint8_t *s, int32_t l);

#endif
