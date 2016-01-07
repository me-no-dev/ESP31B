/* 
  twi.h - SigmaDelta library for ESP31B

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
#ifndef SIGMA_DELTA_H_
#define SIGMA_DELTA_H_
#include "Arduino.h"

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//8 SigmaDelta Channels 8 bit resolution
void sd_set_prescale(uint8_t channel, uint8_t prescale);
void sd_set_freq(uint8_t channel, uint32_t freq);
void sd_write(uint8_t channel, uint8_t in);
void sd_attach_pin(uint8_t pin, uint8_t channel);
void sd_detach_pin(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif