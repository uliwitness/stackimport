/*
 *  CStackFile.cpp
 *  stackimport
 *
 *  Created by Uli Kusterer on 06.10.06.
 *  Copyright 2006 M. Uli Kusterer. All rights reserved.
 *
 */

#include "CStackFile.h"
#include <iostream>
#include <fstream>
#include "picture.h"
#include "woba.h"
#include "EndianStuff.h"


bool	CStackFile::LoadFile( const std::string& fpath )
{
	std::ifstream		theFile( fpath.c_str() );
	if( !theFile.is_open() )
		return false;
	
	// Read block header:
	char		vBlockType[5] = { 0 };
	u_int32_t	vBlockSize = 0,
				vBlockID = 0;
	
	while( true )
	{
		theFile.read( (char*) &vBlockSize, sizeof(vBlockSize) );
		if( theFile.eof() )	// Couldn't read because we hit end of file.
			break;
		
		vBlockSize = BIG_ENDIAN_32(vBlockSize);
		theFile.read( vBlockType, 4 );
		theFile.read( (char*) &vBlockID, sizeof(vBlockID) );
		vBlockID = BIG_ENDIAN_32(vBlockID);
		
		if( strcmp(vBlockType,"TAIL") == 0 && vBlockID == 0xffffffff )	// End marker block?
			break;
		else if( strcmp(vBlockType,"BMAP") == 0 )	// Image block?
		{
			printf( "'%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			picture		thePicture;
			char*		pictureData = new char[vBlockSize -12];
			theFile.read( pictureData, vBlockSize -12 );
			woba_decode( thePicture, pictureData );
			
			char		fname[256];
			sprintf( fname, "BMAP_%u.pbm", vBlockID );
			thePicture.writebitmaptopbm( fname );
			sprintf( fname, "BMAP_%u_mask.pbm", vBlockID );
			thePicture.writemasktopbm( fname );
			delete [] pictureData;
		}
		else
		{
			printf( "'%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			theFile.ignore( vBlockSize -12 );	// Skip rest of block data.
		}
	}
	
	return true;
}
