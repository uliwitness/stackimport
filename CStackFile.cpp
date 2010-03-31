/*
 *  CStackFile.cpp
 *  stackimport
 *
 *  Created by Uli Kusterer on 06.10.06.
 *  Copyright 2006 M. Uli Kusterer. All rights reserved.
 *
 */


#define MAC_CODE		1


#include "CStackFile.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "picture.h"
#include "woba.h"
#include "CBuf.h"
#include "EndianStuff.h"
#if MAC_CODE
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#endif


// Table of C-strings for converting the non-ASCII MacRoman characters (above 127)
//	into the requisite UTF8 byte sequences:
unsigned char	sMacRomanToUTF8Table[128][5] =
{
	{ 0xc3, 0x84, 0x00, 0x00 }, { 0xc3, 0x85, 0x00, 0x00 }, { 0xc3, 0x87, 0x00, 0x00 }, { 0xc3, 0x89, 0x00, 0x00 },
	{ 0xc3, 0x91, 0x00, 0x00 }, { 0xc3, 0x96, 0x00, 0x00 }, { 0xc3, 0x9c, 0x00, 0x00 }, { 0xc3, 0xa1, 0x00, 0x00 },
	{ 0xc3, 0xa0, 0x00, 0x00 }, { 0xc3, 0xa2, 0x00, 0x00 }, { 0xc3, 0xa4, 0x00, 0x00 }, { 0xc3, 0xa3, 0x00, 0x00 },
	{ 0xc3, 0xa5, 0x00, 0x00 }, { 0xc3, 0xa7, 0x00, 0x00 }, { 0xc3, 0xa9, 0x00, 0x00 }, { 0xc3, 0xa8, 0x00, 0x00 },
	{ 0xc3, 0xaa, 0x00, 0x00 }, { 0xc3, 0xab, 0x00, 0x00 }, { 0xc3, 0xad, 0x00, 0x00 }, { 0xc3, 0xac, 0x00, 0x00 },
	{ 0xc3, 0xae, 0x00, 0x00 }, { 0xc3, 0xaf, 0x00, 0x00 }, { 0xc3, 0xb1, 0x00, 0x00 }, { 0xc3, 0xb3, 0x00, 0x00 },
	{ 0xc3, 0xb2, 0x00, 0x00 }, { 0xc3, 0xb4, 0x00, 0x00 }, { 0xc3, 0xb6, 0x00, 0x00 }, { 0xc3, 0xb5, 0x00, 0x00 },
	{ 0xc3, 0xba, 0x00, 0x00 }, { 0xc3, 0xb9, 0x00, 0x00 }, { 0xc3, 0xbb, 0x00, 0x00 }, { 0xc3, 0xbc, 0x00, 0x00 },
	{ 0xe2, 0x80, 0xa0, 0x00 }, { 0xc2, 0xb0, 0x00, 0x00 }, { 0xc2, 0xa2, 0x00, 0x00 }, { 0xc2, 0xa3, 0x00, 0x00 },
	{ 0xc2, 0xa7, 0x00, 0x00 }, { 0xe2, 0x80, 0xa2, 0x00 }, { 0xc2, 0xb6, 0x00, 0x00 }, { 0xc3, 0x9f, 0x00, 0x00 },
	{ 0xc2, 0xae, 0x00, 0x00 }, { 0xc2, 0xa9, 0x00, 0x00 }, { 0xe2, 0x84, 0xa2, 0x00 }, { 0xc2, 0xb4, 0x00, 0x00 },
	{ 0xc2, 0xa8, 0x00, 0x00 }, { 0xe2, 0x89, 0xa0, 0x00 }, { 0xc3, 0x86, 0x00, 0x00 }, { 0xc3, 0x98, 0x00, 0x00 },
	{ 0xe2, 0x88, 0x9e, 0x00 }, { 0xc2, 0xb1, 0x00, 0x00 }, { 0xe2, 0x89, 0xa4, 0x00 }, { 0xe2, 0x89, 0xa5, 0x00 },
	{ 0xc2, 0xa5, 0x00, 0x00 }, { 0xc2, 0xb5, 0x00, 0x00 }, { 0xe2, 0x88, 0x82, 0x00 }, { 0xe2, 0x88, 0x91, 0x00 },
	{ 0xe2, 0x88, 0x8f, 0x00 }, { 0xcf, 0x80, 0x00, 0x00 }, { 0xe2, 0x88, 0xab, 0x00 }, { 0xc2, 0xaa, 0x00, 0x00 },
	{ 0xc2, 0xba, 0x00, 0x00 }, { 0xce, 0xa9, 0x00, 0x00 }, { 0xc3, 0xa6, 0x00, 0x00 }, { 0xc3, 0xb8, 0x00, 0x00 },
	{ 0xc2, 0xbf, 0x00, 0x00 }, { 0xc2, 0xa1, 0x00, 0x00 }, { 0xc2, 0xac, 0x00, 0x00 }, { 0xe2, 0x88, 0x9a, 0x00 },
	{ 0xc6, 0x92, 0x00, 0x00 }, { 0xe2, 0x89, 0x88, 0x00 }, { 0xe2, 0x88, 0x86, 0x00 }, { 0xc2, 0xab, 0x00, 0x00 },
	{ 0xc2, 0xbb, 0x00, 0x00 }, { 0xe2, 0x80, 0xa6, 0x00 }, { 0xc2, 0xa0, 0x00, 0x00 }, { 0xc3, 0x80, 0x00, 0x00 },
	{ 0xc3, 0x83, 0x00, 0x00 }, { 0xc3, 0x95, 0x00, 0x00 }, { 0xc5, 0x92, 0x00, 0x00 }, { 0xc5, 0x93, 0x00, 0x00 },
	{ 0xe2, 0x80, 0x93, 0x00 }, { 0xe2, 0x80, 0x94, 0x00 }, { 0xe2, 0x80, 0x9c, 0x00 }, { 0xe2, 0x80, 0x9d, 0x00 },
	{ 0xe2, 0x80, 0x98, 0x00 }, { 0xe2, 0x80, 0x99, 0x00 }, { 0xc3, 0xb7, 0x00, 0x00 }, { 0xe2, 0x97, 0x8a, 0x00 },
	{ 0xc3, 0xbf, 0x00, 0x00 }, { 0xc5, 0xb8, 0x00, 0x00 }, { 0xe2, 0x81, 0x84, 0x00 }, { 0xe2, 0x82, 0xac, 0x00 },
	{ 0xe2, 0x80, 0xb9, 0x00 }, { 0xe2, 0x80, 0xba, 0x00 }, { 0xef, 0xac, 0x81, 0x00 }, { 0xef, 0xac, 0x82, 0x00 },
	{ 0xe2, 0x80, 0xa1, 0x00 }, { 0xc2, 0xb7, 0x00, 0x00 }, { 0xe2, 0x80, 0x9a, 0x00 }, { 0xe2, 0x80, 0x9e, 0x00 },
	{ 0xe2, 0x80, 0xb0, 0x00 }, { 0xc3, 0x82, 0x00, 0x00 }, { 0xc3, 0x8a, 0x00, 0x00 }, { 0xc3, 0x81, 0x00, 0x00 },
	{ 0xc3, 0x8b, 0x00, 0x00 }, { 0xc3, 0x88, 0x00, 0x00 }, { 0xc3, 0x8d, 0x00, 0x00 }, { 0xc3, 0x8e, 0x00, 0x00 },
	{ 0xc3, 0x8f, 0x00, 0x00 }, { 0xc3, 0x8c, 0x00, 0x00 }, { 0xc3, 0x93, 0x00, 0x00 }, { 0xc3, 0x94, 0x00, 0x00 },
	{ 0xef, 0xa3, 0xbf, 0x00 }, { 0xc3, 0x92, 0x00, 0x00 }, { 0xc3, 0x9a, 0x00, 0x00 }, { 0xc3, 0x9b, 0x00, 0x00 },
	{ 0xc3, 0x99, 0x00, 0x00 }, { 0xc4, 0xb1, 0x00, 0x00 }, { 0xcb, 0x86, 0x00, 0x00 }, { 0xcb, 0x9c, 0x00, 0x00 },
	{ 0xc2, 0xaf, 0x00, 0x00 }, { 0xcb, 0x98, 0x00, 0x00 }, { 0xcb, 0x99, 0x00, 0x00 }, { 0xcb, 0x9a, 0x00, 0x00 },
	{ 0xc2, 0xb8, 0x00, 0x00 }, { 0xcb, 0x9d, 0x00, 0x00 }, { 0xcb, 0x9b, 0x00, 0x00 }, { 0xcb, 0x87, 0x00, 0x00 }
};


