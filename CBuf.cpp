/*
 *  CBuf.cpp
 *  stackimport
 *
 *  Created by Mr. Z. on 03/31/10.
 *  Copyright 2010 Mr Z. All rights reserved.
 *
 */

#include <cstring>
#include "CBuf.h"
#include "byteutils.h"
#include "assert.h"


CBuf::CBuf( size_t inSize )
	: mShared(NULL)
{
	alloc_buffer( inSize );
}


CBuf::CBuf( const CBuf& inTemplate, size_t startOffs, size_t amount )
	: mShared(NULL)
{
	if( amount == SIZE_MAX )
		amount = inTemplate.size() -startOffs;
	
	if( startOffs == 0 && amount == inTemplate.size() )
	{
		mShared = inTemplate.mShared;
		mShared->mRefCount++;
	}
	else
	{
		alloc_buffer( amount );
		::memcpy( mShared->mBuffer, inTemplate.buf(startOffs, amount), amount );
	}
}


CBuf::~CBuf()
{
	release_buffer();
}


void	CBuf::alloc_buffer( size_t amount )
{
	mShared = (shared_buffer*) calloc( 1, sizeof(shared_buffer) );
	if( amount > 0 )
		mShared->mBuffer = new char[amount];
	mShared->mSize = amount;
	mShared->mRefCount = 1;
}


void	CBuf::release_buffer()
{
	if( mShared->mRefCount > 1 )
		mShared->mRefCount--;
	else
	{
		if( mShared->mBuffer )
			delete [] mShared->mBuffer;
		mShared->mBuffer = NULL;
		mShared->mSize = 0;
		free( mShared );
	}
	mShared = NULL;
}


void	CBuf::make_buffer_exclusive()
{
	if( mShared->mRefCount == 1 )
		return;	// Already are exclusive owner.
	
	shared_buffer*	oldBuffer = mShared;
	alloc_buffer( oldBuffer->mSize );
	::memmove( mShared->mBuffer, oldBuffer->mBuffer, oldBuffer->mSize );
	oldBuffer->mRefCount --;
}


void	CBuf::resize( size_t inSize )
{
	if( mShared->mRefCount == 1 )
	{
		if( mShared->mBuffer )
			delete [] mShared->mBuffer;
		mShared->mBuffer = NULL;
		if( inSize > 0 )
			mShared->mBuffer = new char[inSize];
		mShared->mSize = inSize;
	}
	else
	{
		shared_buffer*	oldBuffer = mShared;
		alloc_buffer( inSize );
		oldBuffer->mRefCount--;
	}
}

void	CBuf::memcpy( size_t toOffs, const char* fromPtr, size_t fromOffs, size_t amount )
{
	make_buffer_exclusive();
	
	char*		thePtr = mShared->mBuffer;
	if( (toOffs +amount) > mShared->mSize )
		return;
	
	::memcpy( thePtr + toOffs, fromPtr +fromOffs, amount );
}


void	CBuf::memcpy( size_t toOffs, const CBuf& fromPtr, size_t fromOffs, size_t amount )
{
	if( amount == SIZE_MAX )
		amount = fromPtr.size() -fromOffs;
	memcpy( toOffs, fromPtr.buf(fromOffs,amount), 0, amount );
}


const char CBuf::operator [] ( int idx ) const
{
	if( idx >= mShared->mSize )
		return 0;
	return mShared->mBuffer[idx];
}


char& CBuf::operator [] ( int idx )
{
	static char		dummy[2048] = {0};
	if( idx >= mShared->mSize )
		return dummy[0];
	
	make_buffer_exclusive();
	return mShared->mBuffer[idx];
}


char*	CBuf::buf( size_t offs, size_t amount )
{
	if( amount == SIZE_MAX )
		amount = mShared->mSize -offs;
	assert( mShared->mBuffer != NULL );
	static char		dummy[2048] = {0};
	if( (amount +offs) > mShared->mSize )
		return dummy;
	
	make_buffer_exclusive();
	return mShared->mBuffer + offs;
}


const char*	CBuf::buf( size_t offs, size_t amount ) const
{
	static char		dummy[2048] = {0};
	if( amount == SIZE_MAX )
		amount = mShared->mSize -offs;
	assert( mShared->mBuffer != NULL );
	if( (amount +offs) > mShared->mSize )
		return dummy;
	
	return mShared->mBuffer + offs;
}


void	CBuf::xornstr( size_t dstOffs, char * src, size_t srcOffs, size_t amount )
{
	assert( mShared->mBuffer != NULL);
	assert( (amount +dstOffs) <= mShared->mSize );
	make_buffer_exclusive();
	::xornstr( mShared->mBuffer +dstOffs, src +srcOffs, amount );
}


void	CBuf::xornstr( size_t dstOffs, const CBuf& src, size_t srcOffs, size_t amount )
{
	assert( mShared->mBuffer != NULL );
	assert( (amount +dstOffs) <= mShared->mSize );
	make_buffer_exclusive();
	::xornstr( mShared->mBuffer +dstOffs, src.buf(srcOffs,amount), amount );
}


void	CBuf::shiftnstr( size_t dstOffs, int amount, int shiftAmount )
{
	assert( mShared->mBuffer != NULL );
	assert( (dstOffs +amount) <= mShared->mSize );
	make_buffer_exclusive();
	::shiftnstr( mShared->mBuffer +dstOffs, amount, shiftAmount );
}


void	CBuf::tofile( const std::string& fpath )
{
	FILE*	theFile = fopen( fpath.c_str(), "w" );
	fwrite( mShared->mBuffer, 1, mShared->mSize, theFile );
	fclose( theFile );
}


CBuf&	CBuf::operator = ( const CBuf& inTemplate )
{
	if( mShared != inTemplate.mShared )
	{
		release_buffer();
		mShared = inTemplate.mShared;
		mShared->mRefCount++;
	}
	
	return *this;
}

