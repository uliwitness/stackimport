/*
 *  CStackFile.h
 *  stackimport
 *
 *  Created by Mr. Z. on 10/06/06.
 *  Copyright 2006 Mr Z. All rights reserved.
 *
 */

#include <map>
#include <vector>
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
	
	virtual bool operator != ( const CStackBlockIdentifier& inOther ) const
	{
		if( strcmp( mType, inOther.mType ) == 0 )
		{
			if( mID == inOther.mID || mIsWildcard || inOther.mIsWildcard )
				return false;
		}
		
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


typedef std::map<CStackBlockIdentifier,CBuf> 	CBlockMap;
typedef std::map<int32_t,std::vector<int32_t> >	CBtnIDsPerBgMap;


class CStackFile
{
public:
	CStackFile();
	
	bool	LoadFile( const std::string& fpath );
	
	void	SetDumpRawBlockData( bool inDumpToFiles ) 			{ mDumpRawBlockData = inDumpToFiles; };
	void	SetStatusMessages( bool inPrintStatusMessages ) 	{ mStatusMessages = inPrintStatusMessages; };
	void	SetProgressMessages( bool inPrintProgressMessages ) { mProgressMessages = inPrintProgressMessages; };

protected:
	bool	LoadStackBlock( CBuf& blockData );
	bool	LoadListBlock( CBuf& blockData );
	bool	LoadPageTable( int32_t blockID, CBuf& blockData );
	bool	LoadFontTable( int32_t blockID, CBuf& blockData );
	bool	LoadStyleTable( int32_t blockID, CBuf& blockData );
	bool	LoadLayerBlock( const char* vBlockType, int32_t blockID, CBuf& blockData );	// Card or Bkgd.
	bool	LoadBackgroundBlock( int32_t blockID, CBuf& blockData );

protected:
	bool			mDumpRawBlockData;	// Create .data files with the contents of each block.
	bool			mStatusMessages;	// Output "Status: blah" messages to stdout.
	bool			mProgressMessages;	// Output "Progress: 1 of N" messages to stdout.
	FILE*			mXmlFile;			// FILE* for writing to output XML file (toc.xml)
	int32_t			mListBlockID;		// ID of the LIST block, read from STAK block.
	int32_t			mFontTableBlockID;	// ID of the FTBL block, read from STAK block.
	int32_t			mStyleTableBlockID;	// ID of the STBL block, read from STAK block.
	int32_t			mCardBlockSize;		// Size of the card entries in PAGE blocks, read from LIST block.
	CBlockMap		mBlockMap;			// Associative map of type/id -> block data mappings for random access to blocks when actually parsing their contents.
	int				mCurrentProgress;
	int				mMaxProgress;
	CBtnIDsPerBgMap	mButtonIDsPerBg;
};