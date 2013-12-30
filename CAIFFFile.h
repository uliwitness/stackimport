//
//  CAIFFFile.h
//  stackimport
//
//  Created by Uli Kusterer on 2013-12-30.
//
//

#ifndef __stackimport__CAIFFFile__
#define __stackimport__CAIFFFile__

#include <cstddef>
#include <cstdint>
#include <cstdio>


class CAIFFFile
{
public:
	CAIFFFile();
	
	void		WriteToFile( FILE* inFile );

	uint16_t	mNumChannels;
	uint32_t	mNumSampleFrames;
	uint16_t	mSampleSize;
	char		mSampleRate;
	const char*	mSoundData;
	uint32_t	mSoundDataLen;
};

#endif /* defined(__stackimport__CAIFFFile__) */
