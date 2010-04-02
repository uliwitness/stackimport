/*
 *  byteutils.h
 *  stackimport
 *
 *  Created by Mr. Z. on 03/31/10.
 *  Copyright 2010 Mr Z. All rights reserved.
 *
 */

#pragma once

char *		__hex( int x );
void		xornstr( char * dest, const char * src, int n );
void		shiftnstr( char * s, int n, int sh );
