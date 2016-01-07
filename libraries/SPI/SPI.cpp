/* 
 SPI.cpp - SPI library for ESP31B

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

#include "SPI.h"
#include "HardwareSerial.h"

extern "C" {
#include "freertos/task.h"
}

typedef union {
        uint32_t regValue;
        struct {
                unsigned regL :6;
                unsigned regH :6;
                unsigned regN :6;
                unsigned regPre :13;
                unsigned regEQU :1;
        };
} spiClk_t;

SPIClass SPI;

SPIClass::SPIClass(uint8_t spi_bus):_sck(-1),_miso(-1),_mosi(-1),_ss(-1) {
  useHwCs = false;
  _spi_bus = spi_bus;
}

void SPIClass::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss) {
  if(sck == -1){
    if(_spi_bus == HSPI){
      _sck = 14;
      _miso = 12;
      _mosi = 13;
      _ss = 15;
    } else if(_spi_bus == VSPI){
      _sck = 18;
      _miso = 19;
      _mosi = 20;
      _ss = 5;
    }
  } else {
    _sck = sck;
    _miso = miso;
    _mosi = mosi;
    _ss = ss;
  }
  pinMode(_sck, OUTPUT);
  pinMode(_sck, FUNCTION_0);
  pinMatrixOutAttach(_sck, SPI_CLK(_spi_bus));
  pinMode(_mosi, OUTPUT);
  pinMode(_mosi, FUNCTION_0);
  pinMatrixOutAttach(_mosi, SPI_MOSI(_spi_bus));
  pinMode(_miso, INPUT);
  pinMode(_miso, FUNCTION_0);
  pinMatrixInAttach(_miso, SPI_MISO(_spi_bus));

  SPIC(_spi_bus) = 0;
  setFrequency(1000000); ///< 1MHz
  SPIU(_spi_bus) = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
  SPIU1(_spi_bus) = (7 << SPILMOSI) | (7 << SPILMISO);
  SPIC1(_spi_bus) = 0;
}

void SPIClass::end() {
  pinMatrixOutDetach(_sck);
  pinMode(_sck, INPUT);
  pinMatrixOutDetach(_mosi);
  pinMode(_mosi, INPUT);
  pinMatrixInDetach(SPI_MISO(_spi_bus), false);
  pinMode(_miso, INPUT);
  if(useHwCs) {
    pinMatrixOutDetach(_ss);
    pinMode(_ss, INPUT);
  }
}

void SPIClass::setHwCs(bool use) {
  if(use) {
    pinMode(_ss, OUTPUT);
    pinMode(_ss, FUNCTION_0);
    pinMatrixOutAttach(_ss, SPI_SS(_spi_bus));
    SPIU(_spi_bus) |= (SPIUCSSETUP | SPIUCSHOLD);
  } else {
    if(useHwCs) {
      pinMatrixOutDetach(_ss);
      pinMode(_ss, INPUT);
      SPIU(_spi_bus) &= ~(SPIUCSSETUP | SPIUCSHOLD);
    }
  }
  useHwCs = use;
}

void SPIClass::beginTransaction(SPISettings settings) {
  while(SPICMD(_spi_bus) & SPIBUSY) ;
  setFrequency(settings._clock);
  setBitOrder(settings._bitOrder);
  setDataMode(settings._dataMode);
}

void SPIClass::endTransaction() {}

void SPIClass::setDataMode(uint8_t dataMode) {

    /**
     SPI_MODE0 0x00 - CPOL: 0  CPHA: 0
     SPI_MODE1 0x01 - CPOL: 0  CPHA: 1
     SPI_MODE2 0x10 - CPOL: 1  CPHA: 0
     SPI_MODE3 0x11 - CPOL: 1  CPHA: 1
     */

    bool CPOL = (dataMode & 0x10); ///< CPOL (Clock Polarity)
    bool CPHA = (dataMode & 0x01); ///< CPHA (Clock Phase)

    if(CPHA) {
        SPIU(_spi_bus) |= (SPIUSME);
    } else {
        SPIU(_spi_bus) &= ~(SPIUSME);
    }

    if(CPOL) {
        //todo How set CPOL???
    }

}

