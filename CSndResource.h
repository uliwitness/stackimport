//
//  CSndResource.h
//  stackimport
//
//  Created by Uli Kusterer on 2013-12-30.
//
//

/*
	C++ class which you give the raw data of a 'snd ' resource and it parses
	the interesting bits out of it.
	
	Currently only supports format 2 sound resources (like HyperCard has them), but shouldn't be too hard to extend for other formats.
*/

#ifndef __stackimport__CSndResource__
#define __stackimport__CSndResource__

#include <cstddef>
#include <stdint.h>


// A CSndResource contains a list of these:
class CSoundCommand
{
public:
	CSoundCommand(uint16_t inCommandType, uint16_t inParam1, uint32_t inParam2, const char* inData, size_t inDataLen );
	
	uint16_t		commandType;
	uint16_t		param1;
	uint32_t		param2;
	
	size_t			GetSoundHeaderOffset();
	uint32_t		GetNumBytesInSample();
	uint32_t		GetSampleRate();
	uint32_t		GetLoopPointStart();
	uint32_t		GetLoopPointEnd();
	uint8_t			GetStandardSampleEncoding();
	uint8_t			GetBaseFrequency();
	const char*		GetSampleData();	// Length is GetNumBytesInSample().
	
protected:
	const char*		mResData;
	size_t			mResDataLen;
};


// The actual resource:
class CSndResource
{
public:
	CSndResource( const char* inData, size_t dataLen );
	
	uint16_t		GetFormat();	// 1 or 2.
	uint16_t		GetNumSoundCommands();
	
	CSoundCommand	GetSoundCommandAtIndex( size_t inIndex );

protected:
	const char*		mResData;
	size_t			mResDataLen;
};

#endif /* defined(__stackimport__CSndResource__) */
