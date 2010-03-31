/*
 *  CStackFile.h
 *  stackimport
 *
 *  Created by Uli Kusterer on 06.10.06.
 *  Copyright 2006 M. Uli Kusterer. All rights reserved.
 *
 */

#include <deque>
#include <map>
#include <stdint.h>
#include "CBuf.h"


class CStackBlockIdentifier
{
public:
	char		mType[5];
	int32_t		mID;
	bool		mIsWildcard;

	CStackBlockIdentifier( const char* inType, int32_t inID ) : mID(inID), mIsWildcard(false) 	{ size_t len = strlen(inType); if(len > 4) len = 4; memmove( mType, inType, len ); mType[len] = 0; };
	CStackBlockIdentifier( const char* inType ) : mID(0), mIsWildcard(true) 					{ size_t len = strlen(inType); if(len > 4) len = 4; memmove( mType, inType, len ); mType[len] = 0; };
	virtual ~CStackBlockIdentifier()		{};
	
	virtual bool operator == ( const CStackBlockIdentifier& inOther ) const
	{
		if( strcmp( mType, inOther.mType ) != 0 )
			return false;
		if( mID != inOther.mID && !mIsWildcard && !inOther.mIsWildcard )
			return false;
		
		return true;
	};
	
	virtual bool operator > ( const CStackBlockIdentifier& inOther ) const
	{
		int		cmpResult = strcmp( mType, inOther.mType );
		if( cmpResult < 0 )
			return false;
		if( cmpResult > 0 )
			return true;
		
		if( mID <= inOther.mID || mIsWildcard || inOther.mIsWildcard )
			return false;
		
		return true;
	};

	virtual bool operator < ( const CStackBlockIdentifier& inOther ) const
	{
		int		cmpResult = strcmp( mType, inOther.mType );
		if( cmpResult > 0 )
			return false;
		if( cmpResult < 0 )
			return true;
		if( mID >= inOther.mID || mIsWildcard || inOther.mIsWildcard )
			return false;
		
		return true;
	};
};


typedef std::map<CStackBlockIdentifier,CBuf> CBlockMap;


class CStackFile
{
public:
	CStackFile();
	
	bool	LoadFile( const std::string& fpath );
	
	void	SetDumpRawBlockData( bool inDumpToFiles ) 		{ mDumpRawBlockData = inDumpToFiles; };
	void	SetStatusMessages( bool inPrintStatusMessages ) { mStatusMessages = inPrintStatusMessages; };

protected:
	bool	LoadStackBlock( CBuf& blockData );
	bool	LoadListBlock( CBuf& blockData );
	bool	LoadPageTable( int32_t blockID, CBuf& blockData );
	bool	LoadFontTable( int32_t blockID, CBuf& blockData );
	bool	LoadStyleTable( int32_t blockID, CBuf& blockData );
	bool	LoadLayerBlock( const char* vBlockType, int32_t blockID, CBuf& blockData );	// Card or Bkgd.

protected:
	bool		mDumpRawBlockData;
	bool		mStatusMessages;
	FILE*		mXmlFile;
	int32_t		mListBlockID;
	int32_t		mCardBlockSize;
	CBlockMap	mBlockMap;
};