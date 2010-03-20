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


bool	CStackFile::LoadFile( const std::string& fpath )
{
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
	FILE*		xmlFile = fopen( packagePath.c_str(), "w" );
	if( !xmlFile )
	{
		fprintf( stderr, "Error: Couldn't create file TOC at '%s'\n", packagePath.c_str() );
		return false;
	}
	
	fprintf( xmlFile, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<!DOCTYPE stackfile PUBLIC \"-//Apple, Inc.//DTD stackfile V 2.0//EN\" \"\" >\n<stackfile>\n" );
	
	// Read block header:
	char		vBlockType[5] = { 0 };
	u_int32_t	vBlockSize = 0,
				vBlockID = 0;
	
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
		else if( strcmp(vBlockType,"BMAP") == 0 )	// Image block?
		{
			fprintf( stdout, "Status: Processing '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- Processed '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			picture		thePicture;
			char*		pictureData = new char[vBlockSize -12];
			theFile.read( pictureData, vBlockSize -12 );
			woba_decode( thePicture, pictureData );
			
			char		fname[256];
			sprintf( fname, "BMAP_%u.pbm", vBlockID );
			thePicture.writebitmapandmasktopbm( fname );
			delete [] pictureData;
		}
		else if( strcmp(vBlockType,"STAK") == 0 )
		{
			fprintf( stdout, "Status: Processing '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<stack>\n" );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int16_t	userLevel = BIG_ENDIAN_16(blockData.int16at( 60 ));
			fprintf( xmlFile, "\t\t<userLevel>%d</userLevel>\n", userLevel );
			int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 64 ));
			fprintf( xmlFile, "\t\t<cantModify> %s </cantModify>\n", (flags & (1 << 15)) ? "<true />" : "<false />" );
			fprintf( xmlFile, "\t\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
			fprintf( xmlFile, "\t\t<privateAccess> %s </privateAccess>\n", (flags & (1 << 13)) ? "<true />" : "<false />" );
			fprintf( xmlFile, "\t\t<cantAbort> %s </cantAbort>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
			fprintf( xmlFile, "\t\t<cantPeek> %s </cantPeek>\n", (flags & (1 << 10)) ? "<true />" : "<false />" );
			char		versStr[16] = { 0 };
			int32_t	version0 = blockData.int32at( 84 );
			NumVersionToStr( (char*) &version0, versStr );
			fprintf( xmlFile, "\t\t<version>%s</version>\n", versStr );
			int32_t	version1 = blockData.int32at( 88 );
			NumVersionToStr( (char*) &version1, versStr );
			fprintf( xmlFile, "\t\t<version>%s</version>\n", versStr );
			int32_t	version2 = blockData.int32at( 92 );
			NumVersionToStr( (char*) &version2, versStr );
			fprintf( xmlFile, "\t\t<version>%s</version>\n", versStr );
			int32_t	version3 = blockData.int32at( 96 );
			NumVersionToStr( (char*) &version3, versStr );
			fprintf( xmlFile, "\t\t<version>%s</version>\n", versStr );
			int16_t	height = BIG_ENDIAN_16(blockData.int16at( 428 ));
			if( height == 0 )
				height = 342;
			int16_t	width = BIG_ENDIAN_16(blockData.int16at( 430 ));
			if( width == 0 )
				width = 512;
			fprintf( xmlFile, "\t\t<cardSize>\n\t\t\t<width>%d</width>\n\t\t\t<height>%d</height>\n\t\t</cardSize>\n", width, height );

			fprintf( xmlFile, "\t\t<patterns>\n" );
			char			pattern[8] = { 0 };
			int				offs = 0x2b4;
			for( int n = 0; n < 40; n++ )
			{
				memmove( pattern, blockData.buf( offs, 8 ), 8 );
				char		fname[256] = { 0 };
				sprintf( fname, "PAT_%u.pbm", n +1 );
				picture		thePicture( 8, 8, 1, false );
				thePicture.memcopyin( pattern, 0, 8 );
				thePicture.writebitmaptopbm( fname );
				offs += 8;
				fprintf(xmlFile,"\t\t\t<pattern>PAT_%u.pbm</pattern>\n", n+1);
			}
			fprintf( xmlFile, "\t\t</patterns>\n" );
			
			int x = 0, startOffs = 0x5f4;
			fprintf( xmlFile, "\t\t<script>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					fprintf( xmlFile, "&lt;" );
				else if( currCh == '>' )
					fprintf( xmlFile, "&gt;" );
				else if( currCh == '&' )
					fprintf( xmlFile, "&amp;" );
				else
					fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
			}
			fprintf( xmlFile, "</script>\n" );
			fprintf( xmlFile, "\t</stack>\n" );
		}
		else if( strcmp(vBlockType,"BKGD") == 0 )
		{
			fprintf( stdout, "Status: Processing '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<background>\n" );
			fprintf( xmlFile, "\t\t<id>%d</id>\n", vBlockID );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int32_t	bitmapID = BIG_ENDIAN_32(blockData.int32at( 4 ));
			fprintf( xmlFile, "\t\t<bitmap>BMAP_%u.pbm</bitmap>\n", bitmapID );
			int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 0x0C -4 ));
			fprintf( xmlFile, "\t\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
			fprintf( xmlFile, "\t\t<showPict> %s </showPict>\n", (flags & (1 << 13)) ? "<false />" : "<true />" );	// showPict is stored reversed.
			fprintf( xmlFile, "\t\t<dontSearch> %s </dontSearch>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
			int16_t	numParts = BIG_ENDIAN_16(blockData.int16at( 0x18 ));
			int16_t	numContents = BIG_ENDIAN_16(blockData.int16at( 0x20 ));
			size_t	currOffsIntoData = 38;
			for( int n = 0; n < numParts; n++ )
			{
				int16_t	partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				
				fprintf( xmlFile, "\t\t<part>\n" );
				int16_t	partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				fprintf( xmlFile, "\t\t\t<id>%d</id>\n", partID );
				int16_t	flagsAndType = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +4 ));
				int16_t	partType = flagsAndType >> 8;
				bool	isButton = partType == 1;
				fprintf( xmlFile, "\t\t\t<type>%s</type>\n", isButton ? "button" : "field" );
				fprintf( xmlFile, "\t\t\t<visible> %s </visible>\n", (flagsAndType & (1 << 7)) ? "<false />" : "<true />" );	// Really "hidden" flag.
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<dontWrap> %s </dontWrap>\n", (flagsAndType & (1 << 5)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved5> %d </reserved5>\n", (flagsAndType & (1 << 5)) >> 5 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<dontSearch> %s </dontSearch>\n", (flagsAndType & (1 << 4)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved4> %d </reserved4>\n", (flagsAndType & (1 << 4)) >> 4 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<sharedText> %s </sharedText>\n", (flagsAndType & (1 << 3)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved3> %d </reserved3>\n", (flagsAndType & (1 << 3)) >> 3 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<fixedLineHeight> %s </fixedLineHeight>\n", (flagsAndType & (1 << 2)) ? "<false />" : "<true />" );	// Really "use real line height" flag.
				else
					fprintf( xmlFile, "\t\t\t<reserved2> %d </reserved2>\n", (flagsAndType & (1 << 2)) >> 2 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<autoTab> %s </autoTab>\n", (flagsAndType & (1 << 1)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved1> %d </reserved1>\n", (flagsAndType & (1 << 1)) >> 1 );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<enabled> %s </enabled>\n", (flagsAndType & (1 << 0)) ? "<false />" : "<true />" );	// Same as lockText on fields. Really "disabled" flag.
				else
					fprintf( xmlFile, "\t\t\t<lockText> %s </lockText>\n", (flagsAndType & (1 << 0)) ? "<true />" : "<false />" );	// Same as enabled on buttons.
				fprintf( xmlFile, "\t\t\t<rect>\n\t\t\t\t<left>%d</left>\n\t\t\t\t<top>%d</top>\n\t\t\t\t<right>%d</right>\n\t\t\t\t<bottom>%d</bottom>\n\t\t\t</rect>\n",
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
				fprintf( xmlFile, "\t\t\t<style>%s</style>\n", styleStr );
				moreFlags = moreFlags >> 8;
				if( isButton )
					fprintf( xmlFile, "\t\t\t<showName> %s </showName>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<autoSelect> %s </autoSelect>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<highlight> %s </highlight>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<showLines> %s </showLines>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<wideMargins> %s </wideMargins>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved25> %d </reserved25>\n", (flagsAndType & (1 << 5)) >> 5 );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<sharedHighlight> %s </sharedHighlight>\n", (moreFlags & (1 << 4)) ? "<false />" : "<true />" );
				else
					fprintf( xmlFile, "\t\t\t<multipleLines> %s </multipleLines>\n", (moreFlags & (1 << 4)) ? "<true />" : "<false />" );
				int8_t	family = moreFlags & 15;
				if( isButton )
					fprintf( xmlFile, "\t\t\t<family>%d</family>\n", family );
				else
					fprintf( xmlFile, "\t\t\t<reservedFamily> %d </reservedFamily>\n", family );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<autoHighlight> %s </autoHighlight>\n", (moreFlags & (1 << 5) || family != 0) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved35> %d </reserved35>\n", (flagsAndType & (1 << 5)) >> 5 );
				
				// titleWidth & iconID are list fields' lastSelectedLine and firstSelectedLine
				// 	We generate a list containing each selected line so users of the file
				//	format can add multiple selection easily.
				int16_t	titleWidth = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +16 ));
				if( isButton )
					fprintf( xmlFile, "\t\t\t<titleWidth>%d</titleWidth>\n", titleWidth );
				int16_t	iconID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +18 ));
				if( isButton )
					fprintf( xmlFile, "\t\t\t<icon>%d</icon>\n", iconID );
				if( !isButton && iconID > 0 )
				{
					if( titleWidth <= 0 )
						titleWidth = iconID;
					
					fprintf( xmlFile, "\t\t\t<selectedLines>\n", titleWidth );
					for( int d = iconID; d <= titleWidth; d++ )
						fprintf( xmlFile, "\t\t\t\t<integer>%d</integer>\n", d );
					fprintf( xmlFile, "\t\t\t</selectedLines>\n" );
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
				fprintf( xmlFile, "\t\t\t<textAlign>%s</textAlign>\n", textAlignStr );
				int16_t	textFontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +22 ));
				fprintf( xmlFile, "\t\t\t<textFontID>%d</textFontID>\n", textFontID );
				int16_t	textSize = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +24 ));
				fprintf( xmlFile, "\t\t\t<textSize>%d</textSize>\n", textSize );
				int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +26 ));
				if( textStyleFlags & (1 << 15) )
					fprintf( xmlFile, "\t\t\t<textStyle>group</textStyle>\n" );
				if( textStyleFlags & (1 << 14) )
					fprintf( xmlFile, "\t\t\t<textStyle>extend</textStyle>\n" );
				if( textStyleFlags & (1 << 13) )
					fprintf( xmlFile, "\t\t\t<textStyle>condense</textStyle>\n" );
				if( textStyleFlags & (1 << 12) )
					fprintf( xmlFile, "\t\t\t<textStyle>shadow</textStyle>\n" );
				if( textStyleFlags & (1 << 11) )
					fprintf( xmlFile, "\t\t\t<textStyle>outline</textStyle>\n" );
				if( textStyleFlags & (1 << 10) )
					fprintf( xmlFile, "\t\t\t<textStyle>underline</textStyle>\n" );
				if( textStyleFlags & (1 << 9) )
					fprintf( xmlFile, "\t\t\t<textStyle>italic</textStyle>\n" );
				if( textStyleFlags & (1 << 8) )
					fprintf( xmlFile, "\t\t\t<textStyle>bold</textStyle>\n" );
				if( textStyleFlags == 0 )
					fprintf( xmlFile, "\t\t\t<textStyle>plain</textStyle>\n" );
				
				int16_t	textHeight = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +28 ));
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<textHeight>%d</textHeight>\n", textHeight );
				
				int x = 0, startOffs = currOffsIntoData +30;
				fprintf( xmlFile, "\t\t\t<name>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n" );
				
				startOffs = x +2;
				fprintf( xmlFile, "\t\t\t<script>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</script>\n" );
				
				fprintf( xmlFile, "\t\t</part>\n" );
				
				currOffsIntoData += partLength;
				currOffsIntoData += (currOffsIntoData % 2);	// Align on even byte.
			}

			for( int n = 0; n < numContents; n++ )
			{
				int16_t		partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				int16_t		partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				fprintf( xmlFile, "\t\t<content>\n" );
				
				CBuf		theText, theStyles;
				if( partID < 0 )	// It's a card part's contents:
				{
					partID = -partID;
					fprintf( xmlFile, "\t\t\t<layer>card</layer>\n", partID );
					fprintf( xmlFile, "\t\t\t<id>%d</id>\n", partID );
					
					uint16_t	stylesLength = BIG_ENDIAN_16(blockData.uint16at( currOffsIntoData +4 ));
					if( stylesLength > 32767 )
					{
						stylesLength = stylesLength -32768;
						theStyles.resize( stylesLength -2 );
						theStyles.memcpy( 0, blockData, currOffsIntoData +5, stylesLength -2 );
					}
					else
						stylesLength = 0;
					theText.resize( partLength -stylesLength +1 );
					theText.memcpy( 0, blockData, currOffsIntoData +5 +stylesLength, partLength -1 -stylesLength );
					theText[theText.size()-1] = 0;
				}
				else	// It's a bg part's contents:
				{
					fprintf( xmlFile, "\t\t\t<layer>background</layer>\n", partID );
					fprintf( xmlFile, "\t\t\t<id>%d</id>\n", partID );
					
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
				
				fprintf( xmlFile, "\t\t\t<text>" );
				size_t	numChars = theText.size();
				for( int x = 0; x < numChars; x++ )
				{
					char currCh = theText[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</text>\n" );
				
				if( theStyles.size() > 0 )
				{
					for( size_t x = 0; x < theStyles.size(); )
					{
						int16_t	startOffset = BIG_ENDIAN_16(theStyles.int16at( x ));
						x += sizeof(int16_t);
						int16_t	styleID = BIG_ENDIAN_16(theStyles.int16at( x ));
						x += sizeof(int16_t);
						
						fprintf( xmlFile, "\t\t\t<stylerun>\n" );
						fprintf( xmlFile, "\t\t\t\t<offset>%u</offset>\n", startOffset );
						fprintf( xmlFile, "\t\t\t\t<id>%u</id>\n", styleID );
						fprintf( xmlFile, "\t\t\t</stylerun>\n" );
					}
					
//					char sfn[256] = { 0 };
//					snprintf( sfn, sizeof(sfn), "style_runs_%d_%d.styl", vBlockID, partID );
//					theStyles.tofile( sfn );
				}
				
				currOffsIntoData += partLength +4 +(partLength % 2);	// Align on even byte.
				
				fprintf( xmlFile, "\t\t</content>\n" );
			}
			
			int x = 0, startOffs = currOffsIntoData;
			fprintf( xmlFile, "\t\t<name>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					fprintf( xmlFile, "&lt;" );
				else if( currCh == '>' )
					fprintf( xmlFile, "&gt;" );
				else if( currCh == '&' )
					fprintf( xmlFile, "&amp;" );
				else
					fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
			}
			fprintf( xmlFile, "</name>\n" );
			
			startOffs = x +1;
			fprintf( xmlFile, "\t\t<script>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					fprintf( xmlFile, "&lt;" );
				else if( currCh == '>' )
					fprintf( xmlFile, "&gt;" );
				else if( currCh == '&' )
					fprintf( xmlFile, "&amp;" );
				else
					fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
			}
			fprintf( xmlFile, "</script>\n" );

			fprintf( xmlFile, "\t</background>\n" );
		}
		else if( strcmp(vBlockType,"CARD") == 0 )
		{
			fprintf( stdout, "Status: Processing '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<card>\n" );
			fprintf( xmlFile, "\t\t<id>%d</id>\n", vBlockID );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int32_t	bitmapID = BIG_ENDIAN_32(blockData.int32at( 4 ));
			if( bitmapID != 0 )
				fprintf( xmlFile, "\t\t<bitmap>BMAP_%u.pbm</bitmap>\n", bitmapID );
			int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 8 ));
			fprintf( xmlFile, "\t\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
			fprintf( xmlFile, "\t\t<showPict> %s </showPict>\n", (flags & (1 << 13)) ? "<false />" : "<true />" );	// showPict is stored reversed.
			fprintf( xmlFile, "\t\t<dontSearch> %s </dontSearch>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
			int16_t	owner = BIG_ENDIAN_16(blockData.int16at( 22 ));
			fprintf( xmlFile, "\t\t<owner>%d</owner>\n", owner );
			int16_t	numParts = BIG_ENDIAN_16(blockData.int16at( 28 ));
			int16_t	numContents = BIG_ENDIAN_16(blockData.int16at( 36 ));
			size_t	currOffsIntoData = 42;
			for( int n = 0; n < numParts; n++ )
			{
				int16_t	partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				
				fprintf( xmlFile, "\t\t<part>\n" );
				int16_t	partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				fprintf( xmlFile, "\t\t\t<id>%d</id>\n", partID );
				int16_t	flagsAndType = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +4 ));
				int16_t	partType = flagsAndType >> 8;
				bool	isButton = partType == 1;
				fprintf( xmlFile, "\t\t\t<type>%s</type>\n", isButton ? "button" : "field" );
				fprintf( xmlFile, "\t\t\t<visible> %s </visible>\n", (flagsAndType & (1 << 7)) ? "<false />" : "<true />" );	// Really "hidden" flag.
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<dontWrap> %s </dontWrap>\n", (flagsAndType & (1 << 5)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved5> %d </reserved5>\n", (flagsAndType & (1 << 5)) >> 5 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<dontSearch> %s </dontSearch>\n", (flagsAndType & (1 << 4)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved4> %d </reserved4>\n", (flagsAndType & (1 << 4)) >> 4 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<sharedText> %s </sharedText>\n", (flagsAndType & (1 << 3)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved3> %d </reserved3>\n", (flagsAndType & (1 << 3)) >> 3 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<fixedLineHeight> %s </fixedLineHeight>\n", (flagsAndType & (1 << 2)) ? "<false />" : "<true />" );	// Really "use real line height" flag.
				else
					fprintf( xmlFile, "\t\t\t<reserved2> %d </reserved2>\n", (flagsAndType & (1 << 2)) >> 2 );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<autoTab> %s </autoTab>\n", (flagsAndType & (1 << 1)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved1> %d </reserved1>\n", (flagsAndType & (1 << 1)) >> 1 );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<enabled> %s </enabled>\n", (flagsAndType & (1 << 0)) ? "<false />" : "<true />" );	// Same as lockText on fields. Really "disabled" flag.
				else
					fprintf( xmlFile, "\t\t\t<lockText> %s </lockText>\n", (flagsAndType & (1 << 0)) ? "<true />" : "<false />" );	// Same as enabled on buttons.
				fprintf( xmlFile, "\t\t\t<rect>\n\t\t\t\t<left>%d</left>\n\t\t\t\t<top>%d</top>\n\t\t\t\t<right>%d</right>\n\t\t\t\t<bottom>%d</bottom>\n\t\t\t</rect>\n",
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
				fprintf( xmlFile, "\t\t\t<style>%s</style>\n", styleStr );
				moreFlags = moreFlags >> 8;
				if( isButton )
					fprintf( xmlFile, "\t\t\t<showName> %s </showName>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<autoSelect> %s </autoSelect>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<highlight> %s </highlight>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<showLines> %s </showLines>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<wideMargins> %s </wideMargins>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved25> %d </reserved25>\n", (flagsAndType & (1 << 5)) >> 5 );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<sharedHighlight> %s </sharedHighlight>\n", (moreFlags & (1 << 4)) ? "<false />" : "<true />" );
				else
					fprintf( xmlFile, "\t\t\t<multipleLines> %s </multipleLines>\n", (moreFlags & (1 << 4)) ? "<true />" : "<false />" );
				int8_t	family = moreFlags & 15;
				if( isButton )
					fprintf( xmlFile, "\t\t\t<family>%d</family>\n", family );
				else
					fprintf( xmlFile, "\t\t\t<reservedFamily> %d </reservedFamily>\n", family );
				if( isButton )
					fprintf( xmlFile, "\t\t\t<autoHighlight> %s </autoHighlight>\n", (moreFlags & (1 << 5) || family != 0) ? "<true />" : "<false />" );
				else
					fprintf( xmlFile, "\t\t\t<reserved35> %d </reserved35>\n", (flagsAndType & (1 << 5)) >> 5 );
				
				// titleWidth & iconID are list fields' lastSelectedLine and firstSelectedLine
				// 	We generate a list containing each selected line so users of the file
				//	format can add multiple selection easily.
				int16_t	titleWidth = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +16 ));
				if( isButton )
					fprintf( xmlFile, "\t\t\t<titleWidth>%d</titleWidth>\n", titleWidth );
				int16_t	iconID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +18 ));
				if( isButton )
					fprintf( xmlFile, "\t\t\t<icon>%d</icon>\n", iconID );
				if( !isButton && iconID > 0 )
				{
					if( titleWidth <= 0 )
						titleWidth = iconID;
					
					fprintf( xmlFile, "\t\t\t<selectedLines>\n", titleWidth );
					for( int d = iconID; d <= titleWidth; d++ )
						fprintf( xmlFile, "\t\t\t\t<integer>%d</integer>\n", d );
					fprintf( xmlFile, "\t\t\t</selectedLines>\n" );
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
				fprintf( xmlFile, "\t\t\t<textAlign>%s</textAlign>\n", textAlignStr );
				int16_t	textFontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +22 ));
				fprintf( xmlFile, "\t\t\t<textFontID>%d</textFontID>\n", textFontID );
				int16_t	textSize = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +24 ));
				fprintf( xmlFile, "\t\t\t<textSize>%d</textSize>\n", textSize );
				int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +26 ));
				if( textStyleFlags & (1 << 15) )
					fprintf( xmlFile, "\t\t\t<textStyle>group</textStyle>\n" );
				if( textStyleFlags & (1 << 14) )
					fprintf( xmlFile, "\t\t\t<textStyle>extend</textStyle>\n" );
				if( textStyleFlags & (1 << 13) )
					fprintf( xmlFile, "\t\t\t<textStyle>condense</textStyle>\n" );
				if( textStyleFlags & (1 << 12) )
					fprintf( xmlFile, "\t\t\t<textStyle>shadow</textStyle>\n" );
				if( textStyleFlags & (1 << 11) )
					fprintf( xmlFile, "\t\t\t<textStyle>outline</textStyle>\n" );
				if( textStyleFlags & (1 << 10) )
					fprintf( xmlFile, "\t\t\t<textStyle>underline</textStyle>\n" );
				if( textStyleFlags & (1 << 9) )
					fprintf( xmlFile, "\t\t\t<textStyle>italic</textStyle>\n" );
				if( textStyleFlags & (1 << 8) )
					fprintf( xmlFile, "\t\t\t<textStyle>bold</textStyle>\n" );
				
				int16_t	textHeight = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +28 ));
				if( !isButton )
					fprintf( xmlFile, "\t\t\t<textHeight>%d</textHeight>\n", textHeight );
				
				int x = 0, startOffs = currOffsIntoData +30;
				fprintf( xmlFile, "\t\t\t<name>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n" );
				
				startOffs = x +2;
				fprintf( xmlFile, "\t\t\t<script>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</script>\n" );
				
				fprintf( xmlFile, "\t\t</part>\n" );
				
				currOffsIntoData += partLength;
				currOffsIntoData += (currOffsIntoData % 2);	// Align on even byte.
			}

			for( int n = 0; n < numContents; n++ )
			{
				int16_t		partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				int16_t		partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				
				fprintf( xmlFile, "\t\t<content>\n" );
				
				CBuf		theText, theStyles;
				if( partID < 0 )	// It's a card part's contents:
				{
					partID = -partID;
					fprintf( xmlFile, "\t\t\t<layer>card</layer>\n", partID );
					fprintf( xmlFile, "\t\t\t<id>%d</id>\n", partID );
					
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
					fprintf( xmlFile, "\t\t\t<layer>background</layer>\n" );
					fprintf( xmlFile, "\t\t\t<id>%d</id>\n", partID );
					
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
				
				fprintf( xmlFile, "\t\t\t<text>" );
				size_t	numChars = theText.size();
				for( int x = 0; x < numChars; x++ )
				{
					char currCh = theText[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</text>\n" );
				if( theStyles.size() > 0 )
				{
//					char sfn[256] = { 0 };
//					snprintf( sfn, sizeof(sfn), "style_runs_%d_%d.styl", vBlockID, partID );
//					theStyles.tofile( sfn );
					
					for( size_t x = 0; x < theStyles.size(); )
					{
						int16_t	startOffset = BIG_ENDIAN_16(theStyles.int16at( x ));
						x += sizeof(int16_t);
						int16_t	styleID = BIG_ENDIAN_16(theStyles.int16at( x ));
						x += sizeof(int16_t);
						
						fprintf( xmlFile, "\t\t\t<stylerun>\n" );
						fprintf( xmlFile, "\t\t\t\t<offset>%u</offset>\n", startOffset );
						fprintf( xmlFile, "\t\t\t\t<id>%u</id>\n", styleID );
						fprintf( xmlFile, "\t\t\t</stylerun>\n" );
					}
				}
				
				currOffsIntoData += partLength +4 +(partLength % 2);	// Align on even byte.
				
				fprintf( xmlFile, "\t\t</content>\n" );
			}

			int x = 0, startOffs = currOffsIntoData;
			fprintf( xmlFile, "\t\t<name>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					fprintf( xmlFile, "&lt;" );
				else if( currCh == '>' )
					fprintf( xmlFile, "&gt;" );
				else if( currCh == '&' )
					fprintf( xmlFile, "&amp;" );
				else
					fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
			}
			fprintf( xmlFile, "</name>\n" );
			
			startOffs = x +1;
			fprintf( xmlFile, "\t\t<script>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					fprintf( xmlFile, "&lt;" );
				else if( currCh == '>' )
					fprintf( xmlFile, "&gt;" );
				else if( currCh == '&' )
					fprintf( xmlFile, "&amp;" );
				else
					fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
			}
			fprintf( xmlFile, "</script>\n" );

			fprintf( xmlFile, "\t</card>\n" );
		}
		else if( strcmp(vBlockType,"FTBL") == 0 )
		{
			fprintf( stdout, "Status: Processing '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int16_t	numFonts = BIG_ENDIAN_16(blockData.int16at( 6 ));
			size_t	currOffsIntoData = 8;
			currOffsIntoData += 4;	// Reserved?
			for( int n = 0; n < numFonts; n++ )
			{
				fprintf( xmlFile, "\t<font>\n" );
				int16_t	fontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				fprintf( xmlFile, "\t\t<id>%d</id>\n", fontID );
				
				int x = 0, startOffs = currOffsIntoData +2;
				fprintf( xmlFile, "\t\t<name>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n" );
			
				currOffsIntoData = x +1;
				currOffsIntoData += currOffsIntoData %2;	// Align on even byte.
				
				fprintf( xmlFile, "\t</font>\n" );
			}
		}
		else if( strcmp(vBlockType,"STBL") == 0 )
		{
			fprintf( stdout, "Status: Processing '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			
			char sfn[256] = { 0 };
			snprintf( sfn, sizeof(sfn), "STBL_%d.data", vBlockID );
			blockData.tofile( sfn );
			
			size_t		currOffs = 4;
			int32_t		numStyles = BIG_ENDIAN_32(blockData.int32at( currOffs ));
			currOffs += 4;
			
			for( int s = 0; s < numStyles; s++ )
			{
				currOffs += 16;
				
				fprintf( xmlFile, "\t<styleentry>\n" );
				int16_t	fontID = BIG_ENDIAN_16(blockData.int16at( currOffs ));
				if( fontID != -1 )
					fprintf( xmlFile, "\t\t<font>%d</font>\n", fontID );
				currOffs += 2;
				
				int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffs ));
				currOffs += 2;
				
				if( textStyleFlags == 0 )
					fprintf( xmlFile, "\t\t<textStyle>plain</textStyle>\n" );
				else if( textStyleFlags != -1 )	// -1 means use field style.
				{
					if( textStyleFlags & (1 << 15) )
						fprintf( xmlFile, "\t\t<textStyle>group</textStyle>\n" );
					if( textStyleFlags & (1 << 14) )
						fprintf( xmlFile, "\t\t<textStyle>extend</textStyle>\n" );
					if( textStyleFlags & (1 << 13) )
						fprintf( xmlFile, "\t\t<textStyle>condense</textStyle>\n" );
					if( textStyleFlags & (1 << 12) )
						fprintf( xmlFile, "\t\t<textStyle>shadow</textStyle>\n" );
					if( textStyleFlags & (1 << 11) )
						fprintf( xmlFile, "\t\t<textStyle>outline</textStyle>\n" );
					if( textStyleFlags & (1 << 10) )
						fprintf( xmlFile, "\t\t<textStyle>underline</textStyle>\n" );
					if( textStyleFlags & (1 << 9) )
						fprintf( xmlFile, "\t\t<textStyle>italic</textStyle>\n" );
					if( textStyleFlags & (1 << 8) )
						fprintf( xmlFile, "\t\t<textStyle>bold</textStyle>\n" );
				}
				int16_t	fontSize = BIG_ENDIAN_16(blockData.int16at( currOffs ));
				if( fontSize != -1 )
					fprintf( xmlFile, "\t\t<size>%d</size>\n", fontSize );
				currOffs += 2;
				currOffs += 2;	// 2 more padding?
				
				fprintf( xmlFile, "\t</styleentry>\n" );
			}
		}
		else if( strcmp(vBlockType,"FREE") == 0 )	// Not a free, reusable block?
		{
			fprintf( stderr, "Status: Skipping '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			fprintf( xmlFile, "\t<!-- Skipped '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			theFile.ignore( vBlockSize -12 );	// Skip rest of block data.
		}
		else
		{
			fprintf( stderr, "Warning: Skipping '%4s' #%d (%d bytes)\n", vBlockType, vBlockID, vBlockSize );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			char		fname[256] = { 0 };
			snprintf( fname, sizeof(fname), "%s_%d.data", vBlockType, vBlockID );
			blockData.tofile( fname );
			fprintf( xmlFile, "\t<unknown type=\"%4s\" id=\"%d\" size=\"%d\" file=\"%s\" />\n", vBlockType, vBlockID, vBlockSize, fname );
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
				
				fprintf( stdout, "Status: Converting 'ICON' %d.\n", theID );
				
				picture		theIcon( 32, 32, 1, false );
				theIcon.memcopyin( *currIcon, 0, 4 * 32 );
				
				theIcon.buildmaskfromsurroundings();
				
//				snprintf( fname, sizeof(fname), "ICON_%d_mask.pbm", theID );
//				theIcon.writemasktopbm( fname );
				
				snprintf( fname, sizeof(fname), "ICON_%d.pbm", theID );
				theIcon.writebitmapandmasktopbm( fname );
				
				fprintf( xmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<type>icon</type>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n\t\t<file>ICON_%d.pbm</file>\n\t</media>\n", theID );
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
				
				fprintf( stdout, "Status: Converting 'PICT' %d.\n", theID );

				snprintf( fname, sizeof(fname), "PICT_%d.pict", theID );
				FILE*		theFile = fopen( fname, "w" );
				if( !theFile )
				{
					fprintf( stderr, "Error: Couldn't create file '%s' for 'ICON' %d.\n", fname, theID );
					return false;
				}
				
				for( int n = 0; n < 8; n++ )
					fputs( "BILL_ATKINSON_ERIC_CARLSON_KEVIN_CALHOUN_DANIEL_THOME_HYPERCARD_", theFile );	// 64 bytes repeated 8 times is a neat 512 byte header.
				fwrite( *currPicture, GetHandleSize( currPicture ), 1, theFile );
				fclose( theFile );

				fprintf( xmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<type>picture</type>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n\t\t<file>PICT_%d.pict</file>\n\t</media>\n", theID );
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
				
				fprintf( xmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<type>cursor</type>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n\t\t<file>CURS_%d.pbm</file>\n\t\t<hotspot>\n\t\t\t<left>%d</left>\n\t\t\t<top>%d</top>\n\t\t</hotspot>\n\t</media>\n", theID, horzPos, vertPos );
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
				
				fprintf( xmlFile, "\t<media>\n\t\t<id>%d</id>\n\t\t<name>", theID );
				for( int n = 1; n <= name[0]; n++ )
				{
					char currCh = name[n];
					if( currCh == '<' )
						fprintf( xmlFile, "&lt;" );
					else if( currCh == '>' )
						fprintf( xmlFile, "&gt;" );
					else if( currCh == '&' )
						fprintf( xmlFile, "&amp;" );
					else
						fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
				}
				fprintf( xmlFile, "</name>\n\t\t<file>snd_%d.aiff</file>\n\t\t<type>sound</type>\n\t</media>\n", theID );
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
				
				fprintf( stdout, "Status: Converting AddColor 'HCbg' %d.\n", theID );
				
				snprintf( fname, sizeof(fname), "HCbg_%d.data", theID );

				size_t	dataLen = GetHandleSize( currIcon );
				CBuf	theData( dataLen );
				theData.memcpy( 0, *currIcon, 0, dataLen );
				theData.tofile( fname );
				
				fprintf( xmlFile, "\t<addcolorbackground>\n\t\t<id>%d</id>\n", theID );
				
				size_t	currOffs = 0;
				while( currOffs < dataLen )
				{
					fprintf( xmlFile, "\t\t<addcolorobject>\n" );
					int8_t	currType = theData[currOffs];
					bool	vHidden = currType & (1 << 7);
					currType &= ~(1 << 7);
					currOffs += 1;
					
					switch( currType )
					{
						case 0x01:	// Button
						{
							fprintf( xmlFile, "\t\t\t<type>button</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( xmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( xmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x02:	// Field
						{
							fprintf( xmlFile, "\t\t\t<type>field</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( xmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( xmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x03:	// Rectangle
						{
							fprintf( xmlFile, "\t\t\t<type>rectangle</type>\n" );
							uint16_t	rc = 0, gc = 0, bc = 0;
							int16_t		bevelDepth = 0;
							int16_t		l, t, r, b;
							fprintf( xmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( xmlFile, "\t\t\t</rect>\n" );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<bevel>%d</bevel>\n", t );
							fprintf( xmlFile, "\t\t\t<color>\n" );
							rc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<red>%d</red>\n", rc );
							gc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<green>%d</green>\n", gc );
							bc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<blue>%d</blue>\n", bc );
							fprintf( xmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x04:	// Picture
						{
							fprintf( xmlFile, "\t\t\t<type>picture</type>\n" );
							int16_t		l, t, r, b;
							fprintf( xmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( xmlFile, "\t\t\t</rect>\n" );

							uint8_t	transparentFlag = theData[currOffs];
							currOffs += 1;
							fprintf( xmlFile, "\t\t\t<transparent> %s </transparent>\n", transparentFlag ? "<true />" : "<false />" );
							
							uint8_t	imageNameLen = theData[currOffs];
							currOffs += 1;
							
							fprintf( xmlFile, "\t\t\t<name>" );
							for( int n = 0; n < imageNameLen; n++ )
							{
								char currCh = theData[currOffs++];
								if( currCh == '<' )
									fprintf( xmlFile, "&lt;" );
								else if( currCh == '>' )
									fprintf( xmlFile, "&gt;" );
								else if( currCh == '&' )
									fprintf( xmlFile, "&amp;" );
								else
									fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
							}
							fprintf( xmlFile, "</name>\n" );
							break;
						}
						
						default:
							fprintf( xmlFile, "\t\t\t<!-- Unknown type %x, aborting 'HCcd' resource %d. -->\n", currType, theID );
							fprintf( stderr, "Error: Unknown type %x, aborting 'HCcd' resource %d.\n", currType, theID );
							currOffs = dataLen;	// Can't read more. Skip the rest.
							break;
					}
					fprintf( xmlFile, "\t\t\t<visible> %s </visible>\n", (vHidden ? "<false />" : "<true />") );
					fprintf( xmlFile, "\t\t</addcolorobject>\n" );
				}
			
				fprintf( xmlFile, "\t</addcolorbackground>\n" );
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
				
				fprintf( stdout, "Status: Converting AddColor 'HCcd' %d.\n", theID );

				snprintf( fname, sizeof(fname), "HCcd_%d.data", theID );
				
				size_t	dataLen = GetHandleSize( currIcon );
				CBuf	theData( dataLen );
				theData.memcpy( 0, *currIcon, 0, dataLen );
				//theData.tofile( fname );
				
				fprintf( xmlFile, "\t<addcolorcard>\n\t\t<id>%d</id>\n", theID );
				
				size_t	currOffs = 0;
				while( currOffs < dataLen )
				{
					fprintf( xmlFile, "\t\t<addcolorobject>\n" );
					int8_t	currType = theData[currOffs];
					bool	vHidden = currType & (1 << 7);
					currType &= ~(1 << 7);
					currOffs += 1;
					
					switch( currType )
					{
						case 0x01:	// Button
						{
							fprintf( xmlFile, "\t\t\t<type>button</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( xmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( xmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x02:	// Field
						{
							fprintf( xmlFile, "\t\t\t<type>field</type>\n" );
							int16_t		buttonID = 0, bevelDepth = 0;
							uint16_t	r = 0, g = 0, b = 0;
							buttonID = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<id>%d</id>\n", buttonID );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( xmlFile, "\t\t\t<color>\n" );
							r = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<red>%d</red>\n", r );
							g = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<green>%d</green>\n", g );
							b = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<blue>%d</blue>\n", b );
							fprintf( xmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x03:	// Rectangle
						{
							fprintf( xmlFile, "\t\t\t<type>rectangle</type>\n" );
							uint16_t	rc = 0, gc = 0, bc = 0;
							int16_t		bevelDepth = 0;
							int16_t		l, t, r, b;
							fprintf( xmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( xmlFile, "\t\t\t</rect>\n" );
							bevelDepth = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t<bevel>%d</bevel>\n", bevelDepth );
							fprintf( xmlFile, "\t\t\t<color>\n" );
							rc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<red>%d</red>\n", rc );
							gc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<green>%d</green>\n", gc );
							bc = BIG_ENDIAN_16(theData.uint16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<blue>%d</blue>\n", bc );
							fprintf( xmlFile, "\t\t\t</color>\n" );
							break;
						}

						case 0x04:	// Picture
						{
							fprintf( xmlFile, "\t\t\t<type>picture</type>\n" );
							int16_t		l, t, r, b;
							fprintf( xmlFile, "\t\t\t<rect>\n" );
							t = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<top>%d</top>\n", t );
							l = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<left>%d</left>\n", l );
							b = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<bottom>%d</bottom>\n", b );
							r = BIG_ENDIAN_16(theData.int16at( currOffs ));
							currOffs += 2;
							fprintf( xmlFile, "\t\t\t\t<right>%d</right>\n", r );
							fprintf( xmlFile, "\t\t\t</rect>\n" );

							uint8_t	transparentFlag = theData[currOffs];
							currOffs += 1;
							fprintf( xmlFile, "\t\t\t<transparent> %s </transparent>\n", transparentFlag ? "<true />" : "<false />" );
							
							uint8_t	imageNameLen = theData[currOffs];
							currOffs += 1;
							
							fprintf( xmlFile, "\t\t\t<name>" );
							for( int n = 0; n < imageNameLen; n++ )
							{
								char currCh = theData[currOffs++];
								if( currCh == '<' )
									fprintf( xmlFile, "&lt;" );
								else if( currCh == '>' )
									fprintf( xmlFile, "&gt;" );
								else if( currCh == '&' )
									fprintf( xmlFile, "&amp;" );
								else
									fprintf( xmlFile, "%s", UniCharFromMacRoman(currCh) );
							}
							fprintf( xmlFile, "</name>\n" );
							break;
						}
						
						default:
							fprintf( xmlFile, "\t\t\t<!-- Unknown type %x, aborting 'HCcd' resource %d. -->\n", currType, theID );
							fprintf( stderr, "Error: Unknown type %x, aborting 'HCcd' resource %d.\n", currType, theID );
							currOffs = dataLen;	// Can't read more. Skip the rest.
							break;
					}
					fprintf( xmlFile, "\t\t\t<visible> %s </visible>\n", (vHidden ? "<false />" : "<true />") );
					fprintf( xmlFile, "\t\t</addcolorobject>\n" );
				}
			
				fprintf( xmlFile, "\t</addcolorcard>\n" );
			}

			CloseResFile( resRefNum );
		}
	}
	else
		fprintf( stderr, "Error: Error %d locating input file's resource fork.\n", (int)err );
	#endif // MAC_CODE
	
	fprintf( xmlFile, "</stackfile>\n" );
	if( xmlFile != stdout )
		fclose( xmlFile );
	
	#if MAC_CODE
	if( err != fnfErr && err != noErr )
	{
		fprintf( stderr, "Error: During conversion of Macintosh fork of stack.\n" );
		return false;
	}
	#endif // MAC_CODE
	
	return true;
}