const unsigned char*	UniCharFromMacRoman( unsigned char c )
{
	if( c >= 128 )
		return sMacRomanToUTF8Table[ c -128 ];
	else if( c == 0x11 )
	{
		static unsigned char	commandKey[4] = { 0xe2, 0x8c, 0x98, 0 };	// Unicode 0x2318
		return commandKey;
	}
	else
	{
		static unsigned char	asciiStr[2] = { 0, 0 };
		asciiStr[0] = c;
		return asciiStr;
	}
}


void	NumVersionToStr( char numVersion[4], char outStr[16] )
{
	char	theCh = 'v';
	
	switch( numVersion[2] )
	{
		case 0x20:
			theCh = 'd';
			break;
		case 0x40:
			theCh = 'a';
			break;
		case 0x60:
			theCh = 'b';
			break;
		case 0x80:
			theCh = 'v';
			break;
	}
	
	if( numVersion[3] == 0 )
		snprintf( outStr, 16, "%x.%x.%x", numVersion[0], (numVersion[1] >> 4), (numVersion[1] & 0x0F) );
	else
		snprintf( outStr, 16, "%x.%x.%x%c%d", numVersion[0], (numVersion[1] >> 4), (numVersion[1] & 0x0F), theCh, numVersion[3] );
}



CStackFile::CStackFile()
	: mDumpRawBlockData(false), mStatusMessages(true), mXmlFile(NULL), mListBlockID(-1),
	mCardBlockSize(-1)
{
	
}


bool	CStackFile::LoadStackBlock( CBuf& blockData )
{
	if( mStatusMessages )
		fprintf( stdout, "Status: Processing 'STAK' #-1 (%lu bytes)\n", blockData.size() );
	fprintf( mXmlFile, "\t<stack>\n" );
	
	if( mDumpRawBlockData )
	{
		char sfn[256] = { 0 };
		snprintf( sfn, sizeof(sfn), "STAK_-1.data" );
		blockData.tofile( sfn );
	}
	
	int32_t	numberOfCards = BIG_ENDIAN_32(blockData.int32at( 32 ));
	fprintf( mXmlFile, "\t\t<cardCount>%d</cardCount>\n", numberOfCards );
	int32_t	cardID = BIG_ENDIAN_32(blockData.int32at( 36 ));
	fprintf( mXmlFile, "\t\t<cardID>%d</cardID>\n", cardID );
	mListBlockID = BIG_ENDIAN_32(blockData.int32at( 40 ));
	fprintf( mXmlFile, "\t\t<listID>%d</listID>\n", mListBlockID );
	int16_t	userLevel = BIG_ENDIAN_16(blockData.int16at( 60 ));
	fprintf( mXmlFile, "\t\t<userLevel>%d</userLevel>\n", userLevel );
	int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 64 ));
	fprintf( mXmlFile, "\t\t<cantModify> %s </cantModify>\n", (flags & (1 << 15)) ? "<true />" : "<false />" );
	fprintf( mXmlFile, "\t\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
	fprintf( mXmlFile, "\t\t<privateAccess> %s </privateAccess>\n", (flags & (1 << 13)) ? "<true />" : "<false />" );
	fprintf( mXmlFile, "\t\t<cantAbort> %s </cantAbort>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
	fprintf( mXmlFile, "\t\t<cantPeek> %s </cantPeek>\n", (flags & (1 << 10)) ? "<true />" : "<false />" );
	char		versStr[16] = { 0 };
	int32_t	version0 = blockData.int32at( 84 );
	NumVersionToStr( (char*) &version0, versStr );
	fprintf( mXmlFile, "\t\t<createdByVersion>%s</createdByVersion>\n", versStr );
	int32_t	version1 = blockData.int32at( 88 );
	NumVersionToStr( (char*) &version1, versStr );
	fprintf( mXmlFile, "\t\t<lastCompactedVersion>%s</lastCompactedVersion>\n", versStr );
	int32_t	version2 = blockData.int32at( 92 );
	NumVersionToStr( (char*) &version2, versStr );
	fprintf( mXmlFile, "\t\t<lastEditedVersion>%s</lastEditedVersion>\n", versStr );
	int32_t	version3 = blockData.int32at( 96 );
	NumVersionToStr( (char*) &version3, versStr );
	fprintf( mXmlFile, "\t\t<firstEditedVersion>%s</firstEditedVersion>\n", versStr );
	int16_t	height = BIG_ENDIAN_16(blockData.int16at( 428 ));
	if( height == 0 )
		height = 342;
	int16_t	width = BIG_ENDIAN_16(blockData.int16at( 430 ));
	if( width == 0 )
		width = 512;
	fprintf( mXmlFile, "\t\t<cardSize>\n\t\t\t<width>%d</width>\n\t\t\t<height>%d</height>\n\t\t</cardSize>\n", width, height );

	fprintf( mXmlFile, "\t\t<patterns>\n" );
	char			pattern[8] = { 0 };
	int				offs = 692;
	for( int n = 0; n < 40; n++ )
	{
		memmove( pattern, blockData.buf( offs, 8 ), 8 );
		char		fname[256] = { 0 };
		sprintf( fname, "PAT_%u.pbm", n +1 );
		picture		thePicture( 8, 8, 1, false );
		thePicture.memcopyin( pattern, 0, 8 );
		thePicture.writebitmaptopbm( fname );
		offs += 8;
		fprintf(mXmlFile,"\t\t\t<pattern>PAT_%u.pbm</pattern>\n", n+1);
	}
	fprintf( mXmlFile, "\t\t</patterns>\n" );
	
	int x = 0, startOffs = 1524;
	fprintf( mXmlFile, "\t\t<script>" );
	for( x = startOffs; blockData[x] != 0; x++ )
	{
		char currCh = blockData[x];
		if( currCh == '<' )
			fprintf( mXmlFile, "&lt;" );
		else if( currCh == '>' )
			fprintf( mXmlFile, "&gt;" );
		else if( currCh == '&' )
			fprintf( mXmlFile, "&amp;" );
		else
			fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
	}
	fprintf( mXmlFile, "</script>\n" );
	fprintf( mXmlFile, "\t</stack>\n" );
	
	return true;
}


bool	CStackFile::LoadStyleTable( int32_t blockID, CBuf& blockData )
{
	int32_t	vBlockSize = blockData.size();
	if( mStatusMessages )
		fprintf( stdout, "Status: Processing 'STBL' #%d %X (%d bytes)\n", blockID, blockID, vBlockSize );
	fprintf( mXmlFile, "\t<!-- 'STBL' #%d (%d bytes) -->\n", blockID, vBlockSize );
	
	if( mDumpRawBlockData )
	{
		char sfn[256] = { 0 };
		snprintf( sfn, sizeof(sfn), "STBL_%d.data", blockID );
		blockData.tofile( sfn );
	}
	
	size_t		currOffs = 4;
	int32_t		numStyles = BIG_ENDIAN_32(blockData.int32at( currOffs ));
	currOffs += 4;
	
	for( int s = 0; s < numStyles; s++ )
	{
		currOffs += 16;
		
		fprintf( mXmlFile, "\t<styleentry>\n" );
		int16_t	fontID = BIG_ENDIAN_16(blockData.int16at( currOffs ));
		if( fontID != -1 )
			fprintf( mXmlFile, "\t\t<font>%d</font>\n", fontID );
		currOffs += 2;
		
		int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffs ));
		currOffs += 2;
		
		if( textStyleFlags == 0 )
			fprintf( mXmlFile, "\t\t<textStyle>plain</textStyle>\n" );
		else if( textStyleFlags != -1 )	// -1 means use field style.
		{
			if( textStyleFlags & (1 << 15) )
				fprintf( mXmlFile, "\t\t<textStyle>group</textStyle>\n" );
			if( textStyleFlags & (1 << 14) )
				fprintf( mXmlFile, "\t\t<textStyle>extend</textStyle>\n" );
			if( textStyleFlags & (1 << 13) )
				fprintf( mXmlFile, "\t\t<textStyle>condense</textStyle>\n" );
			if( textStyleFlags & (1 << 12) )
				fprintf( mXmlFile, "\t\t<textStyle>shadow</textStyle>\n" );
			if( textStyleFlags & (1 << 11) )
				fprintf( mXmlFile, "\t\t<textStyle>outline</textStyle>\n" );
			if( textStyleFlags & (1 << 10) )
				fprintf( mXmlFile, "\t\t<textStyle>underline</textStyle>\n" );
			if( textStyleFlags & (1 << 9) )
				fprintf( mXmlFile, "\t\t<textStyle>italic</textStyle>\n" );
			if( textStyleFlags & (1 << 8) )
				fprintf( mXmlFile, "\t\t<textStyle>bold</textStyle>\n" );
		}
		int16_t	fontSize = BIG_ENDIAN_16(blockData.int16at( currOffs ));
		if( fontSize != -1 )
			fprintf( mXmlFile, "\t\t<size>%d</size>\n", fontSize );
		currOffs += 2;
		currOffs += 2;	// 2 more padding?
		
		fprintf( mXmlFile, "\t</styleentry>\n" );
	}
	
	return true;
}

