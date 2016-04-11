/* 
 core_ESP31B_wiring.c - implementation of Wiring API for ESP31B

 Copyright (c) 2016 Hristo Gochkov. All rights reserved.
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
#include "esp_common.h"

#define MATRIX_DETACH_OUT_SIG 0x80
#define MATRIX_DETACH_IN_LOW_PIN 0x30
#define MATRIX_DETACH_IN_LOW_HIGH 0x38

void pinMatrixOutAttach(uint8_t pin, uint8_t function){
  if(function != MATRIX_DETACH_OUT_SIG)
    pinMode(pin, OUTPUT);
  portENTER_CRITICAL();
  SET_PERI_REG_BITS((GPIO_FUNC_OUT_SEL0 + ((pin / 4) * 4)), 0xff, function, ((pin % 4) * 8));
  portEXIT_CRITICAL();
  if(function == MATRIX_DETACH_OUT_SIG)
    pinMode(pin, INPUT);
}

void pinMatrixOutDetach(uint8_t pin){
  pinMatrixOutAttach(pin, MATRIX_DETACH_OUT_SIG);
}

void pinMatrixInAttach(uint8_t pin, uint8_t function){
  portENTER_CRITICAL();
  SET_PERI_REG_BITS((GPIO_FUNC_IN_SEL0 + ((function / 5) * 4)), 0x3f, pin, ((function % 5) * 6));
  if ((function < 6) || ((function < 16) && (function > 7)) || ((function < 19) && (function > 16)) || ((function < 69) && (function > 62)))
    SET_PERI_REG_MASK(SIG_FUNC_IN_SEL, 1 << function);
  portEXIT_CRITICAL();
}

void pinMatrixInDetach(uint8_t signal, bool high){
  pinMatrixInAttach(high?MATRIX_DETACH_IN_LOW_HIGH:MATRIX_DETACH_IN_LOW_PIN, signal);
}

void intrMatrixAttach(uint32_t source, uint32_t inum){
  uint32 shift = (source % 6) * 5;
  if (shift >= 15) shift++;
  SET_PERI_REG_BITS((PRO_INTR_MAP_REG_A + (source / 6) * 4), 0x1f, inum, shift);
}
