/* 
  twi.h - LedC Driver library for ESP31B

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
#ifndef ESP31B_LEDC_H_
#define ESP31B_LEDC_H_
#include "Arduino.h"

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//8 LowSpeed LED PWM Channes with 4 Timers
#define ledc_ls_timer_resolution(timer, resolution_bits) ledc_ls_setup_timer(timer, 256, resolution_bits - 4)
#define ledc_ls_channel_write(channel, duty) ledc_ls_setup_channel(channel,0,0,duty,1,1,1,0)
void ledc_ls_setup_timer(uint8_t timer, uint32_t div_num, uint8_t lim);
void ledc_ls_setup_channel(uint8_t channel, uint8_t idle, uint32_t hpoint, uint32_t duty, bool increase, uint16_t num, uint16_t cycle, uint16_t scale);
void ledc_ls_attach_timer(uint8_t channel, uint8_t timer);
void ledc_ls_start_channel(uint8_t channel);
void ledc_ls_stop_channel(uint8_t channel);
void ledc_ls_attach_pin(uint8_t pin, uint8_t channel);
void ledc_ls_detach_pin(uint8_t pin);
#define ledc_ls_timer_resolution(timer, resolution_bits)  ledc_ls_setup_timer(timer, 256, resolution_bits - 4)
#define ledc_ls_channel_write(channel, duty)              ledc_ls_setup_channel(channel,0,0,duty,1,1,1,0)

//8 HighSpeed LED PWM Channes with 4 Timers
void ledc_hs_setup_timer(uint8_t timer, uint32_t div_num, uint8_t lim);
void ledc_hs_setup_channel(uint8_t channel, uint8_t idle, uint32_t hpoint, uint32_t duty, bool increase, uint16_t num, uint16_t cycle, uint16_t scale);
void ledc_hs_attach_timer(uint8_t channel, uint8_t timer);
void ledc_hs_start_channel(uint8_t channel);
void ledc_hs_stop_channel(uint8_t channel);
void ledc_hs_attach_pin(uint8_t pin, uint8_t channel);
void ledc_hs_detach_pin(uint8_t pin);
#define ledc_hs_timer_resolution(timer, resolution_bits)  ledc_hs_setup_timer(timer, 256, resolution_bits - 4)
#define ledc_hs_channel_write(channel, duty)              ledc_hs_setup_channel(channel,0,0,duty,1,1,1,0)

#ifdef __cplusplus
}
#endif

#endif
