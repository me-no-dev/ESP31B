/* 
  ESP31B_peri.h - Peripheral registers exposed in more AVR style for ESP31B

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
#ifndef ESP31B_PERI_H_INCLUDED
#define ESP31B_PERI_H_INCLUDED

#include "c_types.h"

#define ESP31B_REG(addr) *((volatile uint32_t *)(0x60000000+(addr)))
#define ESP31B_DREG(addr) *((volatile uint32_t *)(0x3FF00000+(addr)))
#define ESP31B_CLOCK 80000000UL

//CPU Register
//#define CPU2X     ESP31B_DREG(0x14) //when bit 0 is set, F_CPU = 160MHz

//OTP Registers
//#define MAC0      ESP31B_DREG(0x50)
//#define MAC1      ESP31B_DREG(0x54)
//#define CHIPID    ESP31B_DREG(0x58)

//GPIO (0-15) Control Registers
#define GPO0    ESP31B_REG(0x4004) //GPIO_OUT R/W (Output Level)
#define GPOS0   ESP31B_REG(0x4008) //GPIO_OUT_SET WO
#define GPOC0   ESP31B_REG(0x400c) //GPIO_OUT_CLR WO
#define GPO1    ESP31B_REG(0x4010) //GPIO_OUT R/W (Output Level)
#define GPOS1   ESP31B_REG(0x4014) //GPIO_OUT_SET WO
#define GPOC1   ESP31B_REG(0x4018) //GPIO_OUT_CLR WO

#define GPSDS   ESP31B_REG(0x401c) //GPIO_SDIO_SELECT

#define GPE0    ESP31B_REG(0x4020) //GPIO_ENABLE R/W (Enable)
#define GPES0   ESP31B_REG(0x4024) //GPIO_ENABLE_SET WO
#define GPEC0   ESP31B_REG(0x4028) //GPIO_ENABLE_CLR WO
#define GPE1    ESP31B_REG(0x402c) //GPIO_ENABLE R/W (Enable)
#define GPES1   ESP31B_REG(0x4030) //GPIO_ENABLE_SET WO
#define GPEC1   ESP31B_REG(0x4034) //GPIO_ENABLE_CLR WO

#define GPSR    ESP31B_REG(0x4038) //GPIO_STRAP

#define GPI0    ESP31B_REG(0x403c) //GPIO_IN RO (Read Input Level)
#define GPI1    ESP31B_REG(0x4040) //GPIO_IN RO (Read Input Level)

#define GPIE0   ESP31B_REG(0x4044) //GPIO_STATUS R/W (Interrupt Enable)
#define GPIES0  ESP31B_REG(0x4048) //GPIO_STATUS_SET WO
#define GPIEC0  ESP31B_REG(0x404c) //GPIO_STATUS_CLR WO
#define GPIE1   ESP31B_REG(0x4050) //GPIO_STATUS R/W (Interrupt Enable)
#define GPIES1  ESP31B_REG(0x4054) //GPIO_STATUS_SET WO
#define GPIEC1  ESP31B_REG(0x4058) //GPIO_STATUS_CLR WO

#define GPOP0(p) ((GPO0 & (1 << ((p) & 0x1F))) != 0)
#define GPEP0(p) ((GPE0 & (1 << ((p) & 0x1F))) != 0)
#define GPIP0(p) ((GPI0 & (1 << ((p) & 0x1F))) != 0)
#define GPIEP0(p) ((GPIE0 & (1 << ((p) & 0x1F))) != 0)

#define GPOP1(p) ((GPO1 & (1 << ((p-32) & 0x1F))) != 0)
#define GPEP1(p) ((GPE1 & (1 << ((p-32) & 0x1F))) != 0)
#define GPIP1(p) ((GPI1 & (1 << ((p-32) & 0x1F))) != 0)
#define GPIEP1(p) ((GPIE1 & (1 << ((p-32) & 0x1F))) != 0)

//GPIO PIN Control Registers
#define GPC(p) ESP31B_REG(0x4088 + ((p & 0x3F) * 4))

//GPIO (0-39) PIN Control Bits
#define GPCIE  13 //INT_ENABLE 5 bits
#define GPCC   11 //CONFIG 2 bits
#define GPCWE  10 //WAKEUP_ENABLE (can be 1 only when INT_TYPE is high or low)
#define GPCI   7  //INT_TYPE (3bits) 0:disable,1:rising,2:falling,3:change,4:low,5:high
#define GPCD   2  //DRIVER 0:normal,1:open drain
#define GPCS   0  //SOURCE 0:GPIO_DATA,1:SigmaDelta

/*
#define CALI_CONF ESP31B_REG(0x4128)
#define GPIO_CALI_START 31
#define GPIO_CALI_RTC_MAX_M 0x000003FF
#define GPIO_CALI_RTC_MAX 0

#define CALI_DATA ESP31B_REG(0x412c)
#define GPIO_CALI_RDY_SYNC2 31
#define GPIO_CALI_RDY_REAL 30
#define GPIO_CALI_VALUE_SYNC2_M 0x000FFFFF
#define GPIO_CALI_VALUE_SYNC2 0
 */

//GPIO_FUNC_IN_SEL registers
#define GPFIS(f)  ESP31B_REG(0x4130 + ((((f) & 0x7F) / 5) * 4))
#define GPFISB(f) (((f) % 5) * 6) //FUNC Offset
#define GPFISM    0x0000003F //Func Mask

//GPIO_FUNC_OUT_SEL
#define GPFOS(p)  ESP31B_REG(0x4198 + ((((p) & 0xFF) / 4) * 4))
#define GPFOSB(p) (((p) % 4) * 8) //FUNC Offset
#define GPFOSM    0x000000FF //Func Mask

#define GPSFI     ESP31B_REG(0x41c0) //SIG_FUNC_IN_SEL
#define GPIER     ESP31B_DREG(0x7c) //INTERRUPT ENABLE

#define GPFOSP(p, f) (GPFOS(p) = (GPFOS(p) & ~(GPFOSM << GPFOSB(p))) | ((f) << GPFOSB(p)))
#define GPFISP(p, f) do{ GPFIS(f) = (GPFIS(f) & ~(GPFISM << GPFISB(f))) | ((p) << GPFISB(f)); if ((f < 6) || ((f < 16) && (f > 7)) || ((f < 19) && (f > 16)) || ((f < 69) && (f > 62))) GPSFI |= (1 << f); } while(0)


extern uint8_t esp31b_gpioToFn[40];
#define GPF(p) ESP31B_REG(0x9000 + esp31b_gpioToFn[(p & 0x3F)])

//GPIO (0-15) PIN Function Bits
#define GPFSOE 0 //Sleep OE
#define GPFSS  1 //Sleep Sel
#define GPFSPD 2 //Sleep Pulldown
#define GPFSPU 3 //Sleep Pullup
#define GPFSIE 4 //Sleep Interrupt Enable?
#define GPFSD  5 //Sleep Driver 2 bits

#define GPFPD  7 //Pulldown
#define GPFPU  8 //Pullup
#define GPFIE  9 //Interrupt Enable?
#define GPFD   10//Driver 2 bits

#define GPFF   12 //MCU_SEL Function Select 3 bits

#define GPFFS_GPIO(p) (2)
#define GPFFS_BUS(p) (((p)==1||(p)==3)?0:1)

