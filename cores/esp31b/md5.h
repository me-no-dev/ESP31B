/* 
  md5.h - exposed md5 ROM functions for ESP31B

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP31B core for Arduino environment.
 
  original C source from https://github.com/morrissinger/ESP31B-Websocket/raw/master/MD5.h
 
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
#ifndef __ESP31B_MD5__
#define __ESP31B_MD5__

typedef struct {
  uint32_t state[4];
  uint32_t count[2];
  uint8_t buffer[64];
} md5_context_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void MD5Init (md5_context_t *);
extern void MD5Update (md5_context_t *, uint8_t *, uint16_t);
extern void MD5Final (uint8_t [16], md5_context_t *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
