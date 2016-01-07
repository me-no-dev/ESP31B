/* 
  twi.h - Pin Martix library for ESP31B

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
#ifndef PIN_MATRIX_H_
#define PIN_MATRIX_H_
#include "Arduino.h"

#include "c_types.h"

/*
 * PIN MATRIX FUNCTIONS
 * */
//SPI
#define SPICLK_IDX      0
#define SPIQ_IDX        1
#define SPID_IDX        2
#define SPIHD_IDX       3
#define SPIWP_IDX       4
#define SPICS0_IDX      5
#define SPICS1_IDX      6
#define SPICS2_IDX      7

//HSPI
#define HSPICLK_IDX     8
#define HSPIQ_IDX       9
#define HSPID_IDX       10
#define HSPICS0_IDX     11
#define HSPIHD_IDX      12
#define HSPIWP_IDX      13
#define HSPICS1_IDX     61
#define HSPICS2_IDX     62

//VSPI
#define VSPICLK_IDX     63
#define VSPIQ_IDX       64
#define VSPID_IDX       65
#define VSPIHD_IDX      66
#define VSPIWP_IDX      67
#define VSPICS0_IDX     68
#define VSPICS1_IDX     69
#define VSPICS2_IDX     70

//UART0
#define U0RXD_IDX       14 //IN
#define U0TXD_IDX       U0RXD_IDX //OUT
#define U0CTS_IDX       15 //IN
#define U0RTS_IDX       U0CTS_IDX //OUT
#define U0DSR_IDX       16 //IN
#define U0DTR_IDX       U0DSR_IDX //OUT

//UART1
#define U1RXD_IDX       17 //IN
#define U1TXD_IDX       U1RXD_IDX //OUT
#define U1CTS_IDX       18 //IN
#define U1RTS_IDX       U1CTS_IDX //OUT

//I2CM
#define I2CM_SCL_IDX    19 //OUT
#define I2CM_SDA_IDX    20 //I/O

//EXT_I2C
#define EXT_I2C_SCL_IDX 21 //OUT
#define EXT_I2C_SDA_IDX 22 //I/O

//I2CEXT0
#define I2CEXT0_SCL_IDX 29 //I/O
#define I2CEXT0_SDA_IDX 30 //I/O

//I2CEXT1
#define I2CEXT1_SCL_IDX 95 //I/O
#define I2CEXT1_SDA_IDX 96 //I/O

//I2SO
#define I2SO_DATA_IDX   23 //I/O
#define I2SO_BCK_IDX    24 //I/O
#define I2SO_WS_IDX     25 //I/O
#define I2SO_DATA_B_IDX 121//OUT

//I2SI
#define I2SI_DATA_IDX   26 //I/O
#define I2SI_BCK_IDX    27 //I/O
#define I2SI_WS_IDX     28 //I/O
#define I2SI_DATA_B_IDX 122//OUT

//PIN Counter Inputs
#define PCNT_IN00_IDX  39
#define PCNT_IN01_IDX  43
#define PCNT_IN02_IDX  47
#define PCNT_IN03_IDX  51
#define PCNT_IN04_IDX  55
#define PCNT_IN05_IDX  71
#define PCNT_IN06_IDX  75
#define PCNT_IN07_IDX  79

#define PCNT_CTRL00_IDX 41
#define PCNT_CTRL01_IDX 45
#define PCNT_CTRL02_IDX 49
#define PCNT_CTRL03_IDX 53
#define PCNT_CTRL04_IDX 57
#define PCNT_CTRL05_IDX 73
#define PCNT_CTRL06_IDX 77
#define PCNT_CTRL07_IDX 81

#define PCNT_IN10_IDX  40
#define PCNT_IN11_IDX  44
#define PCNT_IN12_IDX  48
#define PCNT_IN13_IDX  52
#define PCNT_IN14_IDX  56
#define PCNT_IN15_IDX  72
#define PCNT_IN16_IDX  76
#define PCNT_IN17_IDX  80

#define PCNT_CTRL10_IDX 42
#define PCNT_CTRL11_IDX 46
#define PCNT_CTRL12_IDX 50
#define PCNT_CTRL13_IDX 54
#define PCNT_CTRL14_IDX 58
#define PCNT_CTRL15_IDX 74
#define PCNT_CTRL16_IDX 78
#define PCNT_CTRL17_IDX 82

//IR REMOTE
#define RMT_IN_IDX(c) (83+(c))
#define RMT_IN0_IDX   83
#define RMT_IN1_IDX   84
#define RMT_IN2_IDX   85
#define RMT_IN3_IDX   86
#define RMT_IN4_IDX   87
#define RMT_IN5_IDX   88
#define RMT_IN6_IDX   89
#define RMT_IN7_IDX   90

#define RMT_OUT_IDX(c) (87+(c))
#define RMT_OUT0_IDX  87
#define RMT_OUT1_IDX  88
#define RMT_OUT2_IDX  89
#define RMT_OUT3_IDX  90
#define RMT_OUT4_IDX  91
#define RMT_OUT5_IDX  92
#define RMT_OUT6_IDX  93
#define RMT_OUT7_IDX  94

#define RMT_OUTB_IDX(c) (118+(c))
#define RMT_OUTB0_IDX 118
#define RMT_OUTB1_IDX 119
#define RMT_OUTB2_IDX 120

//PWM
#define PWM_CAP0_IDX 32 //IN
#define PWM_CAP1_IDX 33 //IN
#define PWM_CAP2_IDX 34 //IN

#define PWM_FLT0_IDX 35 //IN
#define PWM_FLT1_IDX 36 //IN

#define PWM_OUT0_IDX    32
#define PWM_OUT1_IDX    33
#define PWM_OUT2_IDX    34
#define PWM_OUT3_IDX    35
#define PWM_OUT4_IDX    36
#define PWM_OUT5_IDX    37
#define PWM_OUT6_IDX    38
#define PWM_OUT7_IDX    39

#define PWM_OUTB0_IDX   123
#define PWM_OUTB1_IDX   124
#define PWM_OUTB2_IDX   125
#define PWM_OUTB3_IDX   126

//LEDC High Speed
#define LEDC_HS_OUT0_IDX  71
#define LEDC_HS_OUT1_IDX  72
#define LEDC_HS_OUT2_IDX  73
#define LEDC_HS_OUT3_IDX  74
#define LEDC_HS_OUT4_IDX  75
#define LEDC_HS_OUT5_IDX  76
#define LEDC_HS_OUT6_IDX  77
#define LEDC_HS_OUT7_IDX  78

#define LEDC_HS_OUTB0_IDX 112
#define LEDC_HS_OUTB1_IDX 113
#define LEDC_HS_OUTB2_IDX 114

//LEDC Low Speed
#define LEDC_LS_OUT0_IDX  79
#define LEDC_LS_OUT1_IDX  80
#define LEDC_LS_OUT2_IDX  81
#define LEDC_LS_OUT3_IDX  82
#define LEDC_LS_OUT4_IDX  83
#define LEDC_LS_OUT5_IDX  84
#define LEDC_LS_OUT6_IDX  85
#define LEDC_LS_OUT7_IDX  86

#define LEDC_LS_OUTB0_IDX 115
#define LEDC_LS_OUTB1_IDX 116
#define LEDC_LS_OUTB2_IDX 117

//Sigma Delta
#define SD_OUT0_IDX  100
#define SD_OUT1_IDX  101
#define SD_OUT2_IDX  102
#define SD_OUT3_IDX  103
#define SD_OUT4_IDX  104
#define SD_OUT5_IDX  105
#define SD_OUT6_IDX  106
#define SD_OUT7_IDX  107

#define SD_OUTB0_IDX 108
#define SD_OUTB1_IDX 109
#define SD_OUTB2_IDX 110
#define SD_OUTB3_IDX 111

// MISC
#define SDIO_TOHOST_INT_IDX 31 //OUT
#define BT_ACTIVE_IDX       37 //IN
#define BT_PRIORITY_IDX     38 //IN
#define WLAN_ACTIVE_IDX     40 //OUT

//HOST INTERFACE
#define HOST_CARD_DETECT1_IDX       97 //IN
#define HOST_CARD_DETECT2_IDX       98 //IN
#define HOST_CARD_WRITE1_IDX        99 //IN
#define HOST_CARD_WRITE2_IDX        100 //IN
#define HOST_CARD_INT1_IDX          101 //IN
#define HOST_CARD_INT2_IDX          102 //IN

#define HOST_CCMD_OD_PULLUP_EN_IDX  97 //OUT
#define HOST_RST1_IDX               98 //OUT
#define HOST_RST2_IDX               99 //OUT

//DEBUG
#define ATCK_IDX  125 //IN
#define ATMS_IDX  126 //IN
#define ATDI_IDX  127 //IN
#define ATDO_IDX  127 //OUT

//BB_OUT OUTPUTS
#define BB_OUT0_IDX  41
#define BB_OUT1_IDX  42
#define BB_OUT2_IDX  43
#define BB_OUT3_IDX  44
#define BB_OUT4_IDX  45
#define BB_OUT5_IDX  46
#define BB_OUT6_IDX  47
#define BB_OUT7_IDX  48
#define BB_OUT8_IDX  49
#define BB_OUT9_IDX  50
#define BB_OUT10_IDX 51
#define BB_OUT11_IDX 52
#define BB_OUT12_IDX 53
#define BB_OUT13_IDX 54
#define BB_OUT14_IDX 55
#define BB_OUT15_IDX 56
#define BB_OUT16_IDX 57
#define BB_OUT17_IDX 58
#define BB_OUT18_IDX 59
#define BB_OUT19_IDX 60

#ifdef __cplusplus
extern "C" {
#endif

//Pin Matrix
void pinMatrixOutAttach(uint8_t pin, uint8_t function);
void pinMatrixOutDetach(uint8_t pin);
void pinMatrixInAttach(uint8_t pin, uint8_t function);
void pinMatrixInDetach(uint8_t signal, bool high);
void intrMatrixAttach(uint32_t source, uint32_t inum);

#ifdef __cplusplus
}
#endif

#endif