/* Documented but not actually in the hardware
//Timers 0 and 1
#define TMC(t)    ESP31B_REG(0x7000 + (t * 0x24)) //CONFIG
#define TML(t)    ESP31B_REG(0x7004 + (t * 0x24)) //LO
#define TMH(t)    ESP31B_REG(0x7008 + (t * 0x24)) //HI
#define TMU(t)    ESP31B_REG(0x700c + (t * 0x24)) //UPDATE
#define TMAL(t)   ESP31B_REG(0x7010 + (t * 0x24)) //ALARM LO
#define TMAH(t)   ESP31B_REG(0x7014 + (t * 0x24)) //ALARM HI
#define TMLDL(t)  ESP31B_REG(0x7018 + (t * 0x24)) //LOAD LO
#define TMLDH(t)  ESP31B_REG(0x701c + (t * 0x24)) //LOAD HI
#define TMLD(t)   ESP31B_REG(0x7020 + (t * 0x24)) //LOAD

#define TMIE      ESP31B_REG(0x7090) //INT_ENA_TIMERS
#define TMIR      ESP31B_REG(0x7094) //INT_RAW_TIMERS
#define TMIS      ESP31B_REG(0x7098) //INT_ST_TIMERS
#define TMIC      ESP31B_REG(0x709c) //INT_CLR_TIMERS
#define TMCE      ESP31B_REG(0x70fc) //REGCLK
#define TMCLKEN() (TMCE |= (1 << 31))

//Control Register Bits
#define TME       (1 << 31) //EN
#define TMINC     (1 << 30) //INCREASE
#define TMAR      (1 << 29) //AUTORELOAD (restart timer when condition is reached)
#define TMDIV     (13)      //DIVIDER
#define TMDIVM    (0xFFFF)  //DIVIDER MASK
#define TMEIE     (1 << 12) //EDGE INT EN
#define TMLIE     (1 << 11) //LEVEL INT EN
#define TMAE      (1 << 10) //ALARM EN
#define TMSWB     (1 << 9)  //SET WRAP BACK
#define TMCWB     (1 << 8)  //CLR WRAP BACK
#define TMWB      (1 << 7)  //WRAP BACK

//INTERRUPT Register bits (ENA, RAW, ST and CLR)
#define TMLI      (1 << 3)  //LACT_INT
#define TMWI      (1 << 2)  //WDT_INT
#define TMT1      (1 << 1)  //T1_INT
#define TMT0      (1 << 0)  //T0_INT
#define TMT(t)    (1 << t)  //Tx_INT
*/

//Timer 1 Registers (23bit CountDown Timer)
#define T1L  ESP31B_REG(0x7000) //Load Value (Starting Value of Counter) 23bit (0-8388607)
#define T1V  ESP31B_REG(0x7004) //(RO) Current Value
#define T1C  ESP31B_REG(0x7008) //Control Register
#define T1I  ESP31B_REG(0x700C) //Interrupt Status Register (1bit) write to clear
//edge interrupt enable register
#define TEIE 	ESP31B_DREG(0x04)
#define TEIE1	0x02 //bit for timer 1

//Timer 2 Registers (32bit CountUp Timer)
#define T2L  ESP31B_REG(0x7020) //Load Value (Starting Value of Counter)
#define T2V  ESP31B_REG(0x7024) //(RO) Current Value
#define T2C  ESP31B_REG(0x7028) //Control Register
#define T2I  ESP31B_REG(0x702C) //Interrupt Status Register (1bit) write to clear
#define T2A  ESP31B_REG(0x7030) //Alarm Value

//Timer Control Bits
#define TCIS  8 //Interrupt Status
#define TCTE  7 //Timer Enable
#define TCAR  6 //AutoReload (restart timer when condition is reached)
#define TCPD  2 //Prescale Devider (2bit) 0:1(12.5ns/tick), 1:16(0.2us/tick), 2/3:256(3.2us/tick)
#define TCIT  0 //Interrupt Type 0:edge, 1:level



//UART INT Status
//#define UIS   ESP31B_DREG(0x20020)
//#define UIS0  0
//#define UIS1  2


#define UART_RXD_IDX(p) ((p==0)?U0RXD_IDX:((p==1)?U1RXD_IDX:0))
#define UART_TXD_IDX(p) ((p==0)?U0TXD_IDX:((p==1)?U1TXD_IDX:0))

//UART 0 Registers
#define U0F   ESP31B_REG(0x000) //UART FIFO
#define U0IR  ESP31B_REG(0x004) //INT_RAW
#define U0IS  ESP31B_REG(0x008) //INT_STATUS
#define U0IE  ESP31B_REG(0x00c) //INT_ENABLE
#define U0IC  ESP31B_REG(0x010) //INT_CLEAR
#define U0D   ESP31B_REG(0x014) //CLKDIV
#define U0A   ESP31B_REG(0x018) //AUTOBAUD
#define U0S   ESP31B_REG(0x01C) //STATUS
#define U0C0  ESP31B_REG(0x020) //CONF0
#define U0C1  ESP31B_REG(0x024) //CONF1
#define U0LP  ESP31B_REG(0x028) //LOW_PULSE
#define U0HP  ESP31B_REG(0x02C) //HIGH_PULSE
#define U0PN  ESP31B_REG(0x030) //PULSE_NUM
#define U0DT  ESP31B_REG(0x078) //DATE
#define U0ID  ESP31B_REG(0x07C) //ID

//UART 1 Registers
#define U1F   ESP31B_REG(0x10000) //UART FIFO
#define U1IR  ESP31B_REG(0x10004) //INT_RAW
#define U1IS  ESP31B_REG(0x10008) //INT_STATUS
#define U1IE  ESP31B_REG(0x1000c) //INT_ENABLE
#define U1IC  ESP31B_REG(0x10010) //INT_CLEAR
#define U1D   ESP31B_REG(0x10014) //CLKDIV
#define U1A   ESP31B_REG(0x10018) //AUTOBAUD
#define U1S   ESP31B_REG(0x1001C) //STATUS
#define U1C0  ESP31B_REG(0x10020) //CONF0
#define U1C1  ESP31B_REG(0x10024) //CONF1
#define U1LP  ESP31B_REG(0x10028) //LOW_PULSE
#define U1HP  ESP31B_REG(0x1002C) //HIGH_PULSE
#define U1PN  ESP31B_REG(0x10030) //PULSE_NUM
#define U1DT  ESP31B_REG(0x10078) //DATE
#define U1ID  ESP31B_REG(0x1007C) //ID

//UART(uart) Registers
#define USF(u)   ESP31B_REG(0x000+(0x10000*(u&1))) //UART FIFO
#define USIR(u)  ESP31B_REG(0x004+(0x10000*(u&1))) //INT_RAW
#define USIS(u)  ESP31B_REG(0x008+(0x10000*(u&1))) //INT_STATUS
#define USIE(u)  ESP31B_REG(0x00c+(0x10000*(u&1))) //INT_ENABLE
#define USIC(u)  ESP31B_REG(0x010+(0x10000*(u&1))) //INT_CLEAR
#define USD(u)   ESP31B_REG(0x014+(0x10000*(u&1))) //CLKDIV
#define USA(u)   ESP31B_REG(0x018+(0x10000*(u&1))) //AUTOBAUD
#define USS(u)   ESP31B_REG(0x01C+(0x10000*(u&1))) //STATUS
#define USC0(u)  ESP31B_REG(0x020+(0x10000*(u&1))) //CONF0
#define USC1(u)  ESP31B_REG(0x024+(0x10000*(u&1))) //CONF1
#define USLP(u)  ESP31B_REG(0x028+(0x10000*(u&1))) //LOW_PULSE
#define USHP(u)  ESP31B_REG(0x02C+(0x10000*(u&1))) //HIGH_PULSE
#define USPN(u)  ESP31B_REG(0x030+(0x10000*(u&1))) //PULSE_NUM
#define USDT(u)  ESP31B_REG(0x078+(0x10000*(u&1))) //DATE
#define USID(u)  ESP31B_REG(0x07C+(0x10000*(u&1))) //ID

