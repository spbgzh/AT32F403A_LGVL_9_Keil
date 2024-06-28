#include "lcd.h"
#include "at32f403a_407.h"

uint8_t BLK_FALG;

void LCD_Writ_Bus(uint8_t dat) {
  LCD_CS_Clr();
  spi_i2s_data_transmit(LCD_SPIX, dat);
  while (spi_i2s_flag_get(LCD_SPIX, SPI_I2S_BF_FLAG)) {
  };
  LCD_CS_Set();
}

void LCD_WR_DATA8(uint8_t dat) { LCD_Writ_Bus(dat); }

void LCD_WR_DATA(uint16_t dat) {
  LCD_Writ_Bus(dat >> 8);
  LCD_Writ_Bus(dat);
}

void LCD_WR_REG(uint8_t dat) {
  LCD_DC_Clr();
  LCD_Writ_Bus(dat);
  LCD_DC_Set();
}

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  LCD_WR_REG(0x2a);
  LCD_WR_DATA(x1);
  LCD_WR_DATA(x2);
  LCD_WR_REG(0x2b);
  LCD_WR_DATA(y1);
  LCD_WR_DATA(y2);
  LCD_WR_REG(0x2c);
}

void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend,
              uint16_t color) {
  LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);
  for (uint16_t i = ysta; i < yend; i++) {
    for (uint16_t j = xsta; j < xend; j++) {
      LCD_WR_DATA(color);
    }
  }
}

static void LCD_SPI_Init(void) {
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);

  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_LCD_MOSI_PIN;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIO_LCD_MOSI_PORT, &gpio_init_struct);
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_LCD_CLK_PIN;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIO_LCD_CLK_PORT, &gpio_init_struct);

  spi_init_type spi_init_struct;
  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_2;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI1, &spi_init_struct);

  spi_i2s_dma_transmitter_enable(SPI1, TRUE);
  spi_enable(SPI1, TRUE);
}

static void LCD_GPIO_Init(void) {
  gpio_init_type gpio_init_struct;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_LCD_RES_PIN;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIO_LCD_RES_PORT, &gpio_init_struct);

  // lcd_dc
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_LCD_DC_PIN;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIO_LCD_DC_PORT, &gpio_init_struct);

  // lcd_cs
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_LCD_CS_PIN;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIO_LCD_CS_PORT, &gpio_init_struct);

  // lcd_blk
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_LCD_BLK_PIN;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIO_LCD_BLK_PORT, &gpio_init_struct);

  _GPIO_LCD_RES_H();
  _GPIO_LCD_DC_H();
  _GPIO_LCD_CS_H();
  _GPIO_LCD_BLK_L();
}

void ConvHL(uint8_t *s, int32_t l) {
  uint8_t v;
  while (l > 0) {
    v = *(s + 1);
    *(s + 1) = *s;
    *s = v;
    s += 2;
    l -= 2;
  }
}

void LCD_Init(void) {
  BLK_FALG = 1;
  LCD_GPIO_Init();
  LCD_SPI_Init();

  LCD_RES_Set();
  delay_ms(10);
  LCD_RES_Clr();
  delay_ms(10);
  LCD_RES_Set();
  delay_ms(120);

  LCD_BLK_Set();

  LCD_WR_REG(0xFE);
  LCD_WR_REG(0xEF);

  LCD_WR_REG(0xB0);
  LCD_WR_DATA8(0xC0);

  LCD_WR_REG(0xB1);
  LCD_WR_DATA8(0x80);

  LCD_WR_REG(0xB2);
  LCD_WR_DATA8(0x27);

  LCD_WR_REG(0xB3);
  LCD_WR_DATA8(0x13);

  LCD_WR_REG(0xB6);
  LCD_WR_DATA8(0x19);

  LCD_WR_REG(0xB7);
  LCD_WR_DATA8(0x05);

  LCD_WR_REG(0xAC);
  LCD_WR_DATA8(0xC8);

  LCD_WR_REG(0xAB);
  LCD_WR_DATA8(0x0f);

  LCD_WR_REG(0x3A);
  LCD_WR_DATA8(0x05);

  LCD_WR_REG(0xB4);
  LCD_WR_DATA8(0x04);

  LCD_WR_REG(0xA8);
  LCD_WR_DATA8(0x08);

  LCD_WR_REG(0xB8);
  LCD_WR_DATA8(0x08);

  LCD_WR_REG(0xEA);
  LCD_WR_DATA8(0x02);

  LCD_WR_REG(0xE8);
  LCD_WR_DATA8(0x2A);

  LCD_WR_REG(0xE9);
  LCD_WR_DATA8(0x47);

  LCD_WR_REG(0xE7);
  LCD_WR_DATA8(0x5F);

  LCD_WR_REG(0xC6);
  LCD_WR_DATA8(0x21);

  LCD_WR_REG(0xC7);
  LCD_WR_DATA8(0x15);

  LCD_WR_REG(0xF0);
  LCD_WR_DATA8(0x1D);
  LCD_WR_DATA8(0x38);
  LCD_WR_DATA8(0x09);
  LCD_WR_DATA8(0x4D);
  LCD_WR_DATA8(0x92);
  LCD_WR_DATA8(0x2F);
  LCD_WR_DATA8(0x35);
  LCD_WR_DATA8(0x52);
  LCD_WR_DATA8(0x1E);
  LCD_WR_DATA8(0x0C);
  LCD_WR_DATA8(0x04);
  LCD_WR_DATA8(0x12);
  LCD_WR_DATA8(0x14);
  LCD_WR_DATA8(0x1F);

  LCD_WR_REG(0xF1);
  LCD_WR_DATA8(0x16);
  LCD_WR_DATA8(0x40);
  LCD_WR_DATA8(0x1C);
  LCD_WR_DATA8(0x54);
  LCD_WR_DATA8(0xA9);
  LCD_WR_DATA8(0x2D);
  LCD_WR_DATA8(0x2E);
  LCD_WR_DATA8(0x56);
  LCD_WR_DATA8(0x10);
  LCD_WR_DATA8(0x0D);
  LCD_WR_DATA8(0x0C);
  LCD_WR_DATA8(0x1A);
  LCD_WR_DATA8(0x14);
  LCD_WR_DATA8(0x1E);

  LCD_WR_REG(0xF4);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0xFF);

  LCD_WR_REG(0xBA);
  LCD_WR_DATA8(0xFF);
  LCD_WR_DATA8(0xFF);

  LCD_WR_REG(0x36);
  LCD_WR_DATA8(0xc0);

  LCD_WR_REG(0x11);
  delay_ms(120);
  LCD_WR_REG(0x29);
}