bool	CStackFile::LoadFontTable( int32_t blockID, CBuf& blockData )
{
	uint32_t	vBlockSize = blockData.size();
	if( mStatusMessages )
		fprintf( stdout, "Status: Processing 'FTBL' #%d %X (%d bytes)\n", blockID, blockID, vBlockSize );
	fprintf( mXmlFile, "\t<!-- 'FTBL' #%d (%d bytes) -->\n", blockID, vBlockSize );
	int16_t	numFonts = BIG_ENDIAN_16(blockData.int16at( 6 ));
	size_t	currOffsIntoData = 8;
	currOffsIntoData += 4;	// Reserved?
	for( int n = 0; n < numFonts; n++ )
	{
		fprintf( mXmlFile, "\t<font>\n" );
		int16_t	fontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
		fprintf( mXmlFile, "\t\t<id>%d</id>\n", fontID );
		
		int x = 0, startOffs = currOffsIntoData +2;
		fprintf( mXmlFile, "\t\t<name>" );
		for( x = startOffs; blockData[x] != 0; x++ )
		{
			char currCh = blockData[x];
			if( currCh == '<' )
				fprintf( mXmlFile, "&lt;" );
			else if( currCh == '>' )
				fprintf( mXmlFile, "&gt;" );
			else if( currCh == '&' )
				fprintf( mXmlFile, "&amp;" );
			else
				fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
		}
		fprintf( mXmlFile, "</name>\n" );
	
		currOffsIntoData = x +1;
		currOffsIntoData += currOffsIntoData %2;	// Align on even byte.
		
		fprintf( mXmlFile, "\t</font>\n" );
	}
	
	return true;
}