//UART INT Registers Bits
#define UITD 14 //TX Fifo Done
#define UITBID 13 //TX_BRK_IDLE_DONE
#define UITBD 12 //TX_BRK_DONE
#define UIGD 11 //Glitch Detected
#define UIXOF 10 //SW_XOFF
#define UIXON 9 //SW_XON

#define UITO	8 //RX FIFO TimeOut
#define UIBD	7 //Break Detected
#define UICTS	6 //CTS Changed
#define UIDSR	5 //DSR Change
#define UIOF	4 //RX FIFO OverFlow
#define UIFR	3 //Frame Error
#define UIPE	2 //Parity Error
#define UIFE	1 //TX FIFO Empty
#define UIFF	0 //RX FIFO Full


//UART STATUS Registers Bits
#define USTX    31 //TX PIN Level
#define USRTS   30 //RTS PIN Level
#define USDTR   29 //DTR PIN Level
#define USTXC   16 //TX FIFO COUNT (8bit)
#define USRXD   15 //RX PIN Level
#define USCTS   14 //CTS PIN Level
#define USDSR   13 //DSR PIN Level
#define USRXC    0 //RX FIFO COUNT (8bit)

//UART CONF0 Registers Bits
#define UCTRA   27 //TICK_REF_ALWAYS_ON
#define UCEWM   26 //ERR_WR_MASK
#define UCCE    25 //CLK_EN

#define UCDTRI  24 //Invert DTR
#define UCRTSI  23 //Invert RTS
#define UCTXI   22 //Invert TX
#define UCDSRI  21 //Invert DSR
#define UCCTSI  20 //Invert CTS
#define UCRXI   19 //Invert RX
#define UCTXRST 18 //Reset TX FIFO
#define UCRXRST 17 //Reset RX FIFO
#define UCTXHFE 15 //TX Harware Flow Enable
#define UCLBE   14 //LoopBack Enable

#define UCIRRI  13 //IRDA_RX_INV
#define UCIRTI  12 //IRDA_TX_INV
#define UCIRW   11 //IRDA_WCTL
#define UCIRTE  10 //IRDA_TX_EN
#define UCIRDP  9 //IRDA_DPLX

#define UCBRK   8  //Send Break on the TX line
#define UCSWDTR 7  //Set this bit to assert DTR
#define UCSWRTS 6  //Set this bit to assert RTS
#define UCSBN   4  //StopBits Count (2bit) 0:disable, 1:1bit, 2:1.5bit, 3:2bit
#define UCBN    2  //DataBits Count (2bin) 0:5bit, 1:6bit, 2:7bit, 3:8bit
#define UCPAE   1  //Parity Enable
#define UCPA    0  //Parity 0:even, 1:odd

//UART CONF1 Registers Bits
#define UCTOE   31 //RX TimeOut Enable
#define UCTOT   24 //RX TimeOut Treshold (7bit)
#define UCRXHFE 23 //RX Harware Flow Enable
#define UCRXHFT 16 //RX Harware Flow Treshold (7bit)
#define UCFET   8  //TX FIFO Empty Treshold (7bit)
#define UCFFT   0  //RX FIFO Full Treshold (7bit)
/*
//WDT Feed (the dog) Register
#define WDTFEED    ESP31B_REG(0x914)
#define WDT_FEED() (WDTFEED = 0x73)
*/

#define SPI0  0
#define SPI1  1
#define SPI2  2
#define SPI3  3

#define HSPI  2
#define VSPI  3

#define SPIOS(p)    ((p==0)?0x3000:((p==1)?0x2000:((p==2)?0x23000:((p==3)?0x24000:0))))
#define SPI_CLK(p)  ((p==0)?SPICLK_IDX:((p==1)?HSPICLK_IDX:((p==2)?HSPICLK_IDX:((p==3)?VSPICLK_IDX:0))))
#define SPI_MISO(p) ((p==0)?SPIQ_IDX:((p==1)?HSPIQ_IDX:((p==2)?HSPIQ_IDX:((p==3)?VSPIQ_IDX:0))))
#define SPI_MOSI(p) ((p==0)?SPID_IDX:((p==1)?HSPID_IDX:((p==2)?HSPID_IDX:((p==3)?VSPID_IDX:0))))
#define SPI_SS(p)   ((p==0)?SPICS0_IDX:((p==1)?HSPICS0_IDX:((p==2)?HSPICS0_IDX:((p==3)?VSPICS0_IDX:0))))

#define SPI_REG(p,a)  ESP31B_REG(SPIOS(p) + (a))

//SPI1 Registers
#define SPICMD(p)		SPI_REG(p,0x00)
#define SPIA(p) 		SPI_REG(p,0x04)
#define SPIC(p) 		SPI_REG(p,0x08)
#define SPIC1(p) 		SPI_REG(p,0x0C)
#define SPIRS(p) 		SPI_REG(p,0x10)
#define SPIC2(p) 		SPI_REG(p,0x14)
#define SPICLK(p) 	SPI_REG(p,0x18)
#define SPIU(p) 		SPI_REG(p,0x1C)
#define SPIU1(p) 		SPI_REG(p,0x20)
#define SPIU2(p) 		SPI_REG(p,0x24)
#define SPIWS(p) 		SPI_REG(p,0x28)
#define SPIP(p) 		SPI_REG(p,0x2C)
#define SPIS(p) 		SPI_REG(p,0x30)
#define SPIS1(p) 		SPI_REG(p,0x34)
#define SPIS2(p) 		SPI_REG(p,0x38)
#define SPIS3(p) 		SPI_REG(p,0x3C)
#define SPIW0(p) 		SPI_REG(p,0x40)
#define SPIW1(p) 		SPI_REG(p,0x44)
#define SPIW2(p) 		SPI_REG(p,0x48)
#define SPIW3(p) 		SPI_REG(p,0x4C)
#define SPIW4(p) 		SPI_REG(p,0x50)
#define SPIW5(p) 		SPI_REG(p,0x54)
#define SPIW6(p) 		SPI_REG(p,0x58)
#define SPIW7(p) 		SPI_REG(p,0x5C)
#define SPIW8(p) 		SPI_REG(p,0x60)
#define SPIW9(p) 		SPI_REG(p,0x64)
#define SPIW10(p) 	SPI_REG(p,0x68)
#define SPIW11(p) 	SPI_REG(p,0x6C)
#define SPIW12(p) 	SPI_REG(p,0x70)
#define SPIW13(p) 	SPI_REG(p,0x74)
#define SPIW14(p) 	SPI_REG(p,0x78)
#define SPIW15(p) 	SPI_REG(p,0x7C)
#define SPIE0(p) 		SPI_REG(p,0xF0)
#define SPIE1(p) 		SPI_REG(p,0xF4)
#define SPIE2(p) 		SPI_REG(p,0xF8)
#define SPIE3(p) 		SPI_REG(p,0xFC)
#define SPIW(p, i)  SPI_REG(p,(0x40 + ((i & 0xF) * 4)))

