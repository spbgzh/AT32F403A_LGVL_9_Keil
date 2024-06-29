#ifndef __USERCODE_TO_BOOTLOADER_H
#define __USERCODE_TO_BOOTLOADER_H

#include "at32f403a_407.h"
#define BOOTLOADER_ADDRESS 0x1FFFB000
#define BKP_JUMP_FLAG 0x5AA5
#define SET_MSP(msp) __set_MSP(msp)

void app_jump_to_bootloader(void);

uint8_t bpr_check_flag(void);
void bpr_write_flag(void);
void UserSystemInit(void);

#endif
