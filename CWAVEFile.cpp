//
//  CWAVEFile.cpp
//  stackimport
//
//  Created by Uli Kusterer on 2013-12-30.
//
//

#include "CWAVEFile.h"
#include "EndianStuff.h"


CWAVEFile::CWAVEFile()
{
	mNumChannels = 1;
	mBitsPerSample = 16;
	mSampleRate = 44100;
	mSoundData = NULL;
	mSoundDataLen = 0;
}


void	CWAVEFile::WriteToFile( FILE* inFile )
{
	// Root block with file size -8:
	fwrite( "RIFF", 4, 1, inFile );
	uint32_t	fileSize = 0;
	fwrite( &fileSize, sizeof(fileSize), 1, inFile );	// Fill this out later.
	fwrite( "WAVE", 4, 1, inFile );
	fileSize += 4;
	
	// 'fmt ' block:
	fwrite( "fmt ", 4, 1, inFile );
	fileSize += 4;
	uint32_t	commonChunkSize = 16;
	
	fwrite( &commonChunkSize, sizeof(commonChunkSize), 1, inFile );
	fileSize += 4;
	
	uint16_t	audioFormat = 1;	// PCM.
	fwrite( &audioFormat, sizeof(audioFormat), 1, inFile );
	fwrite( &mNumChannels, sizeof(mNumChannels), 1, inFile );
	fwrite( &mSampleRate, sizeof(mSampleRate), 1, inFile );
	uint32_t	byteRate = mSampleRate * mNumChannels * (mBitsPerSample/8);
	fwrite( &byteRate, sizeof(byteRate), 1, inFile );
	uint16_t	blockAlign = mNumChannels * (mBitsPerSample/8);
	fwrite( &blockAlign, sizeof(blockAlign), 1, inFile );
	fwrite( &mBitsPerSample, sizeof(mBitsPerSample), 1, inFile );
	
	fileSize += commonChunkSize;
	
	// Now write actual sound data:
	fwrite( "data", 4, 1, inFile );
	fileSize += 4;
	uint32_t	dataChunkSize = mSoundDataLen;
	fwrite( &dataChunkSize, sizeof(dataChunkSize), 1, inFile );
	fileSize += 4;
	
	fwrite( mSoundData, mSoundDataLen, 1, inFile );			// Sound data.
	fileSize += mSoundDataLen;
	
	// Now write actual file size (not counting RIFF id and size field itself) into the right spot.
	fseek( inFile, 4, SEEK_SET );
	fwrite( &fileSize, sizeof(fileSize), 1, inFile );
}
