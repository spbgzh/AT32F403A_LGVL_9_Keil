#include "rtc.h"

calendar_type calendar;

/* monthly correction data sheet */
const uint8_t table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};

/* monmonth data table of common year */
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void rtc_init(calendar_type *calendar) {
  /* enable pwc and bpr clocks */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);

  /* enable the battery-powered domain write operations */
  pwc_battery_powered_domain_access(TRUE);

  /* reset battery-powered domain register */
  bpr_reset();

  /* enable the lext osc */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);
  /* wait lext is ready */
  while (crm_flag_get(CRM_HEXT_STABLE_FLAG) == RESET)
    ;
  /* select the rtc clock source */
  crm_rtc_clock_select(CRM_RTC_CLOCK_HEXT_DIV);

  /* enable rtc clock */
  crm_rtc_clock_enable(TRUE);

  /* wait for rtc registers update */
  rtc_wait_update_finish();

  /* wait for the register write to complete */
  rtc_wait_config_finish();

  /* enable the rtc second */
  rtc_interrupt_enable(RTC_TS_INT, TRUE);

  /* wait for the register write to complete */
  rtc_wait_config_finish();

  /* set rtc divider: set rtc period to 1sec */
  rtc_divider_set(8000000 / 128 - 1);

  /* wait for the register write to complete */
  rtc_wait_config_finish();

  /* set time */
  rtc_time_set(calendar);

  /* enable the rtc interrupt */
  nvic_irq_enable(RTC_IRQn, 0, 2);

  /* tamper pin disabled */
  bpr_tamper_pin_enable(FALSE);

  /* enable the rtc second output on tamper pin */
  bpr_rtc_output_select(BPR_RTC_OUTPUT_SECOND);
}

/**
  * @brief  judeg the leap year or common year.
  *         - month       1  2  3  4  5  6  7  8  9  10 11 12
  *         - leap year   31 29 31 30 31 30 31 31 30 31 30 31
  *         - common year 31 28 31 30 31 30 31 31 30 31 30 31
  * @param  year
  * @retval 1: leap year
            2: common year
  */
uint8_t is_leap_year(uint16_t year) {
  if (year % 4 == 0) {
    if (year % 100 == 0) {
      if (year % 400 == 0) {
        return 1;
      } else {
        return 0;
      }
    } else {
      return 1;
    }
  } else {
    return 0;
  }
}

/**
 * @brief  set time. convert the input clock to a second.
 *         the time basic : 1970.1.1
 *         legitimate year: 1970 ~ 2099
 * @param  calendar
 * @retval 0: set time right.
 *         1: set time failed.
 */
uint8_t rtc_time_set(calendar_type *calendar) {
  uint32_t t;
  uint32_t seccount = 0;

  if (calendar->year < 1970 || calendar->year > 2099) {
    return 1;
  }

  for (t = 1970; t < calendar->year; t++) {
    if (is_leap_year(t)) {
      seccount += 31622400;
    } else {
      seccount += 31536000;
    }
  }

  calendar->month -= 1;

  for (t = 0; t < calendar->month; t++) {
    seccount += (uint8_t)mon_table[t] * 86400;

    if (is_leap_year(calendar->year) && t == 1) {
      seccount += 86400;
    }
  }

  seccount += (uint8_t)(calendar->date - 1) * 86400;

  seccount += (uint8_t)calendar->hour * 3600;

  seccount += (uint8_t)calendar->min * 60;

  seccount += calendar->sec;

  /* enable pwc and bpr clocks */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);

  /* enable write access to bpr domain */
  pwc_battery_powered_domain_access(TRUE);

  /* set the rtc counter value */
  rtc_counter_set(seccount);

  /* wait for the register write to complete */
  rtc_wait_config_finish();

  return 0;
}

/**
 * @brief  set rtc alarm clock.
 *         the time basic : 1970.1.1
 *         legitimate year: 1970 ~ 2099
 * @param  calendar
 * @retval 0: set alarm right.
 *         1: set alarm failed.
 */
uint8_t rtc_alarm_clock_set(calendar_type *calendar) {
  uint16_t t;
  uint32_t seccount = 0;

  if (calendar->year < 1970 || calendar->year > 2099) {
    return 1;
  }

  for (t = 1970; t < calendar->year; t++) {
    if (is_leap_year(t)) {
      seccount += 31622400;
    } else {
      seccount += 31536000;
    }
  }

  calendar->month -= 1;

  for (t = 0; t < calendar->month; t++) {
    seccount += (uint8_t)mon_table[t] * 86400;

    if (is_leap_year(calendar->year) && t == 1) {
      seccount += 86400;
    }
  }

  seccount += (uint8_t)(calendar->date - 1) * 86400;
  seccount += (uint8_t)calendar->hour * 3600;
  seccount += (uint8_t)calendar->min * 60;
  seccount += calendar->sec;

  /* enable pwc and bpr clocks */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);

  /* enable write access to bpr domain */
  pwc_battery_powered_domain_access(TRUE);

  /* set the rtc counter value */
  rtc_alarm_set(seccount);

  /* wait for the register write to complete */
  rtc_wait_config_finish();

  return 0;
}

/**
 * @brief  get current week by input leap year\mouth\day.
 * @param  year : year
 * @param  mon  : month
 * @param  day  : day
 * @retval week number.
 */
uint8_t rtc_week_get(uint16_t year, uint8_t month, uint8_t day) {
  uint16_t temp2;
  uint8_t yearh, yearl;

  yearh = year / 100;
  yearl = year % 100;

  if (yearh > 19) {
    yearl += 100;
  }

  temp2 = yearl + yearl / 4;
  temp2 = temp2 % 7;
  temp2 = temp2 + day + table_week[month - 1];

  if (yearl % 4 == 0 && month < 3) {
    temp2--;
  }

  return (temp2 % 7);
}

/**
 * @brief  get current time.
 * @param  none.
 * @retval none.
 */
void rtc_time_get(void) {
  static uint16_t daycnt = 0;
  uint32_t timecount = 0;
  uint32_t temp = 0;
  uint32_t temp1 = 0;

  timecount = rtc_counter_get();
  temp = timecount / 86400;

  if (daycnt != temp) {
    daycnt = temp;
    temp1 = 1970;

    while (temp >= 365) {
      if (is_leap_year(temp1)) {
        if (temp >= 366) {
          temp -= 366;
        } else {
          temp1++;
          break;
        }
      } else {
        temp -= 365;
      }

      temp1++;
    }

    calendar.year = temp1;
    temp1 = 0;

    while (temp >= 28) {
      if (is_leap_year(calendar.year) && temp1 == 1) {
        if (temp >= 29) {
          temp -= 29;
        } else {
          break;
        }
      } else {
        if (temp >= mon_table[temp1]) {
          temp -= mon_table[temp1];
        } else {
          break;
        }
      }

      temp1++;
    }

    calendar.month = temp1 + 1;
    calendar.date = temp + 1;
  }

  temp = timecount % 86400;
  calendar.hour = temp / 3600;
  calendar.min = (temp % 3600) / 60;
  calendar.sec = (temp % 3600) % 60;
  calendar.week = rtc_week_get(calendar.year, calendar.month, calendar.date);
}
