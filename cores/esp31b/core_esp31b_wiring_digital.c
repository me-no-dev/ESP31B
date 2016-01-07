/* 
  digital.c - wiring digital implementation for ESP31B

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
#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"
#include "esp_common.h"




uint8_t esp31b_gpioToFn[40] = {
    0x44,
    0x88,
    0x40,
    0x84,
    0x48,
    0x6c,
    0x60,
    0x64,
    0x68,
    0x54,
    0x58,
    0x5c,
    0x34,
    0x38,
    0x30,
    0x3c,
    0x4c,
    0x50,
    0x70,
    0x74,
    0x78,
    0x7c,
    0x80,
    0x8c,
    0xFF,
    0x24,
    0x28,
    0x2c,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x1c,
    0x20,
    0x14,
    0x18,
    0x04,
    0x08,
    0x0c,
    0x10
};


extern void __pinMode(uint8_t pin, uint8_t mode) {
  if(pin > 39 || esp31b_gpioToFn[pin] == 0xFF)
    return;
  portENTER_CRITICAL();
  if(mode & INPUT){
    if(pin < 32) GPEC0 = ((uint32_t)1 << pin);
    else GPEC1 = ((uint32_t)1 << (pin - 32));
    GPF(pin) |= ((uint32_t)1 << GPFIE);
  } else if(mode & OUTPUT){
    if(pin < 32) GPES0 = ((uint32_t)1 << pin);
    else GPES1 = ((uint32_t)1 << (pin - 32));
  }

  if(mode & PULLUP)
    GPF(pin) |= ((uint32_t)1 << GPFPU);
  else
    GPF(pin) &= ~((uint32_t)1 << GPFPU);

  if(mode & PULLDOWN)
    GPF(pin) |= ((uint32_t)1 << GPFPD);
  else
    GPF(pin) &= ~((uint32_t)1 << GPFPD);

  GPF(pin) &= ~((uint32_t)7 << GPFF);
  if(mode & (INPUT | OUTPUT)){
    GPF(pin) |= ((uint32_t)2 << GPFF);
  } else if(mode == SPECIAL){
    GPF(pin) |= ((uint32_t)GPFFS_BUS(pin) << GPFF);
  } else {
    GPF(pin) |= ((uint32_t)(mode >> 5) << GPFF);
  }

  if(mode & OPEN_DRAIN)
    GPC(pin) |= (1 << GPCD);
  GPC(pin) &= ~(((uint32_t)0x1F << GPCIE) | ((uint32_t)0x7 << GPCI));
  GPC(pin) |= ((uint32_t)0x4 << GPCIE);
  portEXIT_CRITICAL();
}

extern void __digitalWrite(uint8_t pin, uint8_t val) {
  if(pin > 39 || esp31b_gpioToFn[pin] == 0xFF)
      return;
  portENTER_CRITICAL();
  if(val){
    if(pin < 32) GPOS0 = ((uint32_t)1 << pin);
    else GPOS1 = ((uint32_t)1 << (pin - 32));
  } else {
    if(pin < 32) GPOC0 = ((uint32_t)1 << pin);
    else GPOC1 = ((uint32_t)1 << (pin - 32));
  }
  portEXIT_CRITICAL();
}

extern int __digitalRead(uint8_t pin) {
  if(pin > 39 || esp31b_gpioToFn[pin] == 0xFF)
      return 0;
  if(pin < 32) return (GPI0 >> pin) & 0x1;
  else return (GPI1 >> (pin - 32)) & 0x1;
}

/*
  GPIO INTERRUPTS
*/

#define ETS_GPIO_INTR_DISABLE() xt_ints_off(1 << ETS_GPIO_INUM)
#define ETS_GPIO_INTR_ENABLE() xt_ints_on(1 << ETS_GPIO_INUM)

typedef void (*voidFuncPtr)(void);

typedef struct {
    int8_t pin;
    uint8_t mode;
    voidFuncPtr fn;
} pin_int_handler_t;