void SPIClass::setBitOrder(uint8_t bitOrder) {
    if(bitOrder == MSBFIRST) {
        SPIC(_spi_bus) &= ~(SPICWBO | SPICRBO);
    } else {
        SPIC(_spi_bus) |= (SPICWBO | SPICRBO);
    }
}

/**
 * calculate the Frequency based on the register value
 * @param reg
 * @return
 */
static uint32_t ClkRegToFreq(spiClk_t * reg) {
    return (ESP31B_CLOCK / ((reg->regPre + 1) * (reg->regN + 1)));
}

void SPIClass::setFrequency(uint32_t freq) {
    static uint32_t lastSetFrequency = 0;
    static uint32_t lastSetRegister = 0;

    if(freq >= ESP31B_CLOCK) {
        setClockDivider(0x80000000);
        return;
    }

    if(lastSetFrequency == freq && lastSetRegister == SPICLK(_spi_bus)) {
        // do nothing (speed optimization)
        return;
    }

    const spiClk_t minFreqReg = { 0x7FFFF000 };
    uint32_t minFreq = ClkRegToFreq((spiClk_t*) &minFreqReg);
    if(freq < minFreq) {
        // use minimum possible clock
        setClockDivider(minFreqReg.regValue);
        lastSetRegister = SPICLK(_spi_bus);
        lastSetFrequency = freq;
        return;
    }

    uint8_t calN = 1;

    spiClk_t bestReg = { 0 };
    int32_t bestFreq = 0;

    // find the best match
    while(calN <= 0x3F) { // 0x3F max for N

        spiClk_t reg = { 0 };
        int32_t calFreq;
        int32_t calPre;
        int8_t calPreVari = -2;

        reg.regN = calN;

        while(calPreVari++ <= 1) { // test different variants for Pre (we calculate in int so we miss the decimals, testing is the easyest and fastest way)
            calPre = (((ESP31B_CLOCK / (reg.regN + 1)) / freq) - 1) + calPreVari;
            if(calPre > 0x1FFF) {
                reg.regPre = 0x1FFF; // 8191
            } else if(calPre <= 0) {
                reg.regPre = 0;
            } else {
                reg.regPre = calPre;
            }

            reg.regL = ((reg.regN + 1) / 2);
            // reg.regH = (reg.regN - reg.regL);

            // test calculation
            calFreq = ClkRegToFreq(&reg);
            //os_printf("-----[0x%08X][%d]\t EQU: %d\t Pre: %d\t N: %d\t H: %d\t L: %d = %d\n", reg.regValue, freq, reg.regEQU, reg.regPre, reg.regN, reg.regH, reg.regL, calFreq);

            if(calFreq == (int32_t) freq) {
                // accurate match use it!
                memcpy(&bestReg, &reg, sizeof(bestReg));
                break;
            } else if(calFreq < (int32_t) freq) {
                // never go over the requested frequency
                if(abs(freq - calFreq) < abs(freq - bestFreq)) {
                    bestFreq = calFreq;
                    memcpy(&bestReg, &reg, sizeof(bestReg));
                }
            }
        }
        if(calFreq == (int32_t) freq) {
            // accurate match use it!
            break;
        }
        calN++;
    }

    // os_printf("[0x%08X][%d]\t EQU: %d\t Pre: %d\t N: %d\t H: %d\t L: %d\t - Real Frequency: %d\n", bestReg.regValue, freq, bestReg.regEQU, bestReg.regPre, bestReg.regN, bestReg.regH, bestReg.regL, ClkRegToFreq(&bestReg));

    setClockDivider(bestReg.regValue);
    lastSetRegister = SPICLK(_spi_bus);
    lastSetFrequency = freq;

}

void SPIClass::setClockDivider(uint32_t clockDiv) {
  SPICLK(_spi_bus) = clockDiv;
}

