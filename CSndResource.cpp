//
//  CSndResource.cpp
//  stackimport
//
//  Created by Uli Kusterer on 2013-12-30.
//
//

#include "CSndResource.h"

// No-op for now, as the Resource Manager pre-swaps the 'snd ' resource for us:
//	But once we're using our own res-fork-reading code, map these to BIG_ENDIAN_xx.
#define BE16(n)		n
#define BE32(n)		n

CSoundCommand::CSoundCommand( uint16_t inCommandType, uint16_t inParam1, uint32_t inParam2, const char* inData, size_t inDataLen )
: commandType(inCommandType), param1(inParam1), param2(inParam2)
{
	mResData = inData;
	mResDataLen = inDataLen;
	
}
	
uint32_t	CSoundCommand::GetNumBytesInSample()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return 0;
	
	const char*		theData = mResData +param2 +4;	// Sound data, then skip pointer for use in RAM.
	return BE32( (*(uint32_t*)theData) );
}


uint32_t	CSoundCommand::GetSampleRate()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return 0;
	
	const char*		theData = mResData +param2 +4 +4;	// Sound data, then skip pointer for use in RAM, then bytes in sample.
	return BE32( (*(uint32_t*)theData) );
}


uint32_t	CSoundCommand::GetLoopPointStart()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return 0;
	
	const char*		theData = mResData +param2 +4 +4 +4;	// Sound data, then skip pointer for use in RAM, bytes in sample, sample rate.
	return BE32( (*(uint32_t*)theData) );
}


uint32_t	CSoundCommand::GetLoopPointEnd()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return 0;
	
	const char*		theData = mResData +param2 +4 +4 +4 +4;	// Sound data, then skip pointer for use in RAM, bytes in sample, sample rate, loop point start.
	return BE32( (*(uint32_t*)theData) );
}

uint8_t	CSoundCommand::GetStandardSampleEncoding()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return 0;
	
	const char*		theData = mResData +param2 +4 +4 +4 +4 +4;	// Sound data, then skip pointer for use in RAM, bytes in sample, sample rate, loop point start/end.
	return BE32( (*(uint8_t*)theData) );
}


uint8_t	CSoundCommand::GetBaseFrequency()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return 0;
	
	const char*		theData = mResData +param2 +4 +4 +4 +4 +4 +1;	// Sound data, then skip pointer for use in RAM, bytes in sample, sample rate, loop point start/end, standard sample encoding.
	return BE32( (*(uint8_t*)theData) );
}


const char*	CSoundCommand::GetSampleData()
{
	if( commandType != 0x8050 && commandType != 0x8051 )	// Neither soundCmd nor bufferCmd?
		return NULL;
	
	const char*		theData = mResData +param2 +4 +4 +4 +4 +4 +1 +1;	// Sound data, then skip pointer for use in RAM, bytes in sample, sample rate, loop point start/end, standard sample encoding, base frequency.
	return theData;
}


CSndResource::CSndResource( const char* inData, size_t dataLen )
 : mResData(inData), mResDataLen(dataLen)
{
	
}
	
uint16_t	CSndResource::GetFormat()
{
	return BE16((*(uint16_t*)mResData));
}


uint16_t	CSndResource::GetNumSoundCommands()
{
	switch( GetFormat() )
	{
		case 2:
		{
			const char*	theData = mResData +2 +2;	// Skip type & refCount.
			return BE16((*(uint16_t*)theData));
			break;
		}
	}
	
	return 0;
}
	
CSoundCommand	CSndResource::GetSoundCommandAtIndex( size_t inIndex )
{
	const char*	theData = mResData +2 +2 +2;	// Skip type, refCount & number of commands.
	theData += inIndex * 8;
	
	CSoundCommand	theCmd( BE16((*(uint16_t*)theData)), BE16((*(uint16_t*)(theData +2))), BE16((*(uint32_t*)(theData +2 +2))), mResData, mResDataLen );
	
	return theCmd;
}

