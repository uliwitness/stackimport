/*
 *  Tests.cpp
 *  stackimport
 *
 *  Created by Mr. Z. on 10/06/06.
 *  Copyright 2006 Mr Z. All rights reserved.
 *
 */

#include "CStackFile.h"
#include <assert.h>


void	RunTests()
{
	CStackBlockIdentifier		a( "TEST", 1 );
	CStackBlockIdentifier		b( "TEST", 2 );
	CStackBlockIdentifier		wildcardYes( "TEST" );
	CStackBlockIdentifier		wildcardNo( "TOON" );
	
	// ==
	assert((a == b) == false);
	
	assert(a == wildcardYes);
	assert(wildcardYes == a);
	
	assert(b == wildcardYes);
	assert(b == wildcardYes);
	
	assert((a == wildcardNo) == false);
	assert((wildcardNo == a) == false);
	
	assert((b == wildcardNo) == false);
	assert((wildcardNo == b) == false);
	
	// <
	assert( a < b );
	
	assert( (a < wildcardYes) == false );
	assert( (wildcardYes < a) == false );
	
	assert( (b < wildcardYes) == false );
	assert( (wildcardYes < b) == false );
	
	assert( a < wildcardNo );
	assert( (wildcardNo < a) == false );
	
	assert( b < wildcardNo );
	assert( (wildcardNo < b) == false );
	
	// >
	assert( (a > b) == false );
	
	assert( (a > wildcardYes) == false );
	assert( (wildcardYes > a) == false );
	
	assert( (b > wildcardYes) == false );
	assert( (wildcardYes > b) == false );
	
	assert( (a > wildcardNo) == false );
	assert( wildcardNo > a );
	
	assert( (b > wildcardNo) == false );
	assert( wildcardNo > b );
}