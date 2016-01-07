/*
 Esp.cpp - ESP31B-specific APIs
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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

#include "Arduino.h"
#include "flash_utils.h"
#include <memory>
#include "interrupts.h"

extern "C" {
#include "esp_common.h"

extern struct rst_info resetInfo;
}


//#define DEBUG_SERIAL Serial


/**
 * User-defined Literals
 *  usage:
 *
 *   uint32_t = test = 10_MHz; // --> 10000000
 */

unsigned long long operator"" _kHz(unsigned long long x) {
    return x * 1000;
}

unsigned long long operator"" _MHz(unsigned long long x) {
    return x * 1000 * 1000;
}

unsigned long long operator"" _GHz(unsigned long long x) {
    return x * 1000 * 1000 * 1000;
}

unsigned long long operator"" _kBit(unsigned long long x) {
    return x * 1024;
}

unsigned long long operator"" _MBit(unsigned long long x) {
    return x * 1024 * 1024;
}

unsigned long long operator"" _GBit(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}

unsigned long long operator"" _kB(unsigned long long x) {
    return x * 1024;
}

unsigned long long operator"" _MB(unsigned long long x) {
    return x * 1024 * 1024;
}

unsigned long long operator"" _GB(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}


EspClass ESP;

void EspClass::restart(void)
{
    system_restart();
}

uint16_t EspClass::getVcc(void)
{
    InterruptLock lock;
    return system_get_vdd33();
}

uint32_t EspClass::getFreeHeap(void)
{
    return system_get_free_heap_size();
}

const char * EspClass::getSdkVersion(void)
{
    return system_get_sdk_version();
}

uint8_t EspClass::getCpuFreqMHz(void)
{
    return system_get_cpu_freq();
}

uint32_t EspClass::getFlashChipSize(void)
{
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        return magicFlashChipSize((bytes[3] & 0xf0) >> 4);
    }
    return 0;
}

uint32_t EspClass::getFlashChipSpeed(void)
{
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        return magicFlashChipSpeed(bytes[3] & 0x0F);
    }
    return 0;
}

FlashMode_t EspClass::getFlashChipMode(void)
{
    FlashMode_t mode = FM_UNKNOWN;
    uint32_t data;
    uint8_t * bytes = (uint8_t *) &data;
    // read first 4 byte (magic byte + flash config)
    if(spi_flash_read(0x0000, &data, 4) == SPI_FLASH_RESULT_OK) {
        mode = magicFlashChipMode(bytes[2]);
    }
    return mode;
}

uint32_t EspClass::magicFlashChipSize(uint8_t byte) {
    switch(byte & 0x0F) {
        case 0x0: // 4 Mbit (512KB)
            return (512_kB);
        case 0x1: // 2 MBit (256KB)
            return (256_kB);
        case 0x2: // 8 MBit (1MB)
            return (1_MB);
        case 0x3: // 16 MBit (2MB)
            return (2_MB);
        case 0x4: // 32 MBit (4MB)
            return (4_MB);
        case 0x5: // 64 MBit (8MB)
            return (8_MB);
        case 0x6: // 128 MBit (16MB)
            return (16_MB);
        case 0x7: // 256 MBit (32MB)
            return (32_MB);
        default: // fail?
            return 0;
    }
}

uint32_t EspClass::magicFlashChipSpeed(uint8_t byte) {
    switch(byte & 0x0F) {
        case 0x0: // 40 MHz
            return (40_MHz);
        case 0x1: // 26 MHz
            return (26_MHz);
        case 0x2: // 20 MHz
            return (20_MHz);
        case 0xf: // 80 MHz
            return (80_MHz);
        default: // fail?
            return 0;
    }
}

FlashMode_t EspClass::magicFlashChipMode(uint8_t byte) {
    FlashMode_t mode = (FlashMode_t) byte;
    if(mode > FM_DOUT) {
        mode = FM_UNKNOWN;
    }
    return mode;
}

bool EspClass::eraseConfig(void) {
    bool ret = true;
    size_t cfgAddr = (ESP.getFlashChipSize() - 0x4000);
    size_t cfgSize = (8*1024);

    noInterrupts();
    while(cfgSize) {

        if(spi_flash_erase_sector((cfgAddr / SPI_FLASH_SEC_SIZE)) != SPI_FLASH_RESULT_OK) {
            ret = false;
        }

        cfgSize -= SPI_FLASH_SEC_SIZE;
        cfgAddr += SPI_FLASH_SEC_SIZE;
    }
    interrupts();

    return ret;
}

bool EspClass::flashEraseSector(uint32_t sector) {
    int rc = spi_flash_erase_sector(sector);
    return rc == 0;
}

bool EspClass::flashWrite(uint32_t offset, uint32_t *data, size_t size) {
    int rc = spi_flash_write(offset, (uint32_t*) data, size);
    return rc == 0;
}

bool EspClass::flashRead(uint32_t offset, uint32_t *data, size_t size) {
    int rc = spi_flash_read(offset, (uint32_t*) data, size);
    return rc == 0;
}
