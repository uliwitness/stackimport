//
//  CAIFFFile.cpp
//  stackimport
//
//  Created by Uli Kusterer on 2013-12-30.
//
//

#include "CAIFFFile.h"
#include "EndianStuff.h"


CAIFFFile::CAIFFFile()
{
	mNumChannels = 1;
}


void	CAIFFFile::WriteToFile( FILE* inFile )
{
	// Root block with file size -8:
	fwrite( "FORM", 4, 1, inFile );
	uint32_t	fileSize = 0;
	fwrite( &fileSize, sizeof(fileSize), 1, inFile );	// Fill this out later.
	fwrite( "AIFF", 4, 1, inFile );
	fileSize += 4;
	
	// COMM block:
	fwrite( "COMM", 4, 1, inFile );
	fileSize += 4;
	uint32_t	commonChunkSize = BIG_ENDIAN_32(18);
	
	fwrite( &commonChunkSize, sizeof(commonChunkSize), 1, inFile );
	fileSize += 4;
	
	fwrite( &mNumChannels, sizeof(mNumChannels), 1, inFile );
	fwrite( &mNumSampleFrames, sizeof(mNumSampleFrames), 1, inFile );
	fwrite( &mSampleSize, sizeof(mSampleSize), 1, inFile );
	fwrite( mSampleRate, sizeof(mSampleRate), 1, inFile );
	
	fileSize += commonChunkSize;
	
	// Now write actual sound data:
	fwrite( "SSND", 4, 1, inFile );
	fileSize += 4;
	uint32_t	dataChunkSize = BIG_ENDIAN_32(4 +4 +mSoundDataLen);
	
	fwrite( &dataChunkSize, sizeof(dataChunkSize), 1, inFile );
	fileSize += 4;
	
	uint32_t	dataOffset = 0;
	fwrite( &dataOffset, sizeof(dataOffset), 1, inFile );	// offset. (optional)
	fileSize += 4;
	fwrite( &dataOffset, sizeof(dataOffset), 1, inFile );	// alignment. (optional)
	fileSize += 4;
	fwrite( mSoundData, mSoundDataLen, 1, inFile );			// Sound data.
	fileSize += 4;
	if( (mSoundDataLen %2) == 1 )	// Odd length?
	{
		uint8_t		padding = 0;
		fwrite( &padding, 1, 1, inFile );
		// This padding doesn't count into the chunk length, but in the file size.
		fileSize += 1;
	}
	
	// Now write actual file size (not counting FORM id and size field ittself) into the right spot.
	fseek( inFile, 4, SEEK_SET );
	fwrite( &fileSize, sizeof(fileSize), 1, inFile );
}