inline void SPIClass::setDataBits(uint16_t bits) {
    const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    bits--;
    SPIU1(_spi_bus) = ((SPIU1(_spi_bus) & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}

uint8_t SPIClass::transfer(uint8_t data) {
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    // reset to 8Bit mode
    setDataBits(8);
    SPIW0(_spi_bus) = data;
    SPICMD(_spi_bus) |= SPIBUSY;
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    return (uint8_t) (SPIW0(_spi_bus) & 0xff);
}

uint16_t SPIClass::transfer16(uint16_t data) {
    union {
            uint16_t val;
            struct {
                    uint8_t lsb;
                    uint8_t msb;
            };
    } in, out;
    in.val = data;

    if((SPIC(_spi_bus) & (SPICWBO | SPICRBO))) {
        //MSBFIRST
        out.msb = transfer(in.msb);
        out.lsb = transfer(in.lsb);
    } else {
        //LSBFIRST
        out.lsb = transfer(in.lsb);
        out.msb = transfer(in.msb);
    }
    return out.val;
}

void SPIClass::write(uint8_t data) {
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    // reset to 8Bit mode
    setDataBits(8);
    SPIW0(_spi_bus) = data;
    SPICMD(_spi_bus) |= SPIBUSY;
    while(SPICMD(_spi_bus) & SPIBUSY) ;
}

void SPIClass::write16(uint16_t data) {
    write16(data, !(SPIC(_spi_bus) & (SPICWBO | SPICRBO)));
}

void SPIClass::write16(uint16_t data, bool msb) {
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    // Set to 16Bits transfer
    setDataBits(16);
    if(msb) {
        // MSBFIRST Byte first
        SPIW0(_spi_bus) = (data >> 8) | (data << 8);
        SPICMD(_spi_bus) |= SPIBUSY;
    } else {
        // LSBFIRST Byte first
        SPIW0(_spi_bus) = data;
        SPICMD(_spi_bus) |= SPIBUSY;
    }
    while(SPICMD(_spi_bus) & SPIBUSY) ;
}

void SPIClass::write32(uint32_t data) {
    write32(data, !(SPIC(_spi_bus) & (SPICWBO | SPICRBO)));
}

void SPIClass::write32(uint32_t data, bool msb) {
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    // Set to 32Bits transfer
    setDataBits(32);
    if(msb) {
        union {
                uint32_t l;
                uint8_t b[4];
        } data_;
        data_.l = data;
        // MSBFIRST Byte first
        SPIW0(_spi_bus) = (data_.b[3] | (data_.b[2] << 8) | (data_.b[1] << 16) | (data_.b[0] << 24));
        SPICMD(_spi_bus) |= SPIBUSY;
    } else {
        // LSBFIRST Byte first
        SPIW0(_spi_bus) = data;
        SPICMD(_spi_bus) |= SPIBUSY;
    }
    while(SPICMD(_spi_bus) & SPIBUSY) ;
}

/**
 * Note:
 *  data need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param data uint8_t *
 * @param size uint32_t
 */
void SPIClass::writeBytes(uint8_t * data, uint32_t size) {
    while(size) {
        if(size > 64) {
            writeBytes_(data, 64);
            size -= 64;
            data += 64;
        } else {
            writeBytes_(data, size);
            size = 0;
        }
    }
}

void SPIClass::writeBytes_(uint8_t * data, uint8_t size) {
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    // Set Bits to transfer
    setDataBits(size * 8);

    volatile uint32_t * fifoPtr = &SPIW0(_spi_bus);
    uint32_t * dataPtr = (uint32_t*) data;
    uint8_t dataSize = ((size + 3) / 4);

    while(dataSize--) {
        *fifoPtr = *dataPtr;
        dataPtr++;
        fifoPtr++;
    }

    SPICMD(_spi_bus) |= SPIBUSY;
    while(SPICMD(_spi_bus) & SPIBUSY) ;
}


/**
 * Note:
 *  data need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
void SPIClass::writePattern(uint8_t * data, uint8_t size, uint32_t repeat) {
    if(size > 64) return; //max Hardware FIFO

    uint32_t byte = (size * repeat);
    uint8_t r = (64 / size);

    while(byte) {
        if(byte > 64) {
            writePattern_(data, size, r);
            byte -= 64;
        } else {
            writePattern_(data, size, (byte / size));
            byte = 0;
        }
    }
}

void SPIClass::writePattern_(uint8_t * data, uint8_t size, uint8_t repeat) {
    uint8_t bytes = (size * repeat);
    uint8_t buffer[64];
    uint8_t * bufferPtr = &buffer[0];
    uint8_t * dataPtr;
    uint8_t dataSize = bytes;
    for(uint8_t i = 0; i < repeat; i++) {
        dataSize = size;
        dataPtr = data;
        while(dataSize--) {
            *bufferPtr = *dataPtr;
            dataPtr++;
            bufferPtr++;
        }
    }

    writeBytes(&buffer[0], bytes);
}

/**
 * Note:
 *  in and out need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param out uint8_t *
 * @param in  uint8_t *
 * @param size uint32_t
 */
void SPIClass::transferBytes(uint8_t * out, uint8_t * in, uint32_t size) {
    while(size) {
        if(size > 64) {
            transferBytes_(out, in, 64);
            size -= 64;
            if(out) out += 64;
            if(in) in += 64;
        } else {
            transferBytes_(out, in, size);
            size = 0;
        }
    }
}

void SPIClass::transferBytes_(uint8_t * out, uint8_t * in, uint8_t size) {
    while(SPICMD(_spi_bus) & SPIBUSY) ;
    // Set in/out Bits to transfer

    setDataBits(size * 8);

    volatile uint32_t * fifoPtr = &SPIW0(_spi_bus);
    uint8_t dataSize = ((size + 3) / 4);

    if(out) {
        uint32_t * dataPtr = (uint32_t*) out;
        while(dataSize--) {
            *fifoPtr = *dataPtr;
            dataPtr++;
            fifoPtr++;
        }
    } else {
        // no out data only read fill with dummy data!
        while(dataSize--) {
            *fifoPtr = 0xFFFFFFFF;
            fifoPtr++;
        }
    }

    SPICMD(_spi_bus) |= SPIBUSY;
    while(SPICMD(_spi_bus) & SPIBUSY) ;

    if(in) {
        volatile uint8_t * fifoPtr8 = (volatile uint8_t *) &SPIW0(_spi_bus);
        dataSize = size;
        while(dataSize--) {
            *in = *fifoPtr8;
            in++;
            fifoPtr8++;
        }
    }
}

/*

uint32_t lcdData[16]; //can contain (16*32/9=)56 9-bit data words.
uint8_t lcdDataPos=0;

void spiWrite(uint8_t data) {
  int bytePos=(lcdDataPos/4);
  if ((lcdDataPos & 3) == 0) lcdData[bytePos] |= data << 24;
  if ((lcdDataPos & 3) == 1) lcdData[bytePos] |= data << 16;
  if ((lcdDataPos & 3) == 2) lcdData[bytePos] |= data << 8;
  if ((lcdDataPos & 3) == 3) lcdData[bytePos] |= data << 0;
  lcdDataPos++;
}

void spiSend(){
  int x=0;
  for (x=0; x<16; x++) {
    WRITE_PERI_REG(SPI_W0(VSPI)+(x*4), lcdData[x]);
    lcdData[x]=0;
  }
  WRITE_PERI_REG(SPI_USER1(VSPI), (((lcdDataPos*8)-1)<<SPI_USR_MOSI_BITLEN_S));
  WRITE_PERI_REG(SPI_CMD(VSPI), SPI_USR);
  lcdDataPos=0;
}

void spiInit(){
  //SPI clk = 40MHz
  WRITE_PERI_REG(SPI_CLOCK(VSPI), (0<<SPI_CLKDIV_PRE_S) | (1<<SPI_CLKCNT_N_S)|(1<<SPI_CLKCNT_L_S)|(0<<SPI_CLKCNT_H_S));
  WRITE_PERI_REG(SPI_CTRL(VSPI), 0);
  WRITE_PERI_REG(SPI_USER(VSPI), SPI_CS_SETUP|SPI_CS_HOLD|SPI_USR_MOSI|SPI_WR_BYTE_ORDER);
  WRITE_PERI_REG(SPI_USER1(VSPI), (9<<SPI_USR_MOSI_BITLEN_S));
  //setup pins for SCK, MOSI & SS
}

 */