bool	CStackFile::LoadLayerBlock( const char* vBlockType, int32_t blockID, CBuf& blockData )
{
	int32_t		vBlockSize = blockData.size();
	
	if( mStatusMessages )
		fprintf( stdout, "Status: Processing '%4s' #%d %X (%d bytes)\n", vBlockType, blockID, blockID, vBlockSize );
	fprintf( mXmlFile, "\t<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, blockID, vBlockSize );
	if( strcmp( "BKGD", vBlockType ) == 0 )
		fprintf( mXmlFile, "\t<background>\n" );
	else
		fprintf( mXmlFile, "\t<card>\n" );
	fprintf( mXmlFile, "\t\t<id>%d</id>\n", blockID );
	
	if( mDumpRawBlockData )
	{
		char sfn[256] = { 0 };
		snprintf( sfn, sizeof(sfn), "%s_%d.data", vBlockType, blockID );
		blockData.tofile( sfn );
	}

	int32_t	unknownFiller = BIG_ENDIAN_32(blockData.int32at( 0 ));
	fprintf( mXmlFile, "\t\t<filler1>%d</filler1>\n", unknownFiller );
	int32_t	bitmapID = BIG_ENDIAN_32(blockData.int32at( 4 ));
	if( bitmapID != 0 )
		fprintf( mXmlFile, "\t\t<bitmap>BMAP_%u.pbm</bitmap>\n", bitmapID );
	int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 8 ));
	fprintf( mXmlFile, "\t\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
	fprintf( mXmlFile, "\t\t<showPict> %s </showPict>\n", (flags & (1 << 13)) ? "<false />" : "<true />" );	// showPict is stored reversed.
	fprintf( mXmlFile, "\t\t<dontSearch> %s </dontSearch>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
	int32_t	owner = BIG_ENDIAN_32(blockData.int32at( 24 ));
	fprintf( mXmlFile, "\t\t<owner>%d</owner>\n", owner );
	int16_t	numParts = BIG_ENDIAN_16(blockData.int16at( 28 ));
	int16_t	numContents = BIG_ENDIAN_16(blockData.int16at( 36 ));
	size_t	currOffsIntoData = 42;
	for( int n = 0; n < numParts; n++ )
	{
		int16_t	partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
		
		fprintf( mXmlFile, "\t\t<part>\n" );
		int16_t	partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
		fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", partID );
		int16_t	flagsAndType = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +4 ));
		int16_t	partType = flagsAndType >> 8;
		bool	isButton = partType == 1;
		fprintf( mXmlFile, "\t\t\t<type>%s</type>\n", isButton ? "button" : "field" );
		fprintf( mXmlFile, "\t\t\t<visible> %s </visible>\n", (flagsAndType & (1 << 7)) ? "<false />" : "<true />" );	// Really "hidden" flag.
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<dontWrap> %s </dontWrap>\n", (flagsAndType & (1 << 5)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<reserved5> %d </reserved5>\n", (flagsAndType & (1 << 5)) >> 5 );
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<dontSearch> %s </dontSearch>\n", (flagsAndType & (1 << 4)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<reserved4> %d </reserved4>\n", (flagsAndType & (1 << 4)) >> 4 );
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<sharedText> %s </sharedText>\n", (flagsAndType & (1 << 3)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<reserved3> %d </reserved3>\n", (flagsAndType & (1 << 3)) >> 3 );
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<fixedLineHeight> %s </fixedLineHeight>\n", (flagsAndType & (1 << 2)) ? "<false />" : "<true />" );	// Really "use real line height" flag.
		else
			fprintf( mXmlFile, "\t\t\t<reserved2> %d </reserved2>\n", (flagsAndType & (1 << 2)) >> 2 );
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<autoTab> %s </autoTab>\n", (flagsAndType & (1 << 1)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<reserved1> %d </reserved1>\n", (flagsAndType & (1 << 1)) >> 1 );
		if( isButton )
			fprintf( mXmlFile, "\t\t\t<enabled> %s </enabled>\n", (flagsAndType & (1 << 0)) ? "<false />" : "<true />" );	// Same as lockText on fields. Really "disabled" flag.
		else
			fprintf( mXmlFile, "\t\t\t<lockText> %s </lockText>\n", (flagsAndType & (1 << 0)) ? "<true />" : "<false />" );	// Same as enabled on buttons.
		fprintf( mXmlFile, "\t\t\t<rect>\n\t\t\t\t<left>%d</left>\n\t\t\t\t<top>%d</top>\n\t\t\t\t<right>%d</right>\n\t\t\t\t<bottom>%d</bottom>\n\t\t\t</rect>\n",
					BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +8 )),
					BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +6 )),
					BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +12 )),
					BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +10 )) );
		int16_t	moreFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +14 ));
		int8_t	styleFromLowNibble = moreFlags & 15;
		const char*		styleStr = "unknown";
		if( isButton )
		{
			switch( styleFromLowNibble )
			{
				case 0:
					styleStr = "transparent";
					break;
				case 1:
					styleStr = "opaque";
					break;
				case 2:
					styleStr = "rectangle";
					break;
				case 3:
					styleStr = "roundrect";
					break;
				case 4:
					styleStr = "shadow";
					break;
				case 5:
					styleStr = "checkbox";
					break;
				case 6:
					styleStr = "radiobutton";
					break;
				case 8:
					styleStr = "standard";
					break;
				case 9:
					styleStr = "default";
					break;
				case 10:
					styleStr = "oval";
					break;
				case 11:
					styleStr = "popup";
					break;
			}
		}
		else
		{
			switch( styleFromLowNibble )
			{
				case 0:
					styleStr = "transparent";
					break;
				case 1:
					styleStr = "opaque";
					break;
				case 2:
					styleStr = "rectangle";
					break;
				case 4:
					styleStr = "shadow";
					break;
				case 7:
					styleStr = "scrolling";
					break;
			}
		}
		fprintf( mXmlFile, "\t\t\t<style>%s</style>\n", styleStr );
		moreFlags = moreFlags >> 8;
		int8_t	family = moreFlags & 15;
		if( isButton )
			fprintf( mXmlFile, "\t\t\t<showName> %s </showName>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<autoSelect> %s </autoSelect>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
		if( isButton )
			fprintf( mXmlFile, "\t\t\t<highlight> %s </highlight>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<showLines> %s </showLines>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<wideMargins> %s </wideMargins>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
		else
			fprintf( mXmlFile, "\t\t\t<autoHighlight> %s </autoHighlight>\n", (moreFlags & (1 << 5) || family != 0) ? "<true />" : "<false />" );
		if( isButton )
			fprintf( mXmlFile, "\t\t\t<sharedHighlight> %s </sharedHighlight>\n", (moreFlags & (1 << 4)) ? "<false />" : "<true />" );
		else
			fprintf( mXmlFile, "\t\t\t<multipleLines> %s </multipleLines>\n", (moreFlags & (1 << 4)) ? "<true />" : "<false />" );
		if( isButton )
			fprintf( mXmlFile, "\t\t\t<family>%d</family>\n", family );
		else
			fprintf( mXmlFile, "\t\t\t<reservedFamily> %d </reservedFamily>\n", family );
		
		// titleWidth & iconID are list fields' lastSelectedLine and firstSelectedLine
		// 	We generate a list containing each selected line so users of the file
		//	format can add multiple selection easily.
		int16_t	titleWidth = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +16 ));
		int16_t	iconID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +18 ));
		if( !isButton && iconID > 0 )
		{
			if( titleWidth <= 0 )
				titleWidth = iconID;
			
			fprintf( mXmlFile, "\t\t\t<selectedLines>\n", titleWidth );
			for( int d = iconID; d <= titleWidth; d++ )
				fprintf( mXmlFile, "\t\t\t\t<integer>%d</integer>\n", d );
			fprintf( mXmlFile, "\t\t\t</selectedLines>\n" );
		}
		else if( isButton && styleFromLowNibble == 11 )	// Popup buttons use icon ID for selected line:
		{
			fprintf( mXmlFile, "\t\t\t<titleWidth>%d</titleWidth>\n", titleWidth );
			if( iconID != 0 )
			{
				fprintf( mXmlFile, "\t\t\t<selectedLines>\n" );
				fprintf( mXmlFile, "\t\t\t\t<integer>%d</integer>\n", iconID );
				fprintf( mXmlFile, "\t\t\t</selectedLines>\n" );
			}
		}
		else
		{
			fprintf( mXmlFile, "\t\t\t<titleWidth>%d</titleWidth>\n", titleWidth );
			fprintf( mXmlFile, "\t\t\t<icon>%d</icon>\n", iconID );
		}
		int16_t	textAlign = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +20 ));
		const char*		textAlignStr = "unknown";
		switch( textAlign )
		{
			case 0:
				textAlignStr = "left";
				break;
			case 1:
				textAlignStr = "center";
				break;
			case -1:
				textAlignStr = "right";
				break;
			case -2:
				textAlignStr = "forceLeft";
				break;
		}
		fprintf( mXmlFile, "\t\t\t<textAlign>%s</textAlign>\n", textAlignStr );
		int16_t	textFontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +22 ));
		fprintf( mXmlFile, "\t\t\t<textFontID>%d</textFontID>\n", textFontID );
		int16_t	textSize = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +24 ));
		fprintf( mXmlFile, "\t\t\t<textSize>%d</textSize>\n", textSize );
		int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +26 ));
		if( textStyleFlags & (1 << 15) )
			fprintf( mXmlFile, "\t\t\t<textStyle>group</textStyle>\n" );
		if( textStyleFlags & (1 << 14) )
			fprintf( mXmlFile, "\t\t\t<textStyle>extend</textStyle>\n" );
		if( textStyleFlags & (1 << 13) )
			fprintf( mXmlFile, "\t\t\t<textStyle>condense</textStyle>\n" );
		if( textStyleFlags & (1 << 12) )
			fprintf( mXmlFile, "\t\t\t<textStyle>shadow</textStyle>\n" );
		if( textStyleFlags & (1 << 11) )
			fprintf( mXmlFile, "\t\t\t<textStyle>outline</textStyle>\n" );
		if( textStyleFlags & (1 << 10) )
			fprintf( mXmlFile, "\t\t\t<textStyle>underline</textStyle>\n" );
		if( textStyleFlags & (1 << 9) )
			fprintf( mXmlFile, "\t\t\t<textStyle>italic</textStyle>\n" );
		if( textStyleFlags & (1 << 8) )
			fprintf( mXmlFile, "\t\t\t<textStyle>bold</textStyle>\n" );
		
		int16_t	textHeight = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +28 ));
		if( !isButton )
			fprintf( mXmlFile, "\t\t\t<textHeight>%d</textHeight>\n", textHeight );
		
		int x = 0, startOffs = currOffsIntoData +30;
		fprintf( mXmlFile, "\t\t\t<name>" );
		for( x = startOffs; blockData[x] != 0; x++ )
		{
			char currCh = blockData[x];
			if( currCh == '<' )
				fprintf( mXmlFile, "&lt;" );
			else if( currCh == '>' )
				fprintf( mXmlFile, "&gt;" );
			else if( currCh == '&' )
				fprintf( mXmlFile, "&amp;" );
			else
				fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
		}
		fprintf( mXmlFile, "</name>\n" );
		
		startOffs = x +2;
		fprintf( mXmlFile, "\t\t\t<script>" );
		for( x = startOffs; blockData[x] != 0; x++ )
		{
			char currCh = blockData[x];
			if( currCh == '<' )
				fprintf( mXmlFile, "&lt;" );
			else if( currCh == '>' )
				fprintf( mXmlFile, "&gt;" );
			else if( currCh == '&' )
				fprintf( mXmlFile, "&amp;" );
			else
				fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
		}
		fprintf( mXmlFile, "</script>\n" );
		
		fprintf( mXmlFile, "\t\t</part>\n" );
		
		currOffsIntoData += partLength;
		currOffsIntoData += (currOffsIntoData % 2);	// Align on even byte.
	}

	for( int n = 0; n < numContents; n++ )
	{
		int16_t		partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
		int16_t		partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
		
		fprintf( mXmlFile, "\t\t<content>\n" );
		
		CBuf		theText, theStyles;
		if( partID < 0 )	// It's a card part's contents:
		{
			partID = -partID;
			fprintf( mXmlFile, "\t\t\t<layer>card</layer>\n", partID );
			fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", partID );
			
			uint16_t	stylesLength = BIG_ENDIAN_16(blockData.uint16at( currOffsIntoData +4 ));
			if( stylesLength > 32767 )
			{
				stylesLength = stylesLength -32768;
				theStyles.resize( stylesLength -2 );
				theStyles.memcpy( 0, blockData, currOffsIntoData +6, stylesLength -2 );
			}
			else
				stylesLength = 0;
			theText.resize( partLength -stylesLength +1 );
			theText.memcpy( 0, blockData, currOffsIntoData +4 +stylesLength, partLength -stylesLength );
			theText[theText.size()-1] = 0;
		}
		else	// It's a bg part's contents:
		{
			fprintf( mXmlFile, "\t\t\t<layer>background</layer>\n" );
			fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", partID );
			
			uint16_t	stylesLength = BIG_ENDIAN_16(blockData.uint16at( currOffsIntoData +4 ));
			if( stylesLength > 32767 )
			{
				stylesLength = stylesLength -32768;
				theStyles.resize( stylesLength -2 );
				theStyles.memcpy( 0, blockData, currOffsIntoData +6, stylesLength -2 );
			}
			else
				stylesLength = 0;
			theText.resize( partLength -stylesLength +1 );
			theText.memcpy( 0, blockData, currOffsIntoData +4 +stylesLength,
								partLength -stylesLength );
			theText[theText.size()-1] = 0;
		}
		
		fprintf( mXmlFile, "\t\t\t<text>" );
		size_t	numChars = theText.size();
		for( int x = 0; x < numChars; x++ )
		{
			char currCh = theText[x];
			if( currCh == '<' )
				fprintf( mXmlFile, "&lt;" );
			else if( currCh == '>' )
				fprintf( mXmlFile, "&gt;" );
			else if( currCh == '&' )
				fprintf( mXmlFile, "&amp;" );
			else
				fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
		}
		fprintf( mXmlFile, "</text>\n" );
		if( theStyles.size() > 0 )
		{
//			char sfn[256] = { 0 };
//			snprintf( sfn, sizeof(sfn), "style_runs_%d_%d.styl", blockID, partID );
//			theStyles.tofile( sfn );
			
			for( size_t x = 0; x < theStyles.size(); )
			{
				int16_t	startOffset = BIG_ENDIAN_16(theStyles.int16at( x ));
				x += sizeof(int16_t);
				int16_t	styleID = BIG_ENDIAN_16(theStyles.int16at( x ));
				x += sizeof(int16_t);
				
				fprintf( mXmlFile, "\t\t\t<stylerun>\n" );
				fprintf( mXmlFile, "\t\t\t\t<offset>%u</offset>\n", startOffset );
				fprintf( mXmlFile, "\t\t\t\t<id>%u</id>\n", styleID );
				fprintf( mXmlFile, "\t\t\t</stylerun>\n" );
			}
		}
		
		currOffsIntoData += partLength +4 +(partLength % 2);	// Align on even byte.
		
		fprintf( mXmlFile, "\t\t</content>\n" );
	}

	int x = 0, startOffs = currOffsIntoData;
	fprintf( mXmlFile, "\t\t<name>" );
	for( x = startOffs; blockData[x] != 0; x++ )
	{
		char currCh = blockData[x];
		if( currCh == '<' )
			fprintf( mXmlFile, "&lt;" );
		else if( currCh == '>' )
			fprintf( mXmlFile, "&gt;" );
		else if( currCh == '&' )
			fprintf( mXmlFile, "&amp;" );
		else
			fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
	}
	fprintf( mXmlFile, "</name>\n" );
	
	startOffs = x +1;
	fprintf( mXmlFile, "\t\t<script>" );
	for( x = startOffs; blockData[x] != 0; x++ )
	{
		char currCh = blockData[x];
		if( currCh == '<' )
			fprintf( mXmlFile, "&lt;" );
		else if( currCh == '>' )
			fprintf( mXmlFile, "&gt;" );
		else if( currCh == '&' )
			fprintf( mXmlFile, "&amp;" );
		else
			fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
	}
	fprintf( mXmlFile, "</script>\n" );

	if( strcmp( "BKGD", vBlockType ) == 0 )
		fprintf( mXmlFile, "\t</background>\n" );
	else
		fprintf( mXmlFile, "\t</card>\n" );
	
	return true;
}


