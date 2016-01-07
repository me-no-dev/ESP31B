/*
  timer.c - Timer1 library for ESP31B

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
#include "wiring_private.h"
#include "pins_arduino.h"

#include "c_types.h"
#include "esp_common.h"

#define ETS_FRC1_INTR_ENABLE()          xt_ints_on(1 << ETS_FRC_TIMER1_INUM)
#define ETS_FRC1_INTR_DISABLE()         xt_ints_off(1 << ETS_FRC_TIMER1_INUM)
#define ETS_FRC_TIMER1_INTR_ATTACH(fn,arg) do { \
  intrMatrixAttach(ETS_FRC_TIMER1_SOURCE, ETS_FRC_TIMER1_INUM); \
  xt_set_interrupt_handler(ETS_FRC_TIMER1_INUM, fn, arg); \
} while(0)

static volatile timercallback timer1_user_cb = NULL;

void timer1_attachInterrupt(timercallback userFunc) {
  timer1_user_cb = userFunc;
  ETS_FRC1_INTR_ENABLE();
}

void timer1_detachInterrupt() {
  timer1_user_cb = 0;
  ETS_FRC1_INTR_DISABLE();
}

void timer1_disable(){
  T1C = 0;
  T1I = 0;
}

void timer1_isr_handler(void *para){
  if((T1C & (1 << TCAR)) == 0){
    T1C &= ~(1 << TCTE);
  }
  T1I = 0;
  if (timer1_user_cb)
    timer1_user_cb();
}

void timer1_enable(uint8_t divider, uint8_t int_type, uint8_t reload){
  T1C = (1 << TCTE) | ((reload & 1) << TCAR) | ((divider & 3) << TCPD) | ((int_type & 1) << TCIT);
  T1I = 0;
}

void timer1_write(uint32_t ticks){
  T1L = ((ticks)& 0x7FFFFF);
  T1I = 0;
  T1C |= (1 << TCTE);
}

void timer1_isr_init(){
  ETS_FRC_TIMER1_INTR_ATTACH(timer1_isr_handler, NULL);
}


//-------------------------------------------------------------------
// timer 0

static volatile timercallback timer0_user_cb = NULL;

void timer0_isr_handler(void* para){
  if (timer0_user_cb)
    timer0_user_cb();
}

void timer0_isr_init(){
  xt_set_interrupt_handler(XCHAL_TIMER1_INTERRUPT, timer0_isr_handler, NULL);
}

void timer0_attachInterrupt(timercallback userFunc) {
  timer0_user_cb = userFunc;
  xt_ints_on(1<<XCHAL_TIMER1_INTERRUPT);
}

void timer0_detachInterrupt() {
  timer0_user_cb = NULL;
  xt_ints_off(1<<XCHAL_TIMER1_INTERRUPT);
}
