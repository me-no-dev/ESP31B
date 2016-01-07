/* 
  twi.h - UART library for ESP31B

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
#ifndef _ESP31B_UART_h
#define _ESP31B_UART_h
#include "Arduino.h"

#define SERIAL_5N1 0x8000010
#define SERIAL_6N1 0x8000014
#define SERIAL_7N1 0x8000018
#define SERIAL_8N1 0x800001c
#define SERIAL_5N2 0x8000030
#define SERIAL_6N2 0x8000034
#define SERIAL_7N2 0x8000038
#define SERIAL_8N2 0x800003c
#define SERIAL_5E1 0x8000012
#define SERIAL_6E1 0x8000016
#define SERIAL_7E1 0x800001a
#define SERIAL_8E1 0x800001e
#define SERIAL_5E2 0x8000032
#define SERIAL_6E2 0x8000036
#define SERIAL_7E2 0x800003a
#define SERIAL_8E2 0x800003e
#define SERIAL_5O1 0x8000013
#define SERIAL_6O1 0x8000017
#define SERIAL_7O1 0x800001b
#define SERIAL_8O1 0x800001f
#define SERIAL_5O2 0x8000033
#define SERIAL_6O2 0x8000037
#define SERIAL_7O2 0x800003b
#define SERIAL_8O2 0x800003f

#define SERIAL_FULL     0
#define SERIAL_RX_ONLY  1
#define SERIAL_TX_ONLY  2

typedef struct {
  int uart_nr;
  int baud_rate;
  bool rxEnabled;
  bool txEnabled;
  int8_t rxPin;
  int8_t txPin;
} uart_t;

#ifdef __cplusplus
extern "C" {
#endif

static const int UART0 = 0;
static const int UART1 = 1;
static const int UART_NO = -1;

void uart_write_char(uart_t* uart, char c);
uint8_t uart_read_char(uart_t* uart);
uint8_t uart_rx_available(uart_t* uart);
uint8_t uart_tx_free(uart_t* uart);
void uart_wait_tx_empty(uart_t* uart);
void uart_flush(uart_t* uart);
void uart_set_baudrate(uart_t* uart, int baud_rate);
int uart_get_baudrate(uart_t* uart);
uart_t* uart_init(int uart_nr, int baudrate, uint32_t config, byte mode, int8_t rxPin, int8_t txPin);
void uart_uninit(uart_t* uart);
void uart_set_debug(uart_t* uart);
int uart_get_debug();

#ifdef __cplusplus
}
#endif

#endif
