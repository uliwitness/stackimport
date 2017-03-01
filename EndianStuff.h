/*
 *  EndianStuff.h
 *  stackimport
 *
 *  Created by Mr. Z. on 10/06/06.
 *  Copyright 2006 Mr Z. All rights reserved.
 *
 */

#pragma once

#if MAC_CODE
#include <Carbon/Carbon.h>	// For Endian.h
#endif
#include <stdint.h>

#if TARGET_RT_BIG_ENDIAN
#define BIG_ENDIAN_16(value)	(value)
#define BIG_ENDIAN_32(value)	(value)
#else
#define BIG_ENDIAN_16(value)                 \
        (((((u_int16_t)(value))<<8) & 0xFF00)   | \
         ((((u_int16_t)(value))>>8) & 0x00FF))

#define BIG_ENDIAN_32(value)                     \
        (((((u_int32_t)(value))<<24) & 0xFF000000)  | \
         ((((u_int32_t)(value))<< 8) & 0x00FF0000)  | \
         ((((u_int32_t)(value))>> 8) & 0x0000FF00)  | \
         ((((u_int32_t)(value))>>24) & 0x000000FF))
#endif
