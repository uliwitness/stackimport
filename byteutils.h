/*
 *  byteutils.h
 *  stackimport
 *
 *  Created by Uli Kusterer on 31.03.10.
 *  Copyright 2010 The Void Software. All rights reserved.
 *
 */

#pragma once

char *		__hex( int x );
void		xornstr( char * dest, const char * src, int n );
void		shiftnstr( char * s, int n, int sh );
