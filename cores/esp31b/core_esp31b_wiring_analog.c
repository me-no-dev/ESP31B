/* 
  analog.c - analogRead implementation for ESP31B

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
  
  
  18/06/2015 analogRead bugfix by Testato
*/

#include "wiring_private.h"
#include "pins_arduino.h"


extern int __analogRead(uint8_t pin) {
  uint8_t chan = 0xff;
  if(pin > 35) chan = pin - 36;
  else if(pin > 31) chan = pin - 28;
  if(chan < 8) return system_adc1_read(chan, ADC1_ATTEN_12DB);
  return digitalRead(pin) * 4095;
}

extern int analogRead(uint8_t pin) __attribute__ ((weak, alias("__analogRead")));
