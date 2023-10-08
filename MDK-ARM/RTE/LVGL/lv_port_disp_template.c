/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>
#include "lcd.h"
#include "dma.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    280
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    240
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_t * disp, const lv_area_t * area, lv_color_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_disp_t * disp;
void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    disp = lv_disp_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_disp_set_flush_cb(disp, disp_flush);

    /* Example 1
     * One buffer for partial rendering*/
    //static lv_color_t buf_1_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    //lv_disp_set_draw_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISP_RENDER_MODE_PARTIAL);

    /* Example 2
     * Two buffers for partial rendering
     * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 40];
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 40];
    lv_disp_set_draw_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISP_RENDER_MODE_PARTIAL);

    /* Example 3
     * Two buffers screen sized buffer for double buffering.
     * Both LV_DISP_RENDER_MODE_DIRECT and LV_DISP_RENDER_MODE_FULL works, see their comments*/
    //static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];
    //static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];
    //lv_disp_set_draw_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISP_RENDER_MODE_DIRECT);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    LCD_Init();
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_disp_t * disp_drv, const lv_area_t * area, lv_color_t * px_map)
{
   if(disp_flush_enabled)
    {
				int32_t x=area->x2-area->x1+1;
				int32_t y=area->y2-area->y1+1;
        LCD_Address_Set(area->x1, area->y1, area->x2, area->y2);
        LCD_CS_Clr();
			
				ConvHL((uint8_t *)px_map, 2*x*y);
			
        Dma1_Ch3_Config((uint32_t)px_map, (uint32_t)SPI1+0x0C, 2*x*y);
    }
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
