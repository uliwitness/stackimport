/*
 *  Tests.cpp
 *  stackimport
 *
 *  Created by Uli Kusterer on 01.04.10.
 *  Copyright 2010 The Void Software. All rights reserved.
 *
 */

#include "CStackFile.h"


void	RunTests()
{
	CStackBlockIdentifier		a( "TEST", 1 );
	CStackBlockIdentifier		b( "TEST", 2 );
	CStackBlockIdentifier		wildcardYes( "TEST" );
	CStackBlockIdentifier		wildcardNo( "TOON" );
	
	// ==
	bool	testState = a == b;
	
	testState = a == wildcardYes;
	testState = wildcardYes == a;
	
	testState = b == wildcardYes;
	testState = wildcardYes == b;
	
	testState = a == wildcardNo;
	testState = wildcardNo == a;
	
	testState = b == wildcardNo;
	testState = wildcardNo == b;
	
	// <
	testState = a < b;
	
	testState = a < wildcardYes;
	testState = wildcardYes < a;
	
	testState = b < wildcardYes;
	testState = wildcardYes < b;
	
	testState = a < wildcardNo;
	testState = wildcardNo < a;
	
	testState = b < wildcardNo;
	testState = wildcardNo < b;
	
	// >
	testState = a > b;
	
	testState = a > wildcardYes;
	testState = wildcardYes > a;
	
	testState = b > wildcardYes;
	testState = wildcardYes > b;
	
	testState = a > wildcardNo;
	testState = wildcardNo > a;
	
	testState = b > wildcardNo;
	testState = wildcardNo > b;
}