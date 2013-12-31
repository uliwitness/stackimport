//
//  CWAVEFile.h
//  stackimport
//
//  Created by Uli Kusterer on 2013-12-30.
//
//

#ifndef __stackimport__CWAVEFile__
#define __stackimport__CWAVEFile__

#include <cstddef>
#include <cstdint>
#include <cstdio>


class CWAVEFile
{
public:
	CWAVEFile();
	
	void		WriteToFile( FILE* inFile );

	uint16_t	mNumChannels;
	uint16_t	mBitsPerSample;
	uint32_t	mSampleRate;
	const char*	mSoundData;
	uint32_t	mSoundDataLen;
};

#endif /* defined(__stackimport__CWAVEFile__) */
