#include <iostream>
#include "CStackFile.h"

int main( int argc, char * const argv[] )
{
    CStackFile		theStack;
	
	theStack.LoadFile( "/Users/uli/GIT/uli/stackimport/Art Bits.stak" );
	
    return 0;
}
