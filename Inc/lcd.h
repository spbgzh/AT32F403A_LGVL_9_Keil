#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stdint.h"
#include "spi.h"
#include "delay.h"
#include "dma.h"
#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280

#else
#define LCD_W 280
#define LCD_H 240
#endif

//lcd屏幕
//lcd_res
#define GPIO_LCD_RES_PIN        GPIO_PINS_0
#define GPIO_LCD_RES_PORT       GPIOB
#define _GPIO_LCD_RES_H()       gpio_bits_set(GPIO_LCD_RES_PORT, GPIO_LCD_RES_PIN)
#define _GPIO_LCD_RES_L()       gpio_bits_reset(GPIO_LCD_RES_PORT, GPIO_LCD_RES_PIN)
//lcd_dc
#define GPIO_LCD_DC_PIN         GPIO_PINS_1
#define GPIO_LCD_DC_PORT        GPIOB
#define _GPIO_LCD_DC_H()        gpio_bits_set(GPIO_LCD_DC_PORT, GPIO_LCD_DC_PIN)
#define _GPIO_LCD_DC_L()        gpio_bits_reset(GPIO_LCD_DC_PORT, GPIO_LCD_DC_PIN)
//lcd_cs
#define GPIO_LCD_CS_PIN         GPIO_PINS_2
#define GPIO_LCD_CS_PORT        GPIOB
#define _GPIO_LCD_CS_H()        gpio_bits_set(GPIO_LCD_CS_PORT, GPIO_LCD_CS_PIN)
#define _GPIO_LCD_CS_L()        gpio_bits_reset(GPIO_LCD_CS_PORT, GPIO_LCD_CS_PIN)
//lcd_blk
#define GPIO_LCD_BLK_PIN        GPIO_PINS_10
#define GPIO_LCD_BLK_PORT       GPIOB
#define _GPIO_LCD_BLK_H()       gpio_bits_set(GPIO_LCD_BLK_PORT, GPIO_LCD_BLK_PIN)
#define _GPIO_LCD_BLK_L()       gpio_bits_reset(GPIO_LCD_BLK_PORT, GPIO_LCD_BLK_PIN)

#define LCD_RES_Clr()   _GPIO_LCD_RES_L()
#define LCD_RES_Set()   _GPIO_LCD_RES_H()
#define LCD_DC_Clr()    _GPIO_LCD_DC_L()
#define LCD_DC_Set()    _GPIO_LCD_DC_H()
#define LCD_CS_Clr()    _GPIO_LCD_CS_L()
#define LCD_CS_Set()    _GPIO_LCD_CS_H()
#define LCD_BLK_Clr()   _GPIO_LCD_BLK_L()
#define LCD_BLK_Set()   _GPIO_LCD_BLK_H()
#define LCD_SPIX        SPI1

void LCD_Writ_Bus(uint8_t dat); //总线发送数据
void LCD_WR_DATA8(uint8_t dat); //写入一个字节
void LCD_WR_DATA(uint16_t dat); //写入两个字节
void LCD_WR_REG(uint8_t dat);   //写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void LCD_Init(void);//LCD初始化
void ConvHL(uint8_t *s, int32_t l);

#endif
