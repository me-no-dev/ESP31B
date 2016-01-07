/*
  pwm.c - analogWrite implementation for ESP31B

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
#include "esp_common.h"

#define LEDC_HSTIMER_CONF(t)          (DR_REG_LEDC_BASE + 0x0144 + (t * 8))
#define LEDC_HSTIMER_VALUE(t)         (DR_REG_LEDC_BASE + 0x0148 + (t * 8))
#define LEDC_HSTIMER_PAUSE            LEDC_HSTIMER0_PAUSE
#define LEDC_HSTIMER_RST              LEDC_HSTIMER0_RST
#define LEDC_TICK_ALWAYS_ON_HSTIMER   LEDC_TICK_ALWAYS_ON_HSTIMER0
#define LEDC_DIV_NUM_HSTIMER          LEDC_DIV_NUM_HSTIMER0
#define LEDC_DIV_NUM_HSTIMER_S        LEDC_DIV_NUM_HSTIMER0_S
#define LEDC_HSTIMER_LIM              LEDC_HSTIMER0_LIM
#define LEDC_HSTIMER_LIM_S            LEDC_HSTIMER0_LIM_S

#define LEDC_HSCH_CONF0(c)      (DR_REG_LEDC_BASE + 0x0000 + (c * 0x14))
#define LEDC_IDLE_LV_HSCH       LEDC_IDLE_LV_HSCH0
#define LEDC_IDLE_LV_HSCH_S     LEDC_IDLE_LV_HSCH0_S
#define LEDC_SIG_OUT_EN_HSCH    LEDC_SIG_OUT_EN_HSCH0
#define LEDC_SIG_OUT_EN_HSCH_S  LEDC_SIG_OUT_EN_HSCH0_S
#define LEDC_TIMER_SEL_HSCH     LEDC_TIMER_SEL_HSCH0
#define LEDC_TIMER_SEL_HSCH_S   LEDC_TIMER_SEL_HSCH0_S

#define LEDC_HSCH_CONF1(c)      (DR_REG_LEDC_BASE + 0x000c + (c * 0x14))
#define LEDC_DUTY_START_HSCH    LEDC_DUTY_START_HSCH0
#define LEDC_DUTY_START_HSCH_S  LEDC_DUTY_START_HSCH0_S
#define LEDC_DUTY_INC_HSCH      LEDC_DUTY_INC_HSCH0
#define LEDC_DUTY_INC_HSCH_S    LEDC_DUTY_INC_HSCH0_S
#define LEDC_DUTY_NUM_HSCH      LEDC_DUTY_NUM_HSCH0
#define LEDC_DUTY_NUM_HSCH_S    LEDC_DUTY_NUM_HSCH0_S
#define LEDC_DUTY_CYCLE_HSCH    LEDC_DUTY_CYCLE_HSCH0
#define LEDC_DUTY_CYCLE_HSCH_S  LEDC_DUTY_CYCLE_HSCH0_S
#define LEDC_DUTY_SCALE_HSCH    LEDC_DUTY_SCALE_HSCH0
#define LEDC_DUTY_SCALE_HSCH_S  LEDC_DUTY_SCALE_HSCH0_S

#define LEDC_HSCH_HPOINT(c)     (DR_REG_LEDC_BASE + 0x0004 + (c * 0x14))
#define LEDC_HPOINT_HSCH        LEDC_HPOINT_HSCH0
#define LEDC_HPOINT_HSCH_S      LEDC_HPOINT_HSCH0_S

#define LEDC_HSCH_DUTY(c)       (DR_REG_LEDC_BASE + 0x0008 + (c * 0x14))
#define LEDC_HSCH_DUTY_R(c)     (DR_REG_LEDC_BASE + 0x0010 + (c * 0x14))
#define LEDC_DUTY_HSCH          LEDC_DUTY_HSCH0
#define LEDC_DUTY_HSCH_S        LEDC_DUTY_HSCH0_S

#define HSTIMER_PAUSE(t)        SET_PERI_REG_MASK(LEDC_HSTIMER_CONF(t), LEDC_HSTIMER_PAUSE)//timer 0-3 Pause
#define HSTIMER_RESUME(t)       CLEAR_PERI_REG_MASK(LEDC_HSTIMER_CONF(t), LEDC_HSTIMER_PAUSE)//timer 0-3 Resume
#define HSTIMER_STOP(t)         SET_PERI_REG_MASK(LEDC_HSTIMER_CONF(t), LEDC_HSTIMER_RST)//timer 0-3 Stop
#define HSTIMER_START(t)        CLEAR_PERI_REG_MASK(LEDC_HSTIMER_CONF(t), LEDC_HSTIMER_RST)//timer 0-3 Start?
#define HSTIMER_DIV_NUM(t, d)   SET_PERI_REG_BITS(LEDC_HSTIMER_CONF(t), LEDC_DIV_NUM_HSTIMER, (d), LEDC_DIV_NUM_HSTIMER_S)//timer, 0-3 div_num 18 bit
#define HSTIMER_LIM(t, l)       SET_PERI_REG_BITS(LEDC_HSTIMER_CONF(t), LEDC_HSTIMER_LIM, (l), LEDC_HSTIMER_LIM_S)//timer 0-3, timer_lim 5 bit

#define HSCH_TIMER_SEL(c, t)    SET_PERI_REG_BITS(LEDC_HSCH_CONF0(c), LEDC_TIMER_SEL_HSCH, t, LEDC_TIMER_SEL_HSCH_S)//chan 0-7, timer 0-3 (attach timer to channel)
#define HSCH_IDLE_LVL(c, l)     do{if(l)SET_PERI_REG_MASK(LEDC_HSCH_CONF0(c), LEDC_IDLE_LV_HSCH); else CLEAR_PERI_REG_MASK(LEDC_HSCH_CONF0(c), LEDC_IDLE_LV_HSCH);}while(0)//chan 0-7, level when idle
#define HSCH_SIG_OUT_EN(c)      SET_PERI_REG_MASK(LEDC_HSCH_CONF0(c), LEDC_SIG_OUT_EN_HSCH)//chan 0-7 (enable channel)
#define HSCH_SIG_OUT_DIS(c)     CLEAR_PERI_REG_MASK(LEDC_HSCH_CONF0(c), LEDC_SIG_OUT_EN_HSCH)//chan 0-7 (disable channel)
#define HSCH_HPOINT(c, h)       SET_PERI_REG_BITS(LEDC_HSCH_HPOINT(c), LEDC_HPOINT_HSCH, (h), LEDC_HPOINT_HSCH_S)//hpoint 20 bit
#define HSCH_DUTY(c, d)         SET_PERI_REG_BITS(LEDC_HSCH_DUTY(c), LEDC_DUTY_HSCH, (d), LEDC_DUTY_HSCH_S)//duty 25bit
#define HSCH_DUTY_INC(c, i)     do{if(i)SET_PERI_REG_MASK(LEDC_HSCH_CONF1(c), LEDC_DUTY_INC_HSCH); else CLEAR_PERI_REG_MASK(LEDC_HSCH_CONF1(c), LEDC_DUTY_INC_HSCH0);}while(0)//increase 1 bit
#define HSCH_DUTY_NUM(c, n)     SET_PERI_REG_BITS(LEDC_HSCH_CONF1(c), LEDC_DUTY_NUM_HSCH, (n), LEDC_DUTY_NUM_HSCH_S)//num 10 bit
#define HSCH_DUTY_CYCLE(c, i)   SET_PERI_REG_BITS(LEDC_HSCH_CONF1(c), LEDC_DUTY_CYCLE_HSCH, (i), LEDC_DUTY_CYCLE_HSCH_S)//cycle 10 bit
#define HSCH_DUTY_SCALE(c, s)   SET_PERI_REG_BITS(LEDC_HSCH_CONF1(c), LEDC_DUTY_SCALE_HSCH, (s), LEDC_DUTY_SCALE_HSCH_S)//scale 10 bit
#define HSCH_DUTY_START(c)      SET_PERI_REG_MASK(LEDC_HSCH_CONF1(c), LEDC_DUTY_START_HSCH)//chan 0-7 (channel duty start)
#define HSCH_DUTY_STOP(c)       CLEAR_PERI_REG_MASK(LEDC_HSCH_CONF1(c), LEDC_DUTY_START_HSCH)//chan 0-7 (channel duty stop)

//frequency = 40000000/(div_num*(1 << lim));
void ledc_hs_setup_timer(uint8_t timer, uint32_t div_num, uint8_t lim){
  HSTIMER_STOP(timer);
  HSTIMER_DIV_NUM(timer, div_num);//timer, 0-3 div_num 18 bit
  HSTIMER_LIM(timer, lim);//timer 0-3, timer_lim 5 bit
  HSTIMER_START(timer);
}

void ledc_hs_setup_channel(uint8_t channel, uint8_t idle, uint32_t hpoint, uint32_t duty, bool increase, uint16_t num, uint16_t cycle, uint16_t scale){
  HSCH_IDLE_LVL(channel, idle);//chan 0-7, level when idle
  HSCH_HPOINT(channel, hpoint);//hpoint 20 bit
  HSCH_DUTY(channel, duty);//duty 25bit
  HSCH_DUTY_INC(channel, increase);//increase 1 bit
  HSCH_DUTY_NUM(channel, num);//num 10 bit
  HSCH_DUTY_CYCLE(channel, cycle);//cycle 10 bit
  HSCH_DUTY_SCALE(channel, scale);//scale 10 bit
}

void ledc_hs_attach_timer(uint8_t channel, uint8_t timer){
  HSCH_TIMER_SEL(channel, timer);//chan 0-7, timer 0-3 (attach timer to channel)
}

void ledc_hs_start_channel(uint8_t channel){
  HSCH_DUTY_START(channel);//chan 0-7
  HSCH_SIG_OUT_EN(channel);//chan 0-7
}

void ledc_hs_stop_channel(uint8_t channel){
  HSCH_DUTY_STOP(channel);//chan 0-7
  HSCH_SIG_OUT_DIS(channel);//chan 0-7
}

void ledc_hs_attach_pin(uint8_t pin, uint8_t channel){
  pinMatrixOutAttach(pin, LEDC_HS_SIG_OUT0_IDX + channel);
}

void ledc_hs_detach_pin(uint8_t pin){
  pinMatrixOutDetach(pin);
}