bool	CStackFile::LoadPageTable( int32_t blockID, CBuf& blockData )
{
	bool	success = true;
	
	if( mStatusMessages )
		fprintf( stdout, "Status: Processing 'PAGE' #%lu (%d bytes)\n", blockID, blockData.size() );

	if( mDumpRawBlockData )
	{
		char sfn[256] = { 0 };
		snprintf( sfn, sizeof(sfn), "PAGE_%d.data", blockID );
		blockData.tofile( sfn );
	}
	
	if( mCardBlockSize != -1 )
	{
		size_t		currDataOffs = 12;
		while( true )
		{
			if( !blockData.hasdata( currDataOffs, sizeof(int32_t) ) )
			{
				fprintf( stderr, "Warning: Premature end of 'PAGE' #%d (%lu bytes)\n", blockID, blockData.size() );
				break;
			}
			
			int32_t		currCardID = BIG_ENDIAN_32( blockData.int32at( currDataOffs ) );
			if( currCardID == 0 )
				break;	// End of page list. (Sentinel)
			
			success = LoadLayerBlock( "CARD", currCardID, mBlockMap[CStackBlockIdentifier("CARD",currCardID)] );
			
			currDataOffs += mCardBlockSize;
		}
	}
	else
		fprintf( stderr, "Warning: Couldn't parse 'PAGE' #%d (%lu bytes) because it preceded the page table list.\n", blockID, blockData.size() );
	
	return success;
}


bool	CStackFile::LoadListBlock( CBuf& blockData )
{
	if( mStatusMessages )
		fprintf( stdout, "Status: Processing 'LIST' #%d (%d bytes)\n", mListBlockID, blockData.size() );
	
	if( mDumpRawBlockData )
	{
		char sfn[256] = { 0 };
		snprintf( sfn, sizeof(sfn), "LIST_%d.data", mListBlockID );
		blockData.tofile( sfn );
	}

	size_t		currDataOffs = 4;
	int32_t		numPageTables = BIG_ENDIAN_32(blockData.int32at(currDataOffs));
	currDataOffs += 12;
	mCardBlockSize = BIG_ENDIAN_16(blockData.int16at(currDataOffs));
	currDataOffs += 18;
	for( int32_t r = 0; r < numPageTables; r++ )
	{
		currDataOffs += 2;
		if( !blockData.hasdata( currDataOffs, sizeof(int32_t) ) )
		{
			fprintf( stderr, "Warning: Premature end of 'LIST' #%d (%d bytes)\n", mListBlockID, blockData.size() );
			break;
		}
		
		int32_t		currPagetableID = BIG_ENDIAN_32( blockData.int32at( currDataOffs ) );
		
		LoadPageTable( currPagetableID, mBlockMap[CStackBlockIdentifier("PAGE",currPagetableID)] );
		
		currDataOffs += 4;
	}
	
	return true;
}


