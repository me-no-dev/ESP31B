/* 
 core_ESP31B_wiring.c - implementation of Wiring API for ESP31B

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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

void delay(unsigned long ms) {
  vTaskDelay(ms  / portTICK_PERIOD_MS);
}

unsigned long millis() {
  return system_get_time() / 1000;
}

unsigned long micros() {
  return system_get_time();
}

void delayMicroseconds(unsigned int us) {
  os_delay_us(us);
}

void init() {
  initPins();
}