static pin_int_handler_t interrupt_handlers[8];
static bool __interrupt_initialized = false;

void __onPinInterrupt(){
  ETS_GPIO_INTR_DISABLE();
  uint32 intr_status;
  int intr_num;
  intr_status = READ_PERI_REG(PCNT_INT_ST);
  while ((intr_num = __builtin_ctz(intr_status)) >= 0) {
    intr_status &= ~BIT(intr_num);
    SET_PERI_REG_MASK(PCNT_INT_CLR, BIT(intr_num));
    CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2 + 1));
    SET_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2));
    CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2 + 1));
    CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2));

    pin_int_handler_t *p = &interrupt_handlers[intr_num];
    if(p->pin >= 0 && p->fn != NULL)
      p->fn();
  }
  ETS_GPIO_INTR_ENABLE();
}

extern void __attachInterrupt(uint8_t gpio_num, voidFuncPtr userFunc, int intr_type) {
  if(!__interrupt_initialized){
    __interrupt_initialized = true;
    ETS_GPIO_INTR_DISABLE();
    intrMatrixAttach(HW_GPIO_INUM, ETS_GPIO_INUM);
    xt_set_interrupt_handler(ETS_GPIO_INUM, &__onPinInterrupt, NULL);
    ETS_GPIO_INTR_ENABLE();
  }

  uint8_t intr_num;
  pin_int_handler_t *p;
  for(intr_num=0;intr_num<8;intr_num++){
    p = &interrupt_handlers[intr_num];
    if(p->pin == -1)
      break;
  }
  if(intr_num>=8)
    return;
  p->pin = gpio_num;
  p->mode = intr_type;
  p->fn = userFunc;

  //gpio_pin_intr_state_set(gpio_num, (GPIO_INT_TYPE)intr_type);
  portENTER_CRITICAL();
  GPC(gpio_num) &= ~(((uint32_t)0x1F << GPCIE) | ((uint32_t)0x7 << GPCI));
  GPC(gpio_num) |= ((uint32_t)0x8 << GPCIE) | ((uint32_t)intr_type << GPCI);
  portEXIT_CRITICAL();

  //gpio_intr_config(gpio_num, intr_num, (GPIO_INT_TYPE)intr_type);
  const uint8 intr_gpio_signals[] = {
      PCNT_IN00_IDX,
      PCNT_IN01_IDX,
      PCNT_IN02_IDX,
      PCNT_IN03_IDX,
      PCNT_IN04_IDX,
      PCNT_IN05_IDX,
      PCNT_IN06_IDX,
      PCNT_IN07_IDX
  };

  CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2 + 1));
  SET_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2));

  pinMatrixInAttach(gpio_num, intr_gpio_signals[intr_num]);

  uint32 cfg0_addr = PCNT_U0_CONF0 + (intr_num * 12);
  uint32 cfg1_addr = PCNT_U0_CONF1 + (intr_num * 12);
  uint32 cfg2_addr = PCNT_U0_CONF2 + (intr_num * 12);

  SET_PERI_REG_BITS(cfg0_addr, PCNT_CH1_LCTRL_MODE_U0, 0, PCNT_CH1_LCTRL_MODE_U0_S);
  SET_PERI_REG_BITS(cfg0_addr, PCNT_CH1_HCTRL_MODE_U0, 0, PCNT_CH1_HCTRL_MODE_U0_S);
  SET_PERI_REG_BITS(cfg0_addr, PCNT_CH1_POS_MODE_U0, 0, PCNT_CH1_POS_MODE_U0_S);
  SET_PERI_REG_BITS(cfg0_addr, PCNT_CH1_NEG_MODE_U0, 0, PCNT_CH1_NEG_MODE_U0_S);
  SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_LCTRL_MODE_U0, 0, PCNT_CH0_LCTRL_MODE_U0_S);
  SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_HCTRL_MODE_U0, 0, PCNT_CH0_HCTRL_MODE_U0_S);

  if (intr_type == FALLING) {
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_POS_MODE_U0, 0, PCNT_CH0_POS_MODE_U0_S);
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_NEG_MODE_U0, 1, PCNT_CH0_NEG_MODE_U0_S);
  } else if (intr_type == RISING) {
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_POS_MODE_U0, 1, PCNT_CH0_POS_MODE_U0_S);
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_NEG_MODE_U0, 0, PCNT_CH0_NEG_MODE_U0_S);
  } else if (intr_type == CHANGE) {
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_POS_MODE_U0, 1, PCNT_CH0_POS_MODE_U0_S);
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_NEG_MODE_U0, 1, PCNT_CH0_NEG_MODE_U0_S);
  } else {
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_POS_MODE_U0, 0, PCNT_CH0_POS_MODE_U0_S);
      SET_PERI_REG_BITS(cfg0_addr, PCNT_CH0_NEG_MODE_U0, 0, PCNT_CH0_NEG_MODE_U0_S);
  }

  SET_PERI_REG_BITS(cfg1_addr, PCNT_CNT_THRES0_U0, 1, PCNT_CNT_THRES0_U0_S);
  SET_PERI_REG_BITS(cfg2_addr, PCNT_CNT_L_LIM_U0, 10, PCNT_CNT_L_LIM_U0_S);
  SET_PERI_REG_BITS(cfg2_addr, PCNT_CNT_H_LIM_U0, 10, PCNT_CNT_H_LIM_U0_S);
  CLEAR_PERI_REG_MASK(cfg0_addr, (PCNT_THR_THRES1_EN_U0 | PCNT_THR_L_LIM_EN_U0 | PCNT_THR_H_LIM_EN_U0 | PCNT_THR_ZERO_EN_U0 | PCNT_FILTER_EN_U0));

  SET_PERI_REG_MASK(cfg0_addr, PCNT_THR_THRES0_EN_U0);
  SET_PERI_REG_MASK(PCNT_INT_ENA, BIT(intr_num));

  CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2 + 1));
  CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2));
}

