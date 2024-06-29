#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f403a_407.h"

typedef struct {
  __IO uint16_t year; /*!< specifies the years of calendar.   */
  __IO uint8_t month; /*!< specifies the months of calendar.  */
  __IO uint8_t date;  /*!< specifies the date of calendar.    */
  __IO uint8_t hour;  /*!< specifies the hours of calendar.   */
  __IO uint8_t min;   /*!< specifies the minutes of calendar. */
  __IO uint8_t sec;   /*!< specifies the second of calendar.  */
  __IO uint8_t week;  /*!< specifies the weeks of calendar.   */
} calendar_type;

extern calendar_type calendar;

void rtc_init(calendar_type *calendar);
uint8_t rtc_time_set(calendar_type *calendar);
uint8_t rtc_alarm_clock_set(calendar_type *calendar);
void rtc_time_get(void);

#ifdef __cplusplus
}
#endif

#endif
