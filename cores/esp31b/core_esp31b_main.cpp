/*
 main.cpp - platform initialization and context switching
 emulation

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

//This may be used to change user task stack size:
//#define CONT_STACKSIZE 4096
#include <Arduino.h>
extern "C" {
#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}

extern "C" void ets_update_cpu_frequency(int freqmhz);
void initVariant() __attribute__((weak));
void initVariant() {}

extern void loop();
extern void setup();

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

static void do_global_ctors(void) {
    void (**p)(void);
    for(p = &__init_array_start; p != &__init_array_end; ++p)
        (*p)();
}

extern "C" void __loop_task(void *pvParameters){
  bool setup_done = false;
  for(;;){
    if(!setup_done) {
        setup();
        setup_done = true;
    }
    loop();
  }
}

extern "C" void user_init(void) {
  do_global_ctors();
#if defined(F_CPU) && (F_CPU == 160000000L)
  ets_update_cpu_frequency(160);
#endif
  init();
  initVariant();
  xTaskCreate(__loop_task, "loop_task", 4096, NULL, tskIDLE_PRIORITY, NULL);
}
