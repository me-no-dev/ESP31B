/* 
  twi.h - FRC TIMER and CCOMPARE library for ESP31B

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP31B core for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef ESP_TIMER_H_
#define ESP_TIMER_H_
#include "Arduino.h"

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//TIMER1
//timer dividers
#define TIM_DIV1   0 //80MHz (80 ticks/us - 104857.588 us max)
#define TIM_DIV16  1 //5MHz (5 ticks/us - 1677721.4 us max)
#define TIM_DIV265 2 //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
//timer int_types
#define TIM_LEVEL 1
#define TIM_EDGE  0
//timer reload values
#define TIM_SINGLE  0 //on interrupt routine you need to write a new value to start the timer again
#define TIM_LOOP  1 //on interrupt the counter will start with the same value again

#define timer1_read()           (T1V)
#define timer1_enabled()        ((T1C & (1 << TCTE)) != 0)
#define timer1_interrupted()    ((T1C & (1 << TCIS)) != 0)

typedef void(*timercallback)(void);

void timer1_isr_init(void);
void timer1_enable(uint8_t divider, uint8_t int_type, uint8_t reload);
void timer1_disable(void);
void timer1_attachInterrupt(timercallback userFunc);
void timer1_detachInterrupt(void);
void timer1_write(uint32_t ticks); //maximum ticks 8388607


// timer0 is a special CPU timer that has very high resolution but with
// limited control.
// it uses CCOUNT (ESP.GetCycleCount()) as the non-resetable timer counter
// it does not support divide, type, or reload flags
// it is auto-disabled when the compare value matches CCOUNT
// it is auto-enabled when the compare value changes
#define timer0_read() xthal_get_ccount()
#define timer0_write(count) xthal_set_ccompare(1, count)

void timer0_isr_init(void);
void timer0_attachInterrupt(timercallback userFunc);
void timer0_detachInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif