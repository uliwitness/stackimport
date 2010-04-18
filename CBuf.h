/*
 *  CBuf.h
 *  stackimport
 *
 *  Created by Mr. Z. on 03/31/10.
 *  Copyright 2010 Mr Z. All rights reserved.
 *
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string>


struct shared_buffer
{
	char*		mBuffer;
	size_t		mSize;
	int			mRefCount;
};


class CBuf
{
public:
	explicit CBuf( size_t inSize = 0 );
	CBuf( const CBuf& inTemplate, size_t startOffs = 0, size_t amount = SIZE_MAX ); 
	~CBuf();
	
	void			resize( size_t inSize );
	
	void			memcpy( size_t toOffs, const char* fromPtr, size_t fromOffs, size_t amount );
	void			memcpy( size_t toOffs, const CBuf& fromPtr, size_t fromOffs = 0, size_t amount = SIZE_MAX );
	
	const char		operator [] ( int idx ) const;
	char&			operator [] ( int idx );
	
	char*			buf( size_t offs = 0, size_t amount = SIZE_MAX );
	const char*		buf( size_t offs = 0, size_t amount = SIZE_MAX ) const;
	
	void			xornstr( size_t dstOffs, char * src, size_t srcOffs, size_t amount );
	void			xornstr( size_t dstOffs, const CBuf& src, size_t srcOffs, size_t amount );

	void			shiftnstr( size_t dstOffs, int amount, int shiftAmount );
	
	size_t			size() const					{ return mShared->mSize; };
	
	int16_t			int16at( size_t offs ) const	{ int16_t* theBuf = (int16_t*) buf(offs,sizeof(int16_t)); return *theBuf; };
	int32_t			int32at( size_t offs ) const	{ int32_t* theBuf = (int32_t*) buf(offs,sizeof(int32_t)); return *theBuf; };

	uint16_t		uint16at( size_t offs ) const	{ uint16_t* theBuf = (uint16_t*) buf(offs,sizeof(uint16_t)); return *theBuf; };
	uint32_t		uint32at( size_t offs ) const	{ uint32_t* theBuf = (uint32_t*) buf(offs,sizeof(uint32_t)); return *theBuf; };
	
	bool			hasdata( size_t offs, size_t amount )	{ return (mShared->mBuffer != NULL) && (amount +offs) <= mShared->mSize; };
	
	void			tofile( const std::string& fpath );
	
	void			debug_print()		{ if( !mShared ) printf( "NULL\n" ); else { printf("CBuf %p { size = %zd, refCount = %d, \"%-*s\" }\n", this, mShared->mSize, mShared->mRefCount, (int)mShared->mSize, mShared->mBuffer ); } };
	
	virtual CBuf&	operator = ( const CBuf& inTemplate );

protected:
	void			alloc_buffer( size_t amount );
	void			release_buffer();
	void			make_buffer_exclusive();

protected:
	shared_buffer*	mShared;
};
