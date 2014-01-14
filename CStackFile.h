/*
 *  CStackFile.h
 *  stackimport
 *
 *  Created by Mr. Z. on 10/06/06.
 *  Copyright 2006 Mr Z. All rights reserved.
 *
 */

// If you're not compiling for a platform where the Mac resource manager is
//	available, set the following to 0 to remove that code from compilation:
#define MAC_CODE		1

// If you're compiling for 64 bit, you don't have access to QuickTime, which
//	we use to create AIFF files from 'snd ' resources. So turn this off.
#define USE_QUICKTIME	(!__LP64__)


#include <map>
#include <vector>
#include <stdint.h>
#include "CBuf.h"

#if MAC_CODE
#include <Carbon/Carbon.h>
#if USE_QUICKTIME
#include <QuickTime/QuickTime.h>
#endif
#endif


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
typedef std::map<int16_t,std::string>			CFontTable;

class CStyleEntry
{
public:
	int16_t		mStyleID;
	int16_t		mFontID;
	std::string	mFontName;
	bool		mGroup;
	bool		mExtend;
	bool		mCondense;
	bool		mShadow;
	bool		mOutline;
	bool		mUnderline;
	bool		mItalic;
	bool		mBold;
	int16_t		mFontSize;
	
	CStyleEntry() : mStyleID(0), mFontID(0), mGroup(false), mExtend(false), 
		mCondense(false), mShadow(false), mOutline(false), mUnderline(false),
		mItalic(false), mBold(false), mFontSize(12)	{};
};


class CStackFile
{
public:
	CStackFile();
	
	bool	LoadFile( const std::string& fpath );
	
	void	SetDumpRawBlockData( bool inDumpToFiles ) 			{ mDumpRawBlockData = inDumpToFiles; };
	void	SetStatusMessages( bool inPrintStatusMessages ) 	{ mStatusMessages = inPrintStatusMessages; };
	void	SetProgressMessages( bool inPrintProgressMessages ) { mProgressMessages = inPrintProgressMessages; };

protected:
	bool	LoadStackBlock( int32_t stackID, CBuf& blockData );
	bool	LoadListBlock( CBuf& blockData );
	bool	LoadPageTable( int32_t blockID, CBuf& blockData );
	bool	LoadFontTable( int32_t blockID, CBuf& blockData );
	bool	LoadStyleTable( int32_t blockID, CBuf& blockData );
	bool	LoadLayerBlock( const char* vBlockType, int32_t blockID, CBuf& blockData, uint8_t inFlags );	// Card or Bkgd.
	
#if MAC_CODE
	bool	LoadBWIcons();
	bool	LoadPictures();
	bool	LoadCursors();
	bool	LoadSounds();
	bool	Load68000Resources();
	bool	LoadPowerPCResources();
#endif //MAC_CODE

protected:
	bool			mDumpRawBlockData;	// Create .data files with the contents of each block.
	bool			mStatusMessages;	// Output "Status: blah" messages to stdout.
	bool			mProgressMessages;	// Output "Progress: 1 of N" messages to stdout.
	FILE*			mXmlFile;			// FILE* for writing to output XML file (toc.xml)
	FILE*			mStackXmlFile;		// FILE* for writing to output XML file (stack_1.xml)
	int32_t			mListBlockID;		// ID of the LIST block, read from STAK block.
	int32_t			mFontTableBlockID;	// ID of the FTBL block, read from STAK block.
	int32_t			mStyleTableBlockID;	// ID of the STBL block, read from STAK block.
	int32_t			mCardBlockSize;		// Size of the card entries in PAGE blocks, read from LIST block.
	CBlockMap		mBlockMap;			// Associative map of type/id -> block data mappings for random access to blocks when actually parsing their contents.
	int				mCurrentProgress;	// Current value for progress output.
	int				mMaxProgress;		// Maximum value for progress output.
	CBtnIDsPerBgMap	mButtonIDsPerBg;	// Table that holds the IDs of all BG buttons on each background. Used to detect what card-level button contents entries are actually sharedHighlight entries for a bg button.
	std::string		mBasePath;			// Path to package folder, in which we'll put all the XML files and graphics'n stuff.
	std::string		mFileName;			// Name of the original stack file, w/o the path.
	std::string		mStyleSheetName;	// Name of CSS file containing our styles table.
	CFontTable		mFontTable;			// Actual, parsed font ID -> name mappings from FTBL block.
#if MAC_CODE
	SInt16			mResRefNum;
#endif
	std::map<int16_t,CStyleEntry>	mStyles;
};