extern void __detachInterrupt(uint8_t gpio_num) {
  uint8_t intr_num;
  pin_int_handler_t *p;
  for(intr_num=0;intr_num<8;intr_num++){
    p = &interrupt_handlers[intr_num];
    if(p->pin == gpio_num){
      p->pin = -1;
      p->mode = 0;
      p->fn = NULL;
      break;
    }
  }
  if(intr_num>=8)
    return;

  //gpio_pin_intr_state_set(gpio_num, GPIO_PIN_INTR_DISABLE);
  portENTER_CRITICAL();
  GPC(gpio_num) &= ~(((uint32_t)0x1F << GPCIE) | ((uint32_t)0x7 << GPCI));
  GPC(gpio_num) |= ((uint32_t)0x8 << GPCIE);
  portEXIT_CRITICAL();
  CLEAR_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2 + 1));
  SET_PERI_REG_MASK(PCNT_CTRL, BIT(intr_num * 2));
  CLEAR_PERI_REG_MASK((PCNT_U0_CONF0 + (intr_num * 12)), PCNT_THR_THRES0_EN_U0);
  CLEAR_PERI_REG_MASK(PCNT_INT_ENA, BIT(intr_num));
}

void initPins() {
  int i;
  for(i=0;i<8;i++){
    pin_int_handler_t *p = &interrupt_handlers[i];
    p->pin = -1;
    p->mode = 0;
    p->fn = NULL;
  }
  __interrupt_initialized = false;
  timer1_isr_init();
}

extern void pinMode(uint8_t pin, uint8_t mode) __attribute__ ((weak, alias("__pinMode")));
extern void digitalWrite(uint8_t pin, uint8_t val) __attribute__ ((weak, alias("__digitalWrite")));
extern int digitalRead(uint8_t pin) __attribute__ ((weak, alias("__digitalRead")));
extern void attachInterrupt(uint8_t pin, voidFuncPtr handler, int mode) __attribute__ ((weak, alias("__attachInterrupt")));
extern void detachInterrupt(uint8_t pin) __attribute__ ((weak, alias("__detachInterrupt")));

