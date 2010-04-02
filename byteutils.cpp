/*
 *  byteutils.cpp
 *  stackimport
 *
 *  Created by Mr. Z. on 03/31/10.
 *  Copyright 2010 Mr Z. All rights reserved.
 *
 */

#include "byteutils.h"
#include <string.h>


char * __hex(int x)
{
	const char	*	hex = "0123456789ABCDEF";
	char			h[] = "ab";
	static char		buf[4] = { 0 };
	
	h[0] = hex[(x/16) % 16];
	h[1] = hex[x % 16];
	strcpy( buf, h );
	
	return buf;
}


// X-Ors the bytes in dest with those in src:
void xornstr(char * dest, const char * src, int n)
{
	int i = 0;
	for (i=0; i<n; i++)
	{
		dest[i] ^= src[i];
	}
}

void shiftnstr(char * s, int n, int sh)
{
	int i = 0;
	int p = 1;
	int x = 0;
	for (i=0; i<sh; i++) { p += p; }	// Bitshift p by sh bits?
	for (i=0; i<n; i++)
	{
		x += ((unsigned char)s[i] * 65536) / p;		// Bitshift by 2 bytes?
		s[i] = x / 65536;							// Store low byte?
		x = (x % 65536) * 256;						// Keep high byte?
	}
}