//SPI CMD
#define SPICMDREAD (1 << 31) //SPI_FLASH_READ
#define SPICMDWREN (1 << 30) //SPI_FLASH_WREN
#define SPICMDWRDI (1 << 29) //SPI_FLASH_WRDI
#define SPICMDRDID (1 << 28) //SPI_FLASH_RDID
#define SPICMDRDSR (1 << 27) //SPI_FLASH_RDSR
#define SPICMDWRSR (1 << 26) //SPI_FLASH_WRSR
#define SPICMDPP  (1 << 25) //SPI_FLASH_PP
#define SPICMDSE  (1 << 24) //SPI_FLASH_SE
#define SPICMDBE  (1 << 23) //SPI_FLASH_BE
#define SPICMDCE  (1 << 22) //SPI_FLASH_CE
#define SPICMDDP  (1 << 21) //SPI_FLASH_DP
#define SPICMDRES (1 << 20) //SPI_FLASH_RES
#define SPICMDHPM (1 << 19) //SPI_FLASH_HPM
#define SPICMDUSR (1 << 18) //SPI_FLASH_USR
#define SPIBUSY 	(1 << 18) //SPI_USR

//SPI CTRL (SPIxC)
#define SPICWBO       (1 << 26) //SPI_WR_BIT_ODER
#define SPICRBO       (1 << 25) //SPI_RD_BIT_ODER
#define SPICQIO       (1 << 24) //SPI_QIO_MODE
#define SPICDIO       (1 << 23) //SPI_DIO_MODE
#define SPIC2BSE      (1 << 22) //SPI_TWO_BYTE_STATUS_EN
#define SPICWPR       (1 << 21) //SPI_WP_REG
#define SPICQOUT      (1 << 20) //SPI_QOUT_MODE
#define SPICSHARE     (1 << 19) //SPI_SHARE_BUS
#define SPICHOLD      (1 << 18) //SPI_HOLD_MODE
#define SPICAHB       (1 << 17) //SPI_ENABLE_AHB
#define SPICSSTAAI    (1 << 16) //SPI_SST_AAI
#define SPICRESANDRES (1 << 15) //SPI_RESANDRES
#define SPICDOUT      (1 << 14) //SPI_DOUT_MODE
#define SPICFASTRD    (1 << 13) //SPI_FASTRD_MODE

//SPI CTRL1 (SPIxC1)
#define SPIC1TCSH   0xF //SPI_T_CSH
#define SPIC1TCSH_S 28 //SPI_T_CSH_S
#define SPIC1TRES   0xFFF //SPI_T_RES
#define SPIC1TRES_S 16 //SPI_T_RES_S
#define SPIC1BTL    0xFFFF //SPI_BUS_TIMER_LIMIT
#define SPIC1BTL_S  0 //SPI_BUS_TIMER_LIMIT_S

//SPI Status (SPIxRS)
#define SPIRSEXT    0xFF //SPI_STATUS_EXT
#define SPIRSEXT_S  24 //SPI_STATUS_EXT_S
#define SPIRSWB     0xFF //SPI_WB_MODE
#define SPIRSWB_S   16 //SPI_WB_MODE_S
#define SPIRSSP     (1 << 7) //SPI_FLASH_STATUS_PRO_FLAG
#define SPIRSTBP    (1 << 5) //SPI_FLASH_TOP_BOT_PRO_FLAG
#define SPIRSBP2    (1 << 4) //SPI_FLASH_BP2
#define SPIRSBP1    (1 << 3) //SPI_FLASH_BP1
#define SPIRSBP0    (1 << 2) //SPI_FLASH_BP0
#define SPIRSWRE    (1 << 1) //SPI_FLASH_WRENABLE_FLAG
#define SPIRSBUSY   (1 << 0) //SPI_FLASH_BUSY_FLAG

//SPI CTRL2 (SPIxC2)
#define SPIC2CSDN     0xF //SPI_CS_DELAY_NUM
#define SPIC2CSDN_S   28  //SPI_CS_DELAY_NUM_S
#define SPIC2CSDM     0x3 //SPI_CS_DELAY_MODE
#define SPIC2CSDM_S   26  //SPI_CS_DELAY_MODE_S
#define SPIC2MOSIDN   0x7 //SPI_MOSI_DELAY_NUM
#define SPIC2MOSIDN_S 23  //SPI_MOSI_DELAY_NUM_S
#define SPIC2MOSIDM   0x3 //SPI_MOSI_DELAY_MODE
#define SPIC2MOSIDM_S 21  //SPI_MOSI_DELAY_MODE_S
#define SPIC2MISODN   0x7 //SPI_MISO_DELAY_NUM
#define SPIC2MISODN_S 18  //SPI_MISO_DELAY_NUM_S
#define SPIC2MISODM   0x3 //SPI_MISO_DELAY_MODE
#define SPIC2MISODM_S 16  //SPI_MISO_DELAY_MODE_S
#define SPIC2CKOHM    0xF //SPI_CK_OUT_HIGH_MODE
#define SPIC2CKOHM_S  12  //SPI_CK_OUT_HIGH_MODE_S
#define SPIC2CKOLM    0xF //SPI_CK_OUT_LOW_MODE
#define SPIC2CKOLM_S  8   //SPI_CK_OUT_LOW_MODE_S
#define SPIC2HT       0xF //SPI_HOLD_TIME
#define SPIC2HT_S     4   //SPI_HOLD_TIME_S
#define SPIC2ST       0xF //SPI_SETUP_TIME
#define SPIC2ST_S     0   //SPI_SETUP_TIME_S

//SPI CLK (SPIxCLK)
#define SPICLK_EQU_SYSCLK (1 << 31) //SPI_CLK_EQU_SYSCLK
#define SPICLKDIVPRE 0x1FFF //SPI_CLKDIV_PRE
#define SPICLKDIVPRE_S 18 //SPI_CLKDIV_PRE_S
#define SPICLKCN 0x3F //SPI_CLKCNT_N
#define SPICLKCN_S 12 //SPI_CLKCNT_N_S
#define SPICLKCH 0x3F //SPI_CLKCNT_H
#define SPICLKCH_S 6 //SPI_CLKCNT_H_S
#define SPICLKCL 0x3F //SPI_CLKCNT_L
#define SPICLKCL_S 0 //SPI_CLKCNT_L_S

