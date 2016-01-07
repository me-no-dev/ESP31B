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

#define LEDC_LSTIMER_CONF(t)          (DR_REG_LEDC_BASE + 0x0164 + (t * 8))
#define LEDC_LSTIMER_PARA_UP          LEDC_LSTIMER0_PARA_UP
#define LEDC_LSTIMER_PARA_UP_S        LEDC_LSTIMER0_PARA_UP_S
#define LEDC_LSTIMER_PAUSE            LEDC_LSTIMER0_PAUSE
#define LEDC_LSTIMER_RST              LEDC_LSTIMER0_RST
#define LEDC_TICK_ALWAYS_ON_LSTIMER   LEDC_TICK_ALWAYS_ON_LSTIMER0
#define LEDC_DIV_NUM_LSTIMER          LEDC_DIV_NUM_LSTIMER0
#define LEDC_DIV_NUM_LSTIMER_S        LEDC_DIV_NUM_LSTIMER0_S
#define LEDC_LSTIMER_LIM              LEDC_LSTIMER0_LIM
#define LEDC_LSTIMER_LIM_S            LEDC_LSTIMER0_LIM_S

#define LEDC_LSTIMER_VALUE(t)         (DR_REG_LEDC_BASE + 0x0168 + (t * 8))

#define LEDC_LSCH_CONF0(c)      (DR_REG_LEDC_BASE + 0x00a0 + (c * 0x14))
#define LEDC_PARA_UP_LSCH       LEDC_PARA_UP_LSCH0
#define LEDC_PARA_UP_LSCH_S     LEDC_PARA_UP_LSCH0_S
#define LEDC_IDLE_LV_LSCH       LEDC_IDLE_LV_LSCH0
#define LEDC_IDLE_LV_LSCH_S     LEDC_IDLE_LV_LSCH0_S
#define LEDC_SIG_OUT_EN_LSCH    LEDC_SIG_OUT_EN_LSCH0
#define LEDC_SIG_OUT_EN_LSCH_S  LEDC_SIG_OUT_EN_LSCH0_S
#define LEDC_TIMER_SEL_LSCH     LEDC_TIMER_SEL_LSCH0
#define LEDC_TIMER_SEL_LSCH_S   LEDC_TIMER_SEL_LSCH0_S

#define LEDC_LSCH_CONF1(c)      (DR_REG_LEDC_BASE + 0x00ac + (c * 0x14))
#define LEDC_DUTY_START_LSCH    LEDC_DUTY_START_LSCH0
#define LEDC_DUTY_START_LSCH_S  LEDC_DUTY_START_LSCH0_S
#define LEDC_DUTY_INC_LSCH      LEDC_DUTY_INC_LSCH0
#define LEDC_DUTY_INC_LSCH_S    LEDC_DUTY_INC_LSCH0_S
#define LEDC_DUTY_NUM_LSCH      LEDC_DUTY_NUM_LSCH0
#define LEDC_DUTY_NUM_LSCH_S    LEDC_DUTY_NUM_LSCH0_S
#define LEDC_DUTY_CYCLE_LSCH    LEDC_DUTY_CYCLE_LSCH0
#define LEDC_DUTY_CYCLE_LSCH_S  LEDC_DUTY_CYCLE_LSCH0_S
#define LEDC_DUTY_SCALE_LSCH    LEDC_DUTY_SCALE_LSCH0
#define LEDC_DUTY_SCALE_LSCH_S  LEDC_DUTY_SCALE_LSCH0_S

#define LEDC_LSCH_HPOINT(c)     (DR_REG_LEDC_BASE + 0x00a4 + (c * 0x14))
#define LEDC_HPOINT_LSCH        LEDC_HPOINT_LSCH0
#define LEDC_HPOINT_LSCH_S      LEDC_HPOINT_LSCH0_S

#define LEDC_LSCH_DUTY(c)       (DR_REG_LEDC_BASE + 0x00a8 + (c * 0x14))
#define LEDC_LSCH_DUTY_R(c)     (DR_REG_LEDC_BASE + 0x00b0 + (c * 0x14))
#define LEDC_DUTY_LSCH          LEDC_DUTY_LSCH0
#define LEDC_DUTY_LSCH_S        LEDC_DUTY_LSCH0_S

#define LSTIMER_PAUSE(t)        SET_PERI_REG_MASK(LEDC_LSTIMER_CONF(t), LEDC_LSTIMER_PAUSE)//timer 0-3 Pause
#define LSTIMER_RESUME(t)       CLEAR_PERI_REG_MASK(LEDC_LSTIMER_CONF(t), LEDC_LSTIMER_PAUSE)//timer 0-3 Resume
#define LSTIMER_STOP(t)         SET_PERI_REG_MASK(LEDC_LSTIMER_CONF(t), LEDC_LSTIMER_RST)//timer 0-3 Stop
#define LSTIMER_START(t)        CLEAR_PERI_REG_MASK(LEDC_LSTIMER_CONF(t), LEDC_LSTIMER_RST)//timer 0-3 Start?
#define LSTIMER_DIV_NUM(t, d)   SET_PERI_REG_BITS(LEDC_LSTIMER_CONF(t), LEDC_DIV_NUM_LSTIMER, (d), LEDC_DIV_NUM_LSTIMER_S)//timer, 0-3 div_num 18 bit
#define LSTIMER_LIM(t, l)       SET_PERI_REG_BITS(LEDC_LSTIMER_CONF(t), LEDC_LSTIMER_LIM, (l), LEDC_LSTIMER_LIM_S)//timer 0-3, timer_lim 5 bit
#define LSTIMER_PARA_UP(t)      SET_PERI_REG_MASK(LEDC_LSTIMER_CONF(t), LEDC_LSTIMER_PARA_UP)//timer 0-3