bool	CStackFile::LoadFile( const std::string& fpath )
{
	#if 0
	CStackBlockIdentifier		a( "TEST", 1 );
	CStackBlockIdentifier		b( "TEST", 2 );
	CStackBlockIdentifier		wildcardYes( "TEST" );
	CStackBlockIdentifier		wildcardNo( "TOON" );
	
	// ==
	bool	testState = a == b;
	
	testState = a == wildcardYes;
	testState = wildcardYes == a;
	
	testState = b == wildcardYes;
	testState = wildcardYes == b;
	
	testState = a == wildcardNo;
	testState = wildcardNo == a;
	
	testState = b == wildcardNo;
	testState = wildcardNo == b;
	
	// <
	testState = a < b;
	
	testState = a < wildcardYes;
	testState = wildcardYes < a;
	
	testState = b < wildcardYes;
	testState = wildcardYes < b;
	
	testState = a < wildcardNo;
	testState = wildcardNo < a;
	
	testState = b < wildcardNo;
	testState = wildcardNo < b;
	
	// >
	testState = a > b;
	
	testState = a > wildcardYes;
	testState = wildcardYes > a;
	
	testState = b > wildcardYes;
	testState = wildcardYes > b;
	
	testState = a > wildcardNo;
	testState = wildcardNo > a;
	
	testState = b > wildcardNo;
	testState = wildcardNo > b;
	#endif
	
	std::ifstream		theFile( fpath.c_str() );
	if( !theFile.is_open() )
	{
		fprintf( stderr, "Error: Couldn't open file '%s'\n", fpath.c_str() );
		return false;
	}
	
	std::string				packagePath( fpath );
	std::string::size_type	pos = packagePath.rfind( std::string(".stak") );
	if( pos != std::string::npos )
		packagePath.resize( pos, 'X' );
	packagePath.append( ".xstk" );
	mkdir( packagePath.c_str(), 0777 );
	chdir( packagePath.c_str() );
	
	packagePath.append( "/toc.xml" );
	mXmlFile = fopen( packagePath.c_str(), "w" );
	if( !mXmlFile )
	{
		fprintf( stderr, "Error: Couldn't create file TOC at '%s'\n", packagePath.c_str() );
		return false;
	}
	
	if( mStatusMessages )
		fprintf( stdout, "Status: Output package name is '%s'\n", packagePath.c_str() );
	
	fprintf( mXmlFile, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<!DOCTYPE stackfile PUBLIC \"-//Apple, Inc.//DTD stackfile V 2.0//EN\" \"\" >\n<stackfile>\n" );
	
	char		vBlockType[5] = { 0 };
	u_int32_t	vBlockSize = 0;
	int32_t		vBlockID = 0;
	
	// Read all blocks so we can random-access them. Yes, I know there are more
	//	efficient ways, but honestly, who cares?
	while( true )
	{
		theFile.read( (char*) &vBlockSize, sizeof(vBlockSize) );
		if( theFile.eof() )	// Couldn't read because we hit end of file.
			break;
		
		vBlockSize = BIG_ENDIAN_32(vBlockSize);
		theFile.read( vBlockType, 4 );
		theFile.read( (char*) &vBlockID, sizeof(vBlockID) );
		vBlockID = BIG_ENDIAN_32(vBlockID);
		
		if( strcmp(vBlockType,"TAIL") == 0 && vBlockID == 0xffffffff )	// End marker block?
			break;
		else if( strcmp(vBlockType,"FREE") == 0 )	// Not a free, reusable block?
		{
			if( mStatusMessages )
				fprintf( stdout, "Status: Skipping '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			theFile.ignore( vBlockSize -12 );	// Skip rest of block data.
		}
		else
		{
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			CStackBlockIdentifier	theTypeAndID(vBlockType,vBlockID);
			mBlockMap[theTypeAndID] = blockData;
			//mBlockMap.insert( std::make_pair(theTypeAndID,blockData) );
			printf( "Status: Located block %s %d - (%d)\n", vBlockType, vBlockID, mBlockMap.size() );
		}
	}
	
	// Load some "table of contents"-style blocks other blocks need to refer to:
	CBlockMap::iterator		stackItty = mBlockMap.find(CStackBlockIdentifier("STAK"));
	if( stackItty == mBlockMap.end() )
		fprintf( stderr, "Error: Couldn't find stack block." );
	bool	success = LoadStackBlock( stackItty->second );
	if( success )
		success = LoadFontTable( -1, mBlockMap[CStackBlockIdentifier("FTBL")] );
	if( success )
		success = LoadStyleTable( -1, mBlockMap[CStackBlockIdentifier("STBL")] );
	if( success )
		success = LoadListBlock( mBlockMap[CStackBlockIdentifier("LIST",mListBlockID)] );
	
	// Now load all backgrounds and bitmaps:
	if( success )
	{
		CBlockMap::iterator	currBlockItty = mBlockMap.begin();
		for( ; currBlockItty != mBlockMap.end(); currBlockItty++ )
		{
			if( currBlockItty->first == CStackBlockIdentifier("BMAP") )
			{
				int32_t		blockID = currBlockItty->first.mID;
				CBuf&		blockData = currBlockItty->second;
				if( mStatusMessages )
					fprintf( stdout, "Status: Processing 'BMAP' #%d %X (%d bytes)\n", blockID, blockID, blockData.size() );
				fprintf( mXmlFile, "\t<!-- Processed 'BMAP' #%d (%d bytes) -->\n", blockID, blockData.size() );
				picture		thePicture;
				woba_decode( thePicture, blockData.buf() );
				
				char		fname[256];
				sprintf( fname, "BMAP_%u.pbm", blockID );
				thePicture.writebitmapandmasktopbm( fname );
			}
			else if( currBlockItty->first == CStackBlockIdentifier("BKGD") )
			{
				success = LoadLayerBlock( "BKGD", currBlockItty->first.mID, currBlockItty->second );
			}
		}
	}
		
	#if MAC_CODE
	SInt16		resRefNum = -1;
	FSRef		fileRef;
	
	OSStatus	err = FSPathMakeRef( (const UInt8*) fpath.c_str(), &fileRef, NULL );
	if( err == noErr )
	{
		resRefNum = FSOpenResFile( &fileRef, fsRdPerm );
		if( resRefNum < 0 )
		{
			fprintf( stderr, "Warning: No Mac resource fork to import.\n" );
			err = fnfErr;
		}
		else
		{
			// Export all B/W icons:
			SInt16		numIcons = Count1Resources( 'ICON' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currIcon = Get1IndResource( 'ICON', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currIcon, &theID, &theType, name );
				char		fname[256];
				
				if( mStatusMessages )
					fprintf( stdout, "Status: Converting 'ICON' %d.\n", theID );
				
				picture		theIcon( 32, 32, 1, false );
				theIcon.memcopyin( *currIcon, 0, 4 * 32 );
				
				theIcon.buildmaskfromsurroundings();
				
//				snprintf( fname, sizeof(fname), "ICON_%d_mask.pbm", theID );
//				theIcon.writemasktopbm( fname );
				
				snprintf( fname, sizeof(fname), "ICON_%d.pbm", theID );
				theIcon.writebitmapandmasktopbm( fname );
				
				fprintf( mXmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<type>icon</type>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( mXmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( mXmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( mXmlFile, "&amp;" );
					else
						fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( mXmlFile, "</name>\n\t\t<file>ICON_%d.pbm</file>\n\t</media>\n", theID );
			}

			// Export all PICT images:
			numIcons = Count1Resources( 'PICT' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currPicture = Get1IndResource( 'PICT', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currPicture, &theID, &theType, name );
				char		fname[256];
				
				if( mStatusMessages )
					fprintf( stdout, "Status: Converting 'PICT' %d.\n", theID );

				snprintf( fname, sizeof(fname), "PICT_%d.pict", theID );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'PICT' %d.\n", fname, theID );
					return false;
				}
				
				for( int n = 0; n < 8; n++ )
					fputs( "BILL_ATKINSON_ERIC_CARLSON_KEVIN_CALHOUN_DANIEL_THOME_HYPERCARD_", theFile );	// 64 bytes repeated 8 times is a neat 512 byte header.
				fwrite( *currPicture, GetHandleSize( currPicture ), 1, theFile );
				fclose( theFile );

				fprintf( mXmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<type>picture</type>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( mXmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( mXmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( mXmlFile, "&amp;" );
					else
						fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( mXmlFile, "</name>\n\t\t<file>PICT_%d.pict</file>\n\t</media>\n", theID );
			}

			// Export all CURS cursors:
			numIcons = Count1Resources( 'CURS' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currIcon = Get1IndResource( 'CURS', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currIcon, &theID, &theType, name );
				char		fname[256];
				
				if( mStatusMessages )
					fprintf( stdout, "Status: Converting 'CURS' %d.\n", theID );
				
				snprintf( fname, sizeof(fname), "CURS_%d.pbm", theID );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'CURS' %d.\n", fname, theID );
					return false;
				}
				
				fputs( "P4\n16 16\n", theFile );
				fwrite( *currIcon, 2 * 16, 1, theFile );
				fputs( "\nP4\n16 16\n", theFile );
				fwrite( (*currIcon) +(2 * 16), 2 * 16, 1, theFile );
				int16_t	vertPos = * (int16_t*) ((*currIcon) +(2 * 16) +(2 * 16));
				int16_t	horzPos = * (int16_t*) ((*currIcon) +(2 * 16) +(2 * 16) +2);
				printf("");
				fclose( theFile );
				
				fprintf( mXmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<type>cursor</type>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( mXmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( mXmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( mXmlFile, "&amp;" );
					else
						fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( mXmlFile, "</name>\n\t\t<file>CURS_%d.pbm</file>\n\t\t<hotspot>\n\t\t\t<left>%d</left>\n\t\t\t<top>%d</top>\n\t\t</hotspot>\n\t</media>\n", theID, horzPos, vertPos );
			}

			// Export all 'snd ' sound resources:
			EnterMovies();
			numIcons = Count1Resources( 'snd ' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currIcon = Get1IndResource( 'snd ', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currIcon, &theID, &theType, name );
				char		fname[256];

				if( mStatusMessages )
					fprintf( stdout, "Status: Converting 'snd ' %d.\n", theID );

				snprintf( fname, sizeof(fname), "snd_%d.aiff", theID );
 			   	Handle		myHandle = NewHandleClear(0);
				Handle		myDataRef = NewHandleClear( sizeof(Handle) );
				BlockMove( &myHandle, *myDataRef, sizeof(Handle) );
				
				Movie	theMovie = NewMovie( newMovieActive );
				err = GetMoviesError();
				if( !theMovie || err != noErr )
				{
					fprintf( stderr, "Error: Error %d creating QuickTime container for 'snd ' %d.\n", (int)err, theID );
					return false;
				}
				
				err = SetMovieDefaultDataRef( theMovie, myDataRef, HandleDataHandlerSubType);
				if( !theMovie || err != noErr )
				{
					fprintf( stderr, "Error: Error %d specifying data reference for 'snd ' %d.\n", (int)err, theID );
					return false;
				}
				
				err = PasteHandleIntoMovie( currIcon, 'snd ', theMovie, 0L, NULL );
				if( err != noErr )
				{
					fprintf( stderr, "Error: Error %d inserting data of 'snd ' %d into QuickTime container.\n", (int)err, theID );
					return false;
				}
				
				FSRef		packageRef;
				err = FSPathMakeRef( (UInt8*) packagePath.c_str(), &packageRef, NULL );
				if( err != noErr )
				{
					fprintf( stderr, "Error: Error %d creating reference to package to export 'snd ' %d.\n", (int)err, theID );
					return false;
				}
				FSSpec		theSpec = { 0 };
				err = FSGetCatalogInfo( &packageRef, kFSCatInfoNone, NULL, NULL, &theSpec, NULL );
				if( err != noErr )
				{
					fprintf( stderr, "Error: Error %d converting reference to package to export 'snd ' %d.\n", (int)err, theID );
					return false;
				}
				theSpec.name[0] = strlen(fname);
				strcpy( ((char*)theSpec.name) +1, fname );
				err = ConvertMovieToFile( theMovie, NULL, &theSpec, kQTFileTypeAIFF, 'TVOD', smSystemScript, NULL, createMovieFileDeleteCurFile | movieToFileOnlyExport, NULL );
				if( err != noErr )
				{
					fprintf( stderr, "Error: Error %d converting 'snd ' %d to AIFF.\n", (int)err, theID );
					return false;
				}
				
				DisposeMovie( theMovie );
				
				fprintf( mXmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( mXmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( mXmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( mXmlFile, "&amp;" );
					else
						fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( mXmlFile, "</name>\n\t\t<file>snd_%d.aiff</file>\n\t\t<type>sound</type>\n\t</media>\n", theID );
			}
			ExitMovies();
			
			// Export AddColor resources:
			numIcons = Count1Resources( 'HCbg' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currIcon = Get1IndResource( 'HCbg', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currIcon, &theID, &theType, name );
				char		fname[256];
				
				if( mStatusMessages )
					fprintf( stdout, "Status: Converting AddColor 'HCbg' %d.\n", theID );
				
				snprintf( fname, sizeof(fname), "HCbg_%d.data", theID );

				size_t	dataLen = GetHandleSize( currIcon );
				CBuf	theData( dataLen );
				theData.memcpy( 0, *currIcon, 0, dataLen );
				theData.tofile( fname );
				
				fprintf( mXmlFile, "\t<addcolorbackground>\n\t\t<id>%d</id>\n", theID );
				
				size_t	currOffs = 0;
				while( currOffs < dataLen )
				{
					fprintf( mXmlFile, "\t\t<addcolorobject>\n" );
					int8_t	currType = theData[currOffs];
					bool	vHidden = currType & (1 << 7);
					currType &= ~(1 << 7);
					currOffs += 1;
					
					switch( currType )
					{
						case 0x01:	// Button
						{
							fprintf( mXmlFile, "\t\t\t<type>button</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( mXmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( mXmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x02:	// Field
						{
							fprintf( mXmlFile, "\t\t\t<type>field</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( mXmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( mXmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x03:	// Rectangle
						{
							fprintf( mXmlFile, "\t\t\t<type>rectangle</type>\n" );
							uint16_t	rc = 0, gc = 0, bc = 0;
							int16_t		bevelDepth = 0;
							int16_t		l, t, r, b;
							fprintf( mXmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( mXmlFile, "\t\t\t</rect>\n" );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<bevel>%d</bevel>\n", t );
							fprintf( mXmlFile, "\t\t\t<color>\n" );
							rc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<red>%d</red>\n", rc );
							gc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<green>%d</green>\n", gc );
							bc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<blue>%d</blue>\n", bc );
							fprintf( mXmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x04:	// Picture
						{
							fprintf( mXmlFile, "\t\t\t<type>picture</type>\n" );
							int16_t		l, t, r, b;
							fprintf( mXmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( mXmlFile, "\t\t\t</rect>\n" );

							uint8_t	transparentFlag = theData[currOffs];
							currOffs += 1;
							fprintf( mXmlFile, "\t\t\t<transparent> %s </transparent>\n", transparentFlag ? "<true />" : "<false />" );
							
							uint8_t	imageNameLen = theData[currOffs];
							currOffs += 1;
							
							fprintf( mXmlFile, "\t\t\t<name>" );
							for( int n = 0; n < imageNameLen; n++ )
							{
								char currCh = theData[currOffs++];
								if( currCh == '<' )
									fprintf( mXmlFile, "&lt;" );
								else if( currCh == '>' )
									fprintf( mXmlFile, "&gt;" );
								else if( currCh == '&' )
									fprintf( mXmlFile, "&amp;" );
								else
									fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
							}
							fprintf( mXmlFile, "</name>\n" );
							break;
						}
						
						default:
							fprintf( mXmlFile, "\t\t\t<!-- Unknown type %x, aborting 'HCcd' resource %d. -->\n", currType, theID );
							fprintf( stderr, "Error: Unknown type %x, aborting 'HCcd' resource %d.\n", currType, theID );
							currOffs = dataLen;	// Can't read more. Skip the rest.
							break;
					}
					fprintf( mXmlFile, "\t\t\t<visible> %s </visible>\n", (vHidden ? "<false />" : "<true />") );
					fprintf( mXmlFile, "\t\t</addcolorobject>\n" );
				}
			
				fprintf( mXmlFile, "\t</addcolorbackground>\n" );
			}

			// Export more AddColor resources:
			numIcons = Count1Resources( 'HCcd' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currIcon = Get1IndResource( 'HCcd', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currIcon, &theID, &theType, name );
				char		fname[256];
				
				if( mStatusMessages )
					fprintf( stdout, "Status: Converting AddColor 'HCcd' %d.\n", theID );

				snprintf( fname, sizeof(fname), "HCcd_%d.data", theID );
				
				size_t	dataLen = GetHandleSize( currIcon );
				CBuf	theData( dataLen );
				theData.memcpy( 0, *currIcon, 0, dataLen );
				//theData.tofile( fname );
				
				fprintf( mXmlFile, "\t<addcolorcard>\n\t\t<id>%d</id>\n", theID );
				
				size_t	currOffs = 0;
				while( currOffs < dataLen )
				{
					fprintf( mXmlFile, "\t\t<addcolorobject>\n" );
					int8_t	currType = theData[currOffs];
					bool	vHidden = currType & (1 << 7);
					currType &= ~(1 << 7);
					currOffs += 1;
					
					switch( currType )
					{
						case 0x01:	// Button
						{
							fprintf( mXmlFile, "\t\t\t<type>button</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( mXmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( mXmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x02:	// Field
						{
							fprintf( mXmlFile, "\t\t\t<type>field</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( mXmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( mXmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x03:	// Rectangle
						{
							fprintf( mXmlFile, "\t\t\t<type>rectangle</type>\n" );
							uint16_t	rc = 0, gc = 0, bc = 0;
							int16_t		bevelDepth = 0;
							int16_t		l, t, r, b;
							fprintf( mXmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( mXmlFile, "\t\t\t</rect>\n" );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( mXmlFile, "\t\t\t<color>\n" );
							rc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<red>%d</red>\n", rc );
							gc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<green>%d</green>\n", gc );
							bc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<blue>%d</blue>\n", bc );
							fprintf( mXmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x04:	// Picture
						{
							fprintf( mXmlFile, "\t\t\t<type>picture</type>\n" );
							int16_t		l, t, r, b;
							fprintf( mXmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( mXmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( mXmlFile, "\t\t\t</rect>\n" );

							uint8_t	transparentFlag = theData[currOffs];
							currOffs += 1;
							fprintf( mXmlFile, "\t\t\t<transparent> %s </transparent>\n", transparentFlag ? "<true />" : "<false />" );
							
							uint8_t	imageNameLen = theData[currOffs];
							currOffs += 1;
							
							fprintf( mXmlFile, "\t\t\t<name>" );
							for( int n = 0; n < imageNameLen; n++ )
							{
								char currCh = theData[currOffs++];
								if( currCh == '<' )
									fprintf( mXmlFile, "&lt;" );
								else if( currCh == '>' )
									fprintf( mXmlFile, "&gt;" );
								else if( currCh == '&' )
									fprintf( mXmlFile, "&amp;" );
								else
									fprintf( mXmlFile, "%s", UniCharFromMacRoman(currCh) );
							}
							fprintf( mXmlFile, "</name>\n" );
							break;
						}
						
						default:
							fprintf( mXmlFile, "\t\t\t<!-- Unknown type %x, aborting 'HCcd' resource %d. -->\n", currType, theID );
							fprintf( stderr, "Error: Unknown type %x, aborting 'HCcd' resource %d.\n", currType, theID );
							currOffs = dataLen;	// Can't read more. Skip the rest.
							break;
					}
					fprintf( mXmlFile, "\t\t\t<visible> %s </visible>\n", (vHidden ? "<false />" : "<true />") );
					fprintf( mXmlFile, "\t\t</addcolorobject>\n" );
				}
			
				fprintf( mXmlFile, "\t</addcolorcard>\n" );
			}
			
			// Export all XCMD resources:
			numIcons = Count1Resources( 'XCMD' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currPicture = Get1IndResource( 'XCMD', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currPicture, &theID, &theType, name );
				char		fname[256];
				int			nameLen = name[0];
				
				// C -> P-String:
				memmove( name, name +1, nameLen );
				name[nameLen] = 0;
				
				fprintf( stderr, "Warning: Skipping code resource 'XCMD' %d \"%s\".\n", theID, name );

				snprintf( fname, sizeof(fname), "XCMD_68k_%d_%s.data", theID, name );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'XCMD' %d.\n", fname, theID );
					return false;
				}
				
				fwrite( *currPicture, GetHandleSize( currPicture ), 1, theFile );
				fclose( theFile );

				fprintf( mXmlFile, "\t<externalcommand type=\"command\" platform=\"mac68k\" id=\"%d\" size=\"%ld\" name=\"%s\" file=\"%s\" />\n",
									theID, GetHandleSize( currPicture ), name, fname );
			}
			
			// Export all XFCN resources:
			numIcons = Count1Resources( 'XFCN' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currPicture = Get1IndResource( 'XFCN', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currPicture, &theID, &theType, name );
				char		fname[256];
				int			nameLen = name[0];
				
				// C -> P-String:
				memmove( name, name +1, nameLen );
				name[nameLen] = 0;
				
				fprintf( stderr, "Warning: Skipping code resource 'XFCN' %d \"%s\".\n", theID, name );

				snprintf( fname, sizeof(fname), "XFCN_68k_%d_%s.data", theID, name );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'XFCN' %d.\n", fname, theID );
					return false;
				}
				
				fwrite( *currPicture, GetHandleSize( currPicture ), 1, theFile );
				fclose( theFile );

				fprintf( mXmlFile, "\t<externalcommand type=\"function\" platform=\"mac68k\" id=\"%d\" size=\"%ld\" name=\"%s\" file=\"%s\" />\n",
									theID, GetHandleSize( currPicture ), name, fname );
			}
			
			// Export all xcmd resources:
			numIcons = Count1Resources( 'xcmd' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currPicture = Get1IndResource( 'xcmd', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currPicture, &theID, &theType, name );
				char		fname[256];
				int			nameLen = name[0];
				
				// C -> P-String:
				memmove( name, name +1, nameLen );
				name[nameLen] = 0;
				
				fprintf( stderr, "Warning: Skipping code resource 'xcmd' %d \"%s\".\n", theID, name );

				snprintf( fname, sizeof(fname), "xcmd_ppc_%d_%s.data", theID, name );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'xcmd' %d.\n", fname, theID );
					return false;
				}
				
				fwrite( *currPicture, GetHandleSize( currPicture ), 1, theFile );
				fclose( theFile );

				fprintf( mXmlFile, "\t<externalcommand type=\"command\" platform=\"macppc\" id=\"%d\" size=\"%ld\" name=\"%s\" file=\"%s\" />\n",
									theID, GetHandleSize( currPicture ), name, fname );
			}
			
			// Export all XFCN resources:
			numIcons = Count1Resources( 'xfcn' );
			for( SInt16 x = 1; x <= numIcons; x++ )	// Get1IndResource uses 1-based indexes.
			{
				Handle		currPicture = Get1IndResource( 'xfcn', x );
				ResID       theID = 0;
				ResType		theType = 0L;
				Str255		name;
				GetResInfo( currPicture, &theID, &theType, name );
				char		fname[256];
				int			nameLen = name[0];
				
				// C -> P-String:
				memmove( name, name +1, nameLen );
				name[nameLen] = 0;
				
				fprintf( stderr, "Warning: Skipping code resource 'xfcn' %d \"%s\".\n", theID, name );

				snprintf( fname, sizeof(fname), "xfcn_ppc_%d_%s.data", theID, name );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'xfcn' %d.\n", fname, theID );
					return false;
				}
				
				fwrite( *currPicture, GetHandleSize( currPicture ), 1, theFile );
				fclose( theFile );

				fprintf( mXmlFile, "\t<externalcommand type=\"function\" platform=\"macppc\" id=\"%d\" size=\"%ld\" name=\"%s\" file=\"%s\" />\n",
									theID, GetHandleSize( currPicture ), name, fname );
			}
			
			CloseResFile( resRefNum );
		}
	}
	else
		fprintf( stderr, "Error: Error %d locating input file's resource fork.\n", (int)err );
	#endif // MAC_CODE
	
	fprintf( mXmlFile, "</stackfile>\n" );
	if( mXmlFile != stdout )
		fclose( mXmlFile );
	
	#if MAC_CODE
	if( err != fnfErr && err != noErr )
	{
		fprintf( stderr, "Error: During conversion of Macintosh fork of stack.\n" );
		return false;
	}
	#endif // MAC_CODE
	
	return true;
}