//SPI Phases (SPIxU)
#define SPIUCOMMAND	(1 << 31) //COMMAND pahse, SPI_USR_COMMAND
#define SPIUADDR	(1 << 30) //ADDRESS phase, SPI_FLASH_USR_ADDR
#define SPIUDUMMY	(1 << 29) //DUMMY phase, SPI_FLASH_USR_DUMMY
#define SPIUMISO	(1 << 28) //MISO phase, SPI_FLASH_USR_DIN
#define SPIUMOSI	(1 << 27) //MOSI phase, SPI_FLASH_DOUT
#define SPIUDUMMYIDLE (1 << 26) //SPI_USR_DUMMY_IDLE
#define SPIUMOSIH (1 << 25) //MOSI phase uses W8-W15, SPI_USR_DOUT_HIGHPART
#define SPIUMISOH (1 << 24) //MISO pahse uses W8-W15, SPI_USR_DIN_HIGHPART
#define SPIUPREPHOLD (1 << 23) //SPI_USR_PREP_HOLD
#define SPIUCMDHOLD (1 << 22) //SPI_USR_CMD_HOLD
#define SPIUADDRHOLD (1 << 21) //SPI_USR_ADDR_HOLD
#define SPIUDUMMYHOLD (1 << 20) //SPI_USR_DUMMY_HOLD
#define SPIUMISOHOLD (1 << 19) //SPI_USR_DIN_HOLD
#define SPIUMOSIHOLD (1 << 18) //SPI_USR_DOUT_HOLD
#define SPIUHOLDPOL (1 << 17) //SPI_USR_HOLD_POL
#define SPIUSIO (1 << 16) //SPI_SIO
#define SPIUFWQIO (1 << 15) //SPI_FWRITE_QIO
#define SPIUFWDIO (1 << 14) //SPI_FWRITE_DIO
#define SPIUFWQUAD (1 << 13) //SPI_FWRITE_QUAD
#define SPIUFWDUAL (1 << 12) //SPI_FWRITE_DUAL
#define SPIUWRBYO (1 << 11) //SPI_WR_BYTE_ORDER
#define SPIURDBYO (1 << 10) //SPI_RD_BYTE_ORDER
#define SPIUAHBEM 0x3 //SPI_AHB_ENDIAN_MODE
#define SPIUAHBEM_S 8 //SPI_AHB_ENDIAN_MODE_S
#define SPIUSME (1 << 7) //SPI Master Edge (0:falling, 1:rising), SPI_CK_OUT_EDGE
#define SPIUSSE   (1 << 6) //SPI Slave Edge (0:falling, 1:rising), SPI_CK_I_EDGE
#define SPIUCSSETUP (1 << 5) //SPI_CS_SETUP
#define SPIUCSHOLD (1 << 4) //SPI_CS_HOLD
#define SPIUAHBUCMD (1 << 3) //SPI_AHB_USR_COMMAND
#define SPIUAHBUCMD4B (1 << 1) //SPI_AHB_USR_COMMAND_4BYTE
#define SPIUDUPLEX (1 << 0) //SPI_DOUTDIN

//SPI Phase Length Locations
#define SPILCOMMAND	28 //4 bit in SPIxU2 default 7 (8bit)
#define SPILADDR	26 //6 bit in SPIxU1 default:23 (24bit)
#define SPILDUMMY	0  //8 bit in SPIxU1 default:0 (0 cycles)
#define SPILMISO	8  //9 bit in SPIxU1 default:0 (1bit)
#define SPILMOSI	17 //9 bit in SPIxU1 default:0 (1bit)
//SPI Phase Length Masks
#define SPIMCOMMAND	0xF
#define SPIMADDR	0x3F
#define SPIMDUMMY	0xFF
#define SPIMMISO	0x1FF
#define SPIMMOSI	0x1FF

//SPI Slave (SPIxS)
#define SPISSRES    (1 << 31) //SYNC RESET, SPI_SYNC_RESET
#define SPISE       (1 << 30) //Slave Enable, SPI_SLAVE_MODE
#define SPISBE      (1 << 29) //WR/RD BUF enable, SPI_SLV_WR_RD_BUF_EN
#define SPISSE      (1 << 28) //STA enable, SPI_SLV_WR_RD_STA_EN
#define SPISCD      (1 << 27) //CMD define, SPI_SLV_CMD_DEFINE
#define SPISTRCNT   0xF //SPI_TRANS_CNT
#define SPISTRCNT_S 23 //SPI_TRANS_CNT_S
#define SPISSLS     0x7 //SPI_SLV_LAST_STATE
#define SPISSLS_S   20 //SPI_SLV_LAST_STATE_S
#define SPISSLC     0x7 //SPI_SLV_LAST_COMMAND
#define SPISSLC_S   17 //SPI_SLV_LAST_COMMAND_S
#define SPISCSIM    0x3 //SPI_CS_I_MODE
#define SPIDCSIM_S  10 //SPI_CS_I_MODE_S
#define SPISTRIE    (1 << 9)  //TRANS interrupt enable
#define SPISWSIE    (1 << 8)  //WR_STA interrupt enable
#define SPISRSIE    (1 << 7)  //RD_STA interrupt enable
#define SPISWBIE    (1 << 6)  //WR_BUF interrupt enable
#define SPISRBIE    (1 << 5)  //RD_BUF interrupt enable
#define SPISTRIS    (1 << 4)  //TRANS interrupt status
#define SPISWSIS    (1 << 3)  //WR_STA interrupt status
#define SPISRSIS    (1 << 2)  //RD_STA interrupt status
#define SPISWBIS    (1 << 1)  //WR_BUF interrupt status
#define SPISRBIS    (1 << 0)  //RD_BUF interrupt status

//SPI Slave1 (SPIxS1)
#define SPIS1LSTA   27 //5 bit in SPIxS1 default:0 (1bit), SPI_SLV_STATUS_BITLEN
#define SPIS1FE     (1 << 26) //SPI_SLV_STATUS_FAST_EN
#define SPIS1RSTA   (1 << 25) //default:0 enable STA read from Master, SPI_SLV_STATUS_READBACK
#define SPIS1LBUF   16 //9 bit in SPIxS1 default:0 (1bit), SPI_SLV_BUF_BITLEN
#define SPIS1LRBA   10 //6 bit in SPIxS1 default:0 (1bit), SPI_SLV_RD_ADDR_BITLEN
#define SPIS1LWBA   4  //6 bit in SPIxS1 default:0 (1bit), SPI_SLV_WR_ADDR_BITLEN
#define SPIS1WSDE   (1 << 3) //SPI_SLV_WRSTA_DUMMY_EN
#define SPIS1RSDE   (1 << 2) //SPI_SLV_RDSTA_DUMMY_EN
#define SPIS1WBDE   (1 << 1) //SPI_SLV_WRBUF_DUMMY_EN
#define SPIS1RBDE   (1 << 0) //SPI_SLV_RDBUF_DUMMY_EN

//SPI Slave2 (SPIxS2)
#define SPIS2WBDL 0xFF //SPI_SLV_WRBUF_DUMMY_CYCLELEN
#define SPIS2WBDL_S 24 //SPI_SLV_WRBUF_DUMMY_CYCLELEN_S
#define SPIS2RBDL 0xFF //SPI_SLV_RDBUF_DUMMY_CYCLELEN
#define SPIS2RBDL_S 16 //SPI_SLV_RDBUF_DUMMY_CYCLELEN_S
#define SPIS2WSDL 0xFF //SPI_SLV_WRSTA_DUMMY_CYCLELEN
#define SPIS2WSDL_S 8 //SPI_SLV_WRSTA_DUMMY_CYCLELEN_S
#define SPIS2RSDL 0xFF //SPI_SLV_RDSTA_DUMMY_CYCLELEN
#define SPIS2RSDL_S 0 //SPI_SLV_RDSTA_DUMMY_CYCLELEN_S

//SPI Slave3 (SPIxS3)
#define SPIS3WSCV 0xFF //SPI_SLV_WRSTA_CMD_VALUE
#define SPIS3WSCV_S 24 //SPI_SLV_WRSTA_CMD_VALUE_S
#define SPIS3RSCV 0xFF //SPI_SLV_RDSTA_CMD_VALUE
#define SPIS3RSCV_S 16 //SPI_SLV_RDSTA_CMD_VALUE_S
#define SPIS3WBCV 0xFF //SPI_SLV_WRBUF_CMD_VALUE
#define SPIS3WBCV_S 8 //SPI_SLV_WRBUF_CMD_VALUE_S
#define SPIS3RBCV 0xFF //SPI_SLV_RDBUF_CMD_VALUE
#define SPIS3RBCV_S 0 //SPI_SLV_RDBUF_CMD_VALUE_S