#define LSCH_TIMER_SEL(c, t)    SET_PERI_REG_BITS(LEDC_LSCH_CONF0(c), LEDC_TIMER_SEL_LSCH, t, LEDC_TIMER_SEL_LSCH_S)//chan 0-7, timer 0-3 (attach timer to channel)
#define LSCH_IDLE_LVL(c, l)     do{if(l)SET_PERI_REG_MASK(LEDC_LSCH_CONF0(c), LEDC_IDLE_LV_LSCH); else CLEAR_PERI_REG_MASK(LEDC_LSCH_CONF0(c), LEDC_IDLE_LV_LSCH);}while(0)//chan 0-7, level when idle
#define LSCH_SIG_OUT_EN(c)      SET_PERI_REG_MASK(LEDC_LSCH_CONF0(c), LEDC_SIG_OUT_EN_LSCH)//chan 0-7 (enable channel)
#define LSCH_SIG_OUT_DIS(c)     CLEAR_PERI_REG_MASK(LEDC_LSCH_CONF0(c), LEDC_SIG_OUT_EN_LSCH)//chan 0-7 (disable channel)
#define LSCH_HPOINT(c, h)       SET_PERI_REG_BITS(LEDC_LSCH_HPOINT(c), LEDC_HPOINT_LSCH, (h), LEDC_HPOINT_LSCH_S)//hpoint 20 bit
#define LSCH_DUTY(c, d)         SET_PERI_REG_BITS(LEDC_LSCH_DUTY(c), LEDC_DUTY_LSCH, (d), LEDC_DUTY_LSCH_S)//duty 25bit
#define LSCH_DUTY_INC(c, i)     do{if(i)SET_PERI_REG_MASK(LEDC_LSCH_CONF1(c), LEDC_DUTY_INC_LSCH); else CLEAR_PERI_REG_MASK(LEDC_LSCH_CONF1(c), LEDC_DUTY_INC_LSCH0);}while(0)//increase 1 bit
#define LSCH_DUTY_NUM(c, n)     SET_PERI_REG_BITS(LEDC_LSCH_CONF1(c), LEDC_DUTY_NUM_LSCH, (n), LEDC_DUTY_NUM_LSCH_S)//num 10 bit
#define LSCH_DUTY_CYCLE(c, i)   SET_PERI_REG_BITS(LEDC_LSCH_CONF1(c), LEDC_DUTY_CYCLE_LSCH, (i), LEDC_DUTY_CYCLE_LSCH_S)//cycle 10 bit
#define LSCH_DUTY_SCALE(c, s)   SET_PERI_REG_BITS(LEDC_LSCH_CONF1(c), LEDC_DUTY_SCALE_LSCH, (s), LEDC_DUTY_SCALE_LSCH_S)//scale 10 bit
#define LSCH_DUTY_START(c)      SET_PERI_REG_MASK(LEDC_LSCH_CONF1(c), LEDC_DUTY_START_LSCH)//chan 0-7 (channel duty start)
#define LSCH_DUTY_STOP(c)       CLEAR_PERI_REG_MASK(LEDC_LSCH_CONF1(c), LEDC_DUTY_START_LSCH)//chan 0-7 (channel duty stop)
#define LSCH_PARA_UP(c)         SET_PERI_REG_MASK(LEDC_LSCH_CONF0(c), LEDC_PARA_UP_LSCH)
#define LSCH_PARA_DOWN(c)       CLEAR_PERI_REG_MASK(LEDC_LSCH_CONF0(c), LEDC_PARA_UP_LSCH)

//frequency = 40000000/(div_num*(1 << lim));
void ledc_ls_setup_timer(uint8_t timer, uint32_t div_num, uint8_t lim){
  LSTIMER_STOP(timer);
  LSTIMER_DIV_NUM(timer, div_num);//timer, 0-3 div_num 18 bit
  LSTIMER_LIM(timer, lim);//timer 0-3, timer_lim 5 bit
  LSTIMER_PARA_UP(timer);
  LSTIMER_START(timer);
}

void ledc_ls_setup_channel(uint8_t channel, uint8_t idle, uint32_t hpoint, uint32_t duty, bool increase, uint16_t num, uint16_t cycle, uint16_t scale){
  LSCH_IDLE_LVL(channel, idle);//chan 0-7, level when idle
  LSCH_HPOINT(channel, hpoint);//hpoint 20 bit
  LSCH_DUTY(channel, duty);//duty 25bit
  LSCH_DUTY_INC(channel, increase);//increase 1 bit
  LSCH_DUTY_NUM(channel, num);//num 10 bit
  LSCH_DUTY_CYCLE(channel, cycle);//cycle 10 bit
  LSCH_DUTY_SCALE(channel, scale);//scale 10 bit
}

void ledc_ls_attach_timer(uint8_t channel, uint8_t timer){
  LSCH_TIMER_SEL(channel, timer);//chan 0-7, timer 0-3 (attach timer to channel)
}

void ledc_ls_start_channel(uint8_t channel){
  LSCH_DUTY_START(channel);//chan 0-7
  LSCH_SIG_OUT_EN(channel);//chan 0-7
  LSCH_PARA_UP(channel);
}

void ledc_ls_stop_channel(uint8_t channel){
  LSCH_DUTY_STOP(channel);//chan 0-7
  LSCH_SIG_OUT_DIS(channel);//chan 0-7
  LSCH_PARA_DOWN(channel);
}

void ledc_ls_attach_pin(uint8_t pin, uint8_t channel){
  pinMatrixOutAttach(pin, LEDC_LS_SIG_OUT0_IDX + channel);
}

void ledc_ls_detach_pin(uint8_t pin){
  pinMatrixOutDetach(pin);
}
