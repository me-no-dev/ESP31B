/*
 HardwareSerial.h - Hardware serial library for Wiring
 Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

 Modified 28 September 2010 by Mark Sproul
 Modified 14 August 2012 by Alarus
 Modified 3 December 2013 by Matthijs Kooijman
 Modified 18 December 2014 by Ivan Grokhotkov (ESP31B platform support)
 Modified 31 March 2015 by Markus Sattler (rewrite the code for UART0 + UART1 support in ESP31B)
 Modified 25 April 2015 by Thomas Flayols (add configuration different from 8N1 in ESP31B)
 */

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"
#include "esp31b_uart.h"

class HardwareSerial: public Stream {
  public:
    HardwareSerial(int uart_nr);

    void setPins(int8_t rx, int8_t tx);

    void begin(unsigned long baud) {
      begin(baud, SERIAL_8N1, SERIAL_FULL);
    }
    void begin(unsigned long baud, uint32_t config) {
      begin(baud, config, SERIAL_FULL);
    }
    void begin(unsigned long, uint32_t, uint8_t);
    void end();
    int available(void) override;
    int peek(void) override;
    int read(void) override;
    int availableForWrite(void);
    void flush(void) override;
    size_t write(uint8_t) override;
    inline size_t write(unsigned long n) {
      return write((uint8_t) n);
    }
    inline size_t write(long n) {
      return write((uint8_t) n);
    }
    inline size_t write(unsigned int n) {
      return write((uint8_t) n);
    }
    inline size_t write(int n) {
      return write((uint8_t) n);
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() const;

    void setDebugOutput(bool);
    bool isTxEnabled(void);
    bool isRxEnabled(void);

  protected:
    int _uart_nr;
    uart_t* _uart;
    int8_t  _rx_pin;
    int8_t  _tx_pin;
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;

#endif