//SPI EXT0 (SPIxE0)
#define SPIE0TPPEN (1 << 31) //SPI_T_PP_ENA
#define SPIE0TPPS 0xF //SPI_T_PP_SHIFT
#define SPIE0TPPS_S 16 //SPI_T_PP_SHIFT_S
#define SPIE0TPPT 0xFFF //SPI_T_PP_TIME
#define SPIE0TPPT_S 0 //SPI_T_PP_TIME_S

//SPI EXT1 (SPIxE1)
#define SPIE1TEREN (1 << 31) //SPI_T_ERASE_ENA
#define SPIE1TERS 0xF //SPI_T_ERASE_SHIFT
#define SPIE1TERS_S 16 //SPI_T_ERASE_SHIFT_S
#define SPIE1TERT 0xFFF //SPI_T_ERASE_TIME
#define SPIE1TERT_S 0 //SPI_T_ERASE_TIME_S

//SPI EXT2 (SPIxE2)
#define SPIE2ST 0x7 //SPI_ST
#define SPIE2ST_S 0 //SPI_ST_S

//SPI EXT3 (SPIxE3)
#define SPIE2IHEN 0x3 //SPI_INT_HOLD_ENA
#define SPIE2IHEN_S 0 //SPI_INT_HOLD_ENA_S


// I2S

#define I2SBASEFREQ                       (16000000L)

#define I2STXF  ESP31B_REG(0xF000) //I2STXFIFO (32bit)
#define I2SRXF  ESP31B_REG(0xF004) //I2SRXFIFO (32bit)
#define I2SC    ESP31B_REG(0xF008) //I2SCONF
#define I2SIR   ESP31B_REG(0xF00C) //I2SINT_RAW
#define I2SIS   ESP31B_REG(0xF010) //I2SINT_ST
#define I2SIE   ESP31B_REG(0xF014) //I2SINT_ENA
#define I2SIC   ESP31B_REG(0xF018) //I2SINT_CLR
#define I2ST    ESP31B_REG(0xF01C) //I2STIMING
#define I2SFC   ESP31B_REG(0xF020) //I2S_FIFO_CONF
#define I2SRXEN ESP31B_REG(0xF024) //I2SRXEOF_NUM (32bit)
#define I2SCSD  ESP31B_REG(0xF028) //I2SCONF_SIGLE_DATA (32bit)
#define I2SCC   ESP31B_REG(0xF02C) //I2SCONF_CHAN

#define SLCRXL    ESP31B_REG(0xF030) //I2SRX_LINK
#define SLCTXL    ESP31B_REG(0xF034) //I2STX_LINK
#define SLCRXEDA  ESP31B_REG(0xF038) //I2S_TO_EOF_DES_ADDR
#define SLCTXEDA  ESP31B_REG(0xF03c) //I2S_TX_EOF_DES_ADDR
#define SLCRXEBDA ESP31B_REG(0xF040) //I2S_TO_EOF_BFR_DES_ADDR
#define SLCAT     ESP31B_REG(0xF044) //I2S_AHB_TEST
#define SLCTXD    ESP31B_REG(0xF048) //I2S_TXLINK_DSCR
#define SLCTXDB0  ESP31B_REG(0xF04C) //I2S_TXLINK_DSCR_BF0
#define SLCTXDB1  ESP31B_REG(0xF050) //I2S_TXLINK_DSCR_BF1
#define SLCRXD    ESP31B_REG(0xF054) //I2S_RXLINK_DSCR
#define SLCRXDB0  ESP31B_REG(0xF058) //I2S_RXLINK_DSCR_BF0
#define SLCRXDB1  ESP31B_REG(0xF05C) //I2S_RXLINK_DSCR_BF1
#define SLCC0     ESP31B_REG(0xF060) //I2S_LC_CONF
#define SLCRXP    ESP31B_REG(0xF064) //I2S_RXFIFO_PUSH
#define SLCTXP    ESP31B_REG(0xF068) //I2S_TXFIFO_POP
#define SLCS0     ESP31B_REG(0xF06C) //I2S_LC_STATE0
#define SLCS1     ESP31B_REG(0xF070) //I2S_LC_STATE1
#define SLCHC     ESP31B_REG(0xF074) //I2S_LC_HUNG_CONF

/*
#define I2S_CVSD_CONF0      ESP31B_REG(0xF080)
#define I2S_CVSD_CONF1      ESP31B_REG(0xF084)
#define I2S_CVSD_CONF2      ESP31B_REG(0xF088)
#define I2S_PLC_CONF0       ESP31B_REG(0xF08C)
#define I2S_PLC_CONF1       ESP31B_REG(0xF090)
#define I2S_ESCO_CONF0      ESP31B_REG(0xF098)
#define I2S_SCO_CONF0       ESP31B_REG(0xF09c)
#define I2SCONF1            ESP31B_REG(0xF0a0)
#define I2S_PD_CONF         ESP31B_REG(0xF0a4)
*/
// I2S CONF
#define I2SCDMSM 0x3      //I2S_CLKM_DIV_NUM_MSB
#define I2SCDMS 30        //I2S_CLKM_DIV_NUM_MSB_S
#define I2SSL   (1 << 29) //I2S_I2S_SIG_LOOPBACK
#define I2SCE   (1 << 28) //I2S_CLK_EN
#define I2SBDM  (0x3F)    //I2S_BCK_DIV_NUM
#define I2SBD   (22)      //I2S_BCK_DIV_NUM_S
#define I2SCDM  (0x3F)    //I2S_CLKM_DIV_NUM
#define I2SCD   (16)      //I2S_CLKM_DIV_NUM_S
#define I2SBMM  (0xF)     //I2S_BITS_MOD
#define I2SBM   (12)      //I2S_BITS_MOD_S
#define I2SRMS  (1 << 11) //I2S_RECE_MSB_SHIFT
#define I2STMS  (1 << 10) //I2S_TRANS_MSB_SHIFT
#define I2SRXS  (1 << 9)  //I2S_I2S_RX_START
#define I2STXS  (1 << 8)  //I2S_I2S_TX_START
#define I2SMR   (1 << 7)  //I2S_MSB_RIGHT
#define I2SRF   (1 << 6)  //I2S_RIGHT_FIRST
#define I2SRSM  (1 << 5)  //I2S_RECE_SLAVE_MOD
#define I2STSM  (1 << 4)  //I2S_TRANS_SLAVE_MOD
#define I2SRXFR (1 << 3)  //I2S_I2S_RX_FIFO_RESET
#define I2STXFR (1 << 2)  //I2S_I2S_TX_FIFO_RESET
#define I2SRXR  (1 << 1)  //I2S_I2S_RX_RESET
#define I2STXR  (1 << 0)  //I2S_I2S_TX_RESET
#define I2SRST  (0xF)     //I2S_I2S_RESET_MASK

