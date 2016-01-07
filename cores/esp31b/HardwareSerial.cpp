/*
 HardwareSerial.cpp - ESP31B UART support

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

 Modified 31 March 2015 by Markus Sattler (rewrite the code for UART0 + UART1 support in ESP31B)
 Modified 25 April 2015 by Thomas Flayols (add configuration different from 8N1 in ESP31B)
 Modified 3 May 2015 by Hristo Gochkov (change register access methods)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "cbuf.h"
#include "interrupts.h"

#include "HardwareSerial.h"

HardwareSerial Serial(UART0);
HardwareSerial Serial1(UART1);

HardwareSerial::HardwareSerial(int uart_nr) : _uart_nr(uart_nr), _uart(0), _rx_pin(-1), _tx_pin(-1) {
  if(uart_nr == UART0){
    _rx_pin = 3;
    _tx_pin = 1;
  }
}

void HardwareSerial::begin(unsigned long baud, uint32_t config, byte mode) {
  _uart = uart_init(_uart_nr, baud, config, mode, _rx_pin, _tx_pin);
}

void HardwareSerial::end() {
  if(uart_get_debug() == _uart_nr) {
    uart_set_debug(0);
  }
  uart_uninit(_uart);
  _uart = 0;
}

void HardwareSerial::setPins(int8_t rx, int8_t tx){
  _rx_pin = rx;
  _tx_pin = tx;
}

void HardwareSerial::setDebugOutput(bool en) {
  if(_uart == 0)
    return;
  if(en) {
    if(_uart->txEnabled)
      uart_set_debug(_uart);
    else
      uart_set_debug(0);
  } else {
    if(uart_get_debug() == _uart_nr)
      uart_set_debug(0);
  }
}

bool HardwareSerial::isTxEnabled(void) {
  if(_uart == 0)
    return false;
  return _uart->txEnabled;
}

bool HardwareSerial::isRxEnabled(void) {
  if(_uart == 0)
    return false;
  return _uart->rxEnabled;
}

int HardwareSerial::available(void) {
  if (_uart && _uart->rxEnabled)
    return uart_rx_available(_uart);
  return 0;
}

int HardwareSerial::peek(void) {
   return -1;
}

int HardwareSerial::read(void) {
  if(_uart && _uart->rxEnabled)
    return uart_read_char(_uart);
  return -1;
}

int HardwareSerial::availableForWrite(void) {
  if(_uart && _uart->txEnabled) {
    return uart_tx_free(_uart);
  }
  return 0;
}

void HardwareSerial::flush() {
  if(_uart == 0 || !_uart->txEnabled)
    return;
  uart_wait_tx_empty(_uart);
}

size_t HardwareSerial::write(uint8_t c) {
  if(_uart == 0 || !_uart->txEnabled)
    return 0;
  uart_write_char(_uart, c);
  return 1;
}

HardwareSerial::operator bool() const {
  return _uart != 0;
}
