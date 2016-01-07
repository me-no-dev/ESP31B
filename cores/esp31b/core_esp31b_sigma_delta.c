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

//SigmeDelta
#define SIGMADELTA(c)             (SIGMADELTA0 + ((c) * 4))
#define SIGMADELTA_SD_PRESCALE    SIGMADELTA_SD0_PRESCALE
#define SIGMADELTA_SD_PRESCALE_S  SIGMADELTA_SD0_PRESCALE_S
#define SIGMADELTA_SD_IN          SIGMADELTA_SD0_IN
#define SIGMADELTA_SD_IN_S        SIGMADELTA_SD0_IN_S

#define SIGMADELTA_PRESCALE(c, p) SET_PERI_REG_BITS(SIGMADELTA(c), SIGMADELTA_SD_PRESCALE, (p), SIGMADELTA_SD_PRESCALE_S)//chan 0-7 prescale 8 bit
#define SIGMADELTA_IN(c, i)       SET_PERI_REG_BITS(SIGMADELTA(c), SIGMADELTA_SD_IN, (i), SIGMADELTA_SD_IN_S)//chan 0-7 in 8 bit
#define SIGMADELTA_CLK_EN()       SET_PERI_REG_MASK(SIGMADELTA_CG, SIGMADELTA_GPIO_SD_CLK_EN)
#define SIGMADELTA_CLK_DIS()      CLEAR_PERI_REG_MASK(SIGMADELTA_CG, SIGMADELTA_GPIO_SD_CLK_EN)

void sd_detach_pin(uint8_t pin){
  pinMatrixOutDetach(pin);
}

void sd_attach_pin(uint8_t pin, uint8_t channel){//channel 0-7
  pinMatrixOutAttach(pin, GPIO_SD0_OUT_IDX + channel);
}

void sd_set_freq(uint8_t channel, uint32_t freq){
  uint32_t prescale = (10000000/(freq*32)) - 1;
  if(prescale > 0xFF)
    prescale = 0xFF;
  sd_set_prescale(channel, prescale);
  freq = 10000000/((prescale + 1) * 32);
  os_printf("freq: %u\n", freq);
}

void sd_set_prescale(uint8_t channel, uint8_t prescale){
  SIGMADELTA_PRESCALE(channel, prescale);//chan 0-7 prescale 8 bit
  SIGMADELTA_CLK_DIS();
  SIGMADELTA_CLK_EN();
}

void sd_write(uint8_t channel, uint8_t in){
  in += 128;
  SIGMADELTA_IN(channel, in);//chan 0-7 in 8 bit
}