//I2S INT
#define I2SITTE   (1 << 16) //I2S_I2S_TO_TOTAL_EOF_INT
#define I2SITXDE  (1 << 15) //I2S_I2S_TX_DSCR_EMPTY_INT
#define I2SIRXDER (1 << 14) //I2S_I2S_RX_DSCR_ERR_INT
#define I2SITXDER (1 << 13) //I2S_I2S_TX_DSCR_ERR_INT
#define I2SITE    (1 << 12) //I2S_I2S_TO_EOF_INT
#define I2SIRXD   (1 << 11) //I2S_I2S_RX_DONE_INT
#define I2SIFERE  (1 << 10) //I2S_I2S_FROM_ERR_EOF_INT
#define I2SIFSE   (1 << 9) //I2S_I2S_FROM_SUC_EOF_INT
#define I2SITXD   (1 << 8) //I2S_I2S_TX_DONE_INT
#define I2SITXH   (1 << 7) //I2S_I2S_TX_HUNG_INT
#define I2SIRXH   (1 << 6) //I2S_I2S_RX_HUNG_INT
#define I2SITXRE  (1 << 5) //I2S_I2S_TX_REMPTY_INT
#define I2SITXWF  (1 << 4) //I2S_I2S_TX_WFULL_INT
#define I2SIRXRE  (1 << 3) //I2S_I2S_RX_REMPTY_INT
#define I2SIRXWF  (1 << 2) //I2S_I2S_RX_WFULL_INT
#define I2SITXPD  (1 << 1) //I2S_I2S_TX_PUT_DATA_INT
#define I2SIRXTD  (1 << 0) //I2S_I2S_RX_TAKE_DATA_INT

//I2S TIMING
#define I2STBII  (1 << 22)  //I2S_TRANS_BCK_IN_INV
#define I2SRDS   (1 << 21)  //I2S_RECE_DSYNC_SW
#define I2STDS   (1 << 20)  //I2S_TRANS_DSYNC_SW
#define I2SRBODM (0x3)      //I2S_RECE_BCK_OUT_DELAY
#define I2SRBOD  (18)       //I2S_RECE_BCK_OUT_DELAY_S
#define I2SRWODM (0x3)      //I2S_RECE_WS_OUT_DELAY
#define I2SRWOD  (16)       //I2S_RECE_WS_OUT_DELAY_S
#define I2STSODM (0x3)      //I2S_TRANS_SD_OUT_DELAY
#define I2STSOD  (14)       //I2S_TRANS_SD_OUT_DELAY_S
#define I2STWODM (0x3)      //I2S_TRANS_WS_OUT_DELAY
#define I2STWOD  (12)       //I2S_TRANS_WS_OUT_DELAY_S
#define I2STBODM (0x3)      //I2S_TRANS_BCK_OUT_DELAY
#define I2STBOD  (10)       //I2S_TRANS_BCK_OUT_DELAY_S
#define I2SRSIDM (0x3)      //I2S_RECE_SD_IN_DELAY
#define I2SRSID  (8)        //I2S_RECE_SD_IN_DELAY_S
#define I2SRWIDM (0x3)      //I2S_RECE_WS_IN_DELAY
#define I2SRWID  (6)        //I2S_RECE_WS_IN_DELAY_S
#define I2SRBIDM (0x3)      //I2S_RECE_BCK_IN_DELAY
#define I2SRBID  (4)        //I2S_RECE_BCK_IN_DELAY_S
#define I2STWIDM (0x3)      //I2S_TRANS_WS_IN_DELAY
#define I2STWID  (2)        //I2S_TRANS_WS_IN_DELAY_S
#define I2STBIDM (0x3)      //I2S_TRANS_BCK_IN_DELAY
#define I2STBID  (0)        //I2S_TRANS_BCK_IN_DELAY_S

//I2S FIFO CONF
#define I2SRXFMM (0x7)      //I2S_I2S_RX_FIFO_MOD
#define I2SRXFM  (16)       //I2S_I2S_RX_FIFO_MOD_S
#define I2STXFMM (0x7)      //I2S_I2S_TX_FIFO_MOD
#define I2STXFM  (13)       //I2S_I2S_TX_FIFO_MOD_S
#define I2SDE    (1 << 12)  //I2S_I2S_DSCR_EN
#define I2STXDNM (0x3F)     //I2S_I2S_TX_DATA_NUM
#define I2STXDN  (6)        //I2S_I2S_TX_DATA_NUM_S
#define I2SRXDNM (0x3F)     //I2S_I2S_RX_DATA_NUM
#define I2SRXDN  (0)        //I2S_I2S_RX_DATA_NUM_S

//I2S CONF CHAN
#define I2SRXCMM (0x3)      //I2S_RX_CHAN_MOD
#define I2SRXCM  (3)        //I2S_RX_CHAN_MOD_S
#define I2STXCMM (0x7)      //I2S_TX_CHAN_MOD
#define I2STXCM  (0)        //I2S_TX_CHAN_MOD_S


//SLC (DMA) RX_LINK
#define SLCRXLP   (1 << 31) //I2S_I2S_RXLINK_PARK
#define SLCRXLRS  (1 << 30) //I2S_I2S_RXLINK_RESTART
#define SLCRXLS   (1 << 29) //I2S_I2S_RXLINK_START
#define SLCRXLE   (1 << 28) //I2S_I2S_RXLINK_STOP
#define SLCRXLAM  (0xFFFF)  //I2S_I2S_RXLINK_ADDR
#define SLCRXLA   (0)       //I2S_I2S_RXLINK_ADDR_S

//SLC (DMA) TX_LINK
#define SLCTXLP   (1 << 31) //I2S_I2S_TXLINK_PARK
#define SLCTXLRS  (1 << 30) //I2S_I2S_TXLINK_RESTART
#define SLCTXLS   (1 << 29) //I2S_I2S_TXLINK_START
#define SLCTXLE   (1 << 28) //I2S_I2S_TXLINK_STOP
#define SLCTXLAM  (0xFFFFF) //I2S_I2S_TXLINK_ADDR
#define SLCTXLA   (0)       //I2S_I2S_TXLINK_ADDR_S

//SLC (DMA) AHB_TEST
#define SLCATAM  (0x3)  //I2S_I2S_AHB_TESTADDR
#define SLCATA   (4)    //I2S_I2S_AHB_TESTADDR_S
#define SLCATMM  (0x7)  //I2S_I2S_AHB_TESTMODE
#define SLCATM   (0)    //I2S_I2S_AHB_TESTMODE_S

//SLC (DMA) CONF0
#define SLCMTE    (1 << 13)//I2S_I2S_MEM_TRANS_EN
#define SLCCO     (1 << 12)//I2S_I2S_CHECK_OWNER
#define SLCRXDBE  (1 << 11)//I2S_I2S_RX_DATA_BURST_EN
#define SLCTXBE   (1 << 10)//I2S_I2S_TXDSCR_BURST_EN
#define SLCRXBE   (1 << 9) //I2S_I2S_RXDSCR_BURST_EN
#define SLCTEM    (1 << 8) //I2S_I2S_TO_EOF_MODE
#define SLCRXNRC  (1 << 7) //I2S_I2S_RX_NO_RESTART_CLR
#define SLCRXAW   (1 << 6) //I2S_I2S_RX_AUTO_WRBACK
#define SLCRXLT   (1 << 5) //I2S_I2S_RX_LOOP_TEST
#define SLCTXLT   (1 << 4) //I2S_I2S_TX_LOOP_TEST
#define SLCAR     (1 << 3) //I2S_I2S_AHBM_RST
#define SLCAFR    (1 << 2) //I2S_I2S_AHBM_FIFO_RST
#define SLCRXLR   (1 << 1) //I2S_I2S_RX_RST
#define SLCTXLR   (1 << 0) //I2S_I2S_TX_RST

//SLC (DMA) RX_FIFO_PUSH
#define SLCRXFP   (1 << 16) //I2S_I2S_RXFIFO_PUSH
#define SLCRXWDM  (0x1FF)   //I2S_I2S_RXFIFO_WDATA
#define SLCRXWD   (0)       //I2S_I2S_RXFIFO_WDATA_S

