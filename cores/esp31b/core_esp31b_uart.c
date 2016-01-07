/*
  timer.c - Timer1 library for ESP31B

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

#include "c_types.h"
#include "esp_common.h"
#include "freertos/xtensa_api.h"
#include "esp31b_uart.h"

#define UART_TX_FIFO_SIZE 0x80
#define ETS_UART_INTR_ENABLE()  xt_ints_on(1 << ETS_UART_INUM)
#define ETS_UART_INTR_DISABLE() xt_ints_off(1 << ETS_UART_INUM)
#define ETS_UART_INTR_ATTACH(h, v) xt_set_interrupt_handler(ETS_UART_INUM, (void (*)(void*))h, v);

void uart_write_char(uart_t* uart, char c) {
  while(((USS(uart->uart_nr) >> USTXC) & 0xff) > 0x7F) delayMicroseconds(1);
  USF(uart->uart_nr) = c;
}

uint8_t uart_read_char(uart_t* uart){
  return USF(uart->uart_nr) & 0xff;
}

uint8_t uart_rx_available(uart_t* uart){
  return (USS(uart->uart_nr) >> USRXC) & 0xff;
}

uint8_t uart_tx_free(uart_t* uart){
  return UART_TX_FIFO_SIZE - ((USS(uart->uart_nr) >> USTXC) & 0xff);
}

void uart_wait_tx_empty(uart_t* uart){
  while(((USS(uart->uart_nr) >> USTXC) & 0xff) > 0) delayMicroseconds(1);
}

void uart_flush(uart_t* uart) {
  uint32_t tmp = 0x00000000;

  if(uart == 0)
    return;

  if(uart->rxEnabled)
    tmp |= (1 << UCRXRST);

  if(uart->txEnabled)
    tmp |= (1 << UCTXRST);

  USC0(uart->uart_nr) |= (tmp);
  USC0(uart->uart_nr) &= ~(tmp);
}

void uart_set_baudrate(uart_t* uart, int baud_rate) {
  if(uart == 0)
    return;
  uart->baud_rate = baud_rate;
  USD(uart->uart_nr) = 80000000 / uart->baud_rate;
}

int uart_get_baudrate(uart_t* uart) {
  if(uart == 0)
    return 0;
  return uart->baud_rate;
}

uart_t* uart_init(int uart_nr, int baudrate, uint32_t config, byte mode, int8_t rxPin, int8_t txPin) {
  if(uart_nr != UART0 && uart_nr != UART1)
    return 0;

  if(rxPin == -1 && txPin == -1)
    return 0;

  uart_t* uart = (uart_t*) malloc(sizeof(uart_t));
  if(uart == 0)
    return 0;

  uart->rxPin = rxPin;
  uart->txPin = txPin;
  uart->uart_nr = uart_nr;
  uart->rxEnabled = (uart->rxPin != -1 && mode != SERIAL_TX_ONLY);
  uart->txEnabled = (uart->txPin != -1 && mode != SERIAL_RX_ONLY);

  if(uart->rxEnabled){
    if(uart->uart_nr == UART0 && uart->rxPin == 3){
      pinMode(uart->rxPin, SPECIAL);
    } else {
      pinMode(uart->rxPin, INPUT);
      pinMode(uart->rxPin, FUNCTION_0);
      pinMatrixInAttach(uart->rxPin, UART_RXD_IDX(uart->uart_nr));
    }
  }
  if(uart->txEnabled){
    if(uart->uart_nr == UART0 || uart->txPin == 1){
      pinMode(uart->txPin, SPECIAL);
    } else {
      pinMode(uart->txPin, OUTPUT);
      pinMode(uart->txPin, FUNCTION_0);
      pinMatrixOutAttach(uart->txPin, UART_TXD_IDX(uart->uart_nr));
    }
  }

  uart_flush(uart);
  uart_set_baudrate(uart, baudrate);
  USC0(uart->uart_nr) = config;
  USC1(uart->uart_nr) = 0x00000000;
  return uart;
}

void uart_uninit(uart_t* uart) {
    if(uart == 0)
        return;

    if(uart->rxEnabled){
      pinMode(uart->rxPin, INPUT);
      if(uart->uart_nr || uart->rxPin != 3)
        pinMatrixInDetach(UART_RXD_IDX(uart->uart_nr), false);
    }
    if(uart->txEnabled){
      pinMode(uart->txPin, INPUT);
      if(uart->uart_nr || uart->txPin != 1)
        pinMatrixInDetach(UART_TXD_IDX(uart->uart_nr), false);
    }

    free(uart);
}

void uart_ignore_char(char c) {}

void uart0_write_char(char c) {
  while(((USS(0) >> USTXC) & 0xff) > 0x7F) delayMicroseconds(1);
  USF(0) = c;
}

void uart1_write_char(char c) {
  while(((USS(1) >> USTXC) & 0xff) > 0x7F) delayMicroseconds(1);
  USF(1) = c;
}

static int s_uart_debug_nr = 0;

void uart_set_debug(uart_t* uart) {
  s_uart_debug_nr = uart->uart_nr;
  switch(s_uart_debug_nr) {
    case 0:
      ets_install_putc1((void (*)(char)) &uart0_write_char);
      break;
    case 1:
      ets_install_putc1((void (*)(char)) &uart1_write_char);
      break;
    case -1:
    default:
      ets_install_putc1((void (*)(char)) &uart_ignore_char);
      break;
  }
}

int uart_get_debug() {
  return s_uart_debug_nr;
}

