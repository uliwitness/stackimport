#include <iostream>
#include "CStackFile.h"

int main( int argc, char * const argv[] )
{
    CStackFile		theStack;
	
	if( argc < 2 )
		return 2;
	
	const char*	fpath = argv[1];
	if( !theStack.LoadFile( fpath ) )
		return 3;
	
    return 0;
}