//SLC (DMA) TX_FIFO_POP
#define SLCTXFP   (1 << 16) //I2S_I2S_TXFIFO_POP
#define SLCTXRDM  (0x7FF)   //I2S_I2S_TXFIFO_RDATA
#define SLCTXRD   (0)       //I2S_I2S_TXFIFO_RDATA_S

//SLC HUNG CONF
#define SLCFTE    (1 << 11) //I2S_I2S_LC_FIFO_TIMEOUT_ENA
#define SLCFTSM   (0x7)     //I2S_I2S_LC_FIFO_TIMEOUT_SHIFT 0x00000007
#define SLCFTS    (8)       //I2S_I2S_LC_FIFO_TIMEOUT_SHIFT_S 8
#define SLCFTM    (0xFF)    //I2S_I2S_LC_FIFO_TIMEOUT 0x000000FF
#define SLCFT     (0)       //I2S_I2S_LC_FIFO_TIMEOUT_S 0

/* NOT WORKING
//IR REMOTE
#define IRD(c)                ESP31B_REG(0x4400 + ((c) * 4)) //DATA
#define IRC0(c)               ESP31B_REG(0x4420 + ((c) * 8)) //CONF0
#define IRC1(c)               ESP31B_REG(0x4424 + ((c) * 8)) //CONF1
#define IRS(c)                ESP31B_REG(0x4460 + ((c) * 4)) //STATUS
#define IRA(c)                ESP31B_REG(0x4480 + ((c) * 4)) //ADDRESS
#define IRCD(c)               ESP31B_REG(0x44b0 + ((c) * 4)) //CARRIER DUTY (16bit high + 16bit low)
#define IRIR                  ESP31B_REG(0x44a0) //INT RAW
#define IRIS                  ESP31B_REG(0x44a4) //INT STATUS
#define IRIE                  ESP31B_REG(0x44a8) //INT ENABLE
#define IRIC                  ESP31B_REG(0x44ac) //INT CLEAR

//CONF0 BITS
#define RMT_CLK_EN            (BIT(31))
#define RMT_MEM_PD            (BIT(30))
#define RMT_CARRIER_OUT_LV    (BIT(29))
#define RMT_CARRIER_EN        (BIT(28))
#define RMT_MEM_SIZE_M        (0xF)
#define RMT_MEM_SIZE          (24)
#define RMT_IDLE_THRES_M      (0xFFFF)
#define RMT_IDLE_THRES        (8)
#define RMT_DIV_CNT_M         (0xFF)
#define RMT_DIV_CNT           (0)

//CONF1 BITS
#define RMT_REF_CNT_RST       (BIT(16))
#define RMT_RX_FILTER_THRES_M (0xFF)
#define RMT_RX_FILTER_THRES   (8)
#define RMT_RX_FILTER_EN      (BIT(7))
#define RMT_TX_CONTI_MODE     (BIT(6))
#define RMT_MEM_OWNER         (BIT(5))
#define RMT_APB_MEM_RST       (BIT(4))
#define RMT_MEM_RD_RST        (BIT(3))
#define RMT_MEM_WR_RST        (BIT(2))
#define RMT_RX_EN             (BIT(1))
#define RMT_TX_START          (BIT(0))

//INT BITS
#define RMT_ERR_INT(c)        (1 << (((c) * 3) + 2))
#define RMT_RX_END_INT(c)     (1 << (((c) * 3) + 1))
#define RMT_TX_END_INT(c)     (1 << ((c) * 3))
*/







#define DR_REG_UART_BASE        0x60000000
//#define DR_REG_?????_BASE     0x60001000
#define DR_REG_HSPI_BASE        0x60002000
#define DR_REG_SPI_BASE         0x60003000
//#define DR_REG_GPIO_BASE      0x60004000
//#define DR_REG_RMT_BASE       0x60004400
//#define DR_REG_GPIO_SD_BASE   0x60004f00
#define DR_REG_FE2_BASE         0x60005000
#define DR_REG_FE_BASE          0x60006000
#define DR_REG_TIMER_BASE       0x60007000
//#define DR_REG_RTC_BASE       0x60008000
//#define DR_REG_RTCIO_BASE     0x60008400
//#define DR_REG_IO_MUX_BASE    0x60009000
#define DR_REG_WDG_BASE         0x6000A000
#define DR_REG_HINF_BASE        0x6000B000
#define DR_REG_UHCI1_BASE       0x6000C000
#define DR_REG_MISC_BASE        0x6000D000
#define DR_REG_I2C_BASE         0x6000E000
#define DR_REG_I2S_BASE         0x6000F000 
#define DR_REG_UART1_BASE       0x60010000
#define DR_REG_BT_BASE          0x60011000
#define DR_REG_BT_BUFFER_BASE   0x60012000
#define DR_REG_I2C_EXT_BASE     0x60013000
#define DR_REG_UHCI0_BASE       0x60014000
#define DR_REG_SLCHOST_BASE     0x60015000
//#define DR_REG_?????_BASE     0x60016000
//#define DR_REG_PCNT_BASE      0x60017000
#define DR_REG_SLC_BASE         0x60018000
//#define DR_REG_LEDC_BASE      0x60019000
#define DR_REG_EFUSE_BASE       0x6001A000
#define DR_REG_SPI_ENCRYPT_BASE 0x6001B000
#define DR_REG_PWM_BASE         0x6001C000
#define DR_REG_TIMERGROUP_BASE  0x6001D000
#define DR_REG_TIMERGROUP1_BASE 0x6001E000
#define DR_REG_BB_BASE          0x6001F000

#define TIMERGROUP_LACTLOADHI (DR_REG_TIMERGROUP_BASE +0x0088)
#define TIMERGROUP1_LACTLOADHI (DR_REG_TIMERGROUP1_BASE +0x0088)

#define APB_CTRL_SYSCLK_CONF (DR_REG_APB_CTRL_BASE + 0x0)
#define APB_CTRL_CLK_EN (BIT(11))
#define APB_CTRL_CLK_EN_S 11
#define APB_CTRL_APB_CTRL_CLK_320M_EN (BIT(10))
#define APB_CTRL_APB_CTRL_CLK_320M_EN_S 10
#define APB_CTRL_APB_CTRL_PRE_DIV_CNT 0x000003FF
#define APB_CTRL_APB_CTRL_PRE_DIV_CNT_S 0

#define APB_CTRL_XTAL_TICK_CONF (DR_REG_APB_CTRL_BASE + 0x4)
#define APB_CTRL_APB_CTRL_XTAL_TICK_NUM 0x0000FFFF
#define APB_CTRL_APB_CTRL_XTAL_TICK_NUM_S 0

#define APB_CTRL_PLL_TICK_CONF (DR_REG_APB_CTRL_BASE + 0x8)
#define APB_CTRL_APB_CTRL_PLL_TICK_NUM 0x0000FFFF
#define APB_CTRL_APB_CTRL_PLL_TICK_NUM_S 0

#define APB_CTRL_CK12M_TICK_CONF (DR_REG_APB_CTRL_BASE + 0xC)
#define APB_CTRL_APB_CTRL_CK12M_TICK_NUM 0x0000FFFF
#define APB_CTRL_APB_CTRL_CK12M_TICK_NUM_S 0

#include "esp31b_matrix.h"
#include "esp31b_timer.h"
#include "esp31b_sigma_delta.h"
#include "esp31b_ledc.h"
#include "esp31b_i2s.h"

#endif
