/*
 *  CStackFile.cpp
 *  stackimport
 *
 *  Created by Uli Kusterer on 06.10.06.
 *  Copyright 2006 M. Uli Kusterer. All rights reserved.
 *
 */

#include "CStackFile.h"
#include <iostream>
#include <fstream>
#include "picture.h"
#include "woba.h"
#include "EndianStuff.h"


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
		return false;
	
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
			printf( "<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			picture		thePicture;
			char*		pictureData = new char[vBlockSize -12];
			theFile.read( pictureData, vBlockSize -12 );
			woba_decode( thePicture, pictureData );
			
			char		fname[256],
						fname2[256];
			sprintf( fname, "BMAP_%u.pbm", vBlockID );
			thePicture.writebitmaptopbm( fname );
			sprintf( fname2, "BMAP_%u_mask.pbm", vBlockID );
			thePicture.writemasktopbm( fname2 );
			delete [] pictureData;
			printf( "<bitmap>\n" );
			printf( "\t<id>%d</id>\n", vBlockID );
			printf( "\t<size>%d</size>\n", vBlockSize -12 );
			printf( "\t<image>%s</image>\n", fname );
			printf( "\t<mask>%s</mask>\n", fname2 );
			printf( "</bitmap>\n" );
		}
		else if( strcmp(vBlockType,"STAK") == 0 )
		{
			printf( "<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			printf( "<stack>\n" );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int16_t	userLevel = BIG_ENDIAN_16(blockData.int16at( 60 ));
			printf( "\t<userLevel>%d</userLevel>\n", userLevel );
			int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 64 ));
			printf( "\t<cantModify> %s </cantModify>\n", (flags & (1 << 15)) ? "<true />" : "<false />" );
			printf( "\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
			printf( "\t<privateAccess> %s </privateAccess>\n", (flags & (1 << 13)) ? "<true />" : "<false />" );
			printf( "\t<cantAbort> %s </cantAbort>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
			printf( "\t<cantPeek> %s </cantPeek>\n", (flags & (1 << 10)) ? "<true />" : "<false />" );
			char		versStr[16] = { 0 };
			int32_t	version0 = blockData.int32at( 84 );
			NumVersionToStr( (char*) &version0, versStr );
			printf( "\t<version>%s</version>\n", versStr );
			int32_t	version1 = blockData.int32at( 88 );
			NumVersionToStr( (char*) &version1, versStr );
			printf( "\t<version>%s</version>\n", versStr );
			int32_t	version2 = blockData.int32at( 92 );
			NumVersionToStr( (char*) &version2, versStr );
			printf( "\t<version>%s</version>\n", versStr );
			int32_t	version3 = blockData.int32at( 96 );
			NumVersionToStr( (char*) &version3, versStr );
			printf( "\t<version>%s</version>\n", versStr );
			int16_t	height = BIG_ENDIAN_16(blockData.int16at( 428 ));
			if( height == 0 )
				height = 342;
			int16_t	width = BIG_ENDIAN_16(blockData.int16at( 430 ));
			if( width == 0 )
				width = 512;
			printf( "\t<cardSize>\n\t\t<width>%d</width>\n\t\t<height>%d</height>\n\t</cardSize>\n", width, height );

			char			pattern[8] = { 0 };
			int				offs = 0x2b4;
//			printf("----------\n");
			for( int x = 0; x < 40; x++ )
			{
				memmove( pattern, blockData.buf( offs, 8 ), 8 );
				char		fname[256] = { 0 };
				sprintf( fname, "PAT_%u.pbm", x +1 );
				picture		thePicture( 8, 8, 1, false );
				thePicture.memcopyin( pattern, 0, 8 );
				thePicture.writebitmaptopbm( fname );
				offs += 8;
				printf("\t<pattern>PAT_%u.pbm</pattern>\n", x+1);
//				for( int y = 0; y < 8; y++ )
//				{
//					if( pattern[y] & (1 << 0) ) printf( "|X" ); else printf("| ");
//					if( pattern[y] & (1 << 1) ) printf( "X" ); else printf(" ");
//					if( pattern[y] & (1 << 2) ) printf( "X" ); else printf(" ");
//					if( pattern[y] & (1 << 3) ) printf( "X" ); else printf(" ");
//					if( pattern[y] & (1 << 4) ) printf( "X" ); else printf(" ");
//					if( pattern[y] & (1 << 5) ) printf( "X" ); else printf(" ");
//					if( pattern[y] & (1 << 6) ) printf( "X" ); else printf(" ");
//					if( pattern[y] & (1 << 7) ) printf( "X|\n" ); else printf(" |\n");
//				}
//				printf("----------\n");
			}
			int x = 0, startOffs = 0x5f4;
			printf( "\t<script>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					printf( "&lt;" );
				else if( currCh == '>' )
					printf( "&gt;" );
				else
					printf( "%c", currCh );
			}
			printf( "</script>\n" );
			printf( "</stack>\n" );
		}
		else if( strcmp(vBlockType,"BKGD") == 0 )
		{
			printf( "<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			printf( "<background>\n" );
			printf( "\t<id>%d</id>\n", vBlockID );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int32_t	bitmapID = BIG_ENDIAN_32(blockData.int32at( 4 ));
			printf( "\t<bitmap>%d</bitmap>\n", bitmapID );
			int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 0x0C -4 ));
			printf( "\t<cantDelete> %s g</cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
			printf( "\t<showPict> %s </showPict>\n", (flags & (1 << 13)) ? "<false />" : "<true />" );	// showPict is stored reversed.
			printf( "\t<dontSearch> %s </dontSearch>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
			int16_t	numParts = BIG_ENDIAN_16(blockData.int16at( 0x18 ));
			int16_t	numContents = BIG_ENDIAN_16(blockData.int16at( 0x20 ));
			printf( "\t<parts count=%d>\n", numParts );
			size_t	currOffsIntoData = 38;
			for( int x = 0; x < numParts; x++ )
			{
				int16_t	partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				
				printf( "\t\t<part>\n" );
				int16_t	partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				printf( "\t\t\t<id>%d</id>\n", partID );
				int16_t	flagsAndType = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +4 ));
				int16_t	partType = flagsAndType >> 8;
				printf( "\t\t\t<type>%s</type>\n", partType == 1 ? "button" : "field" );
				printf( "\t\t\t<visible> %s </visible>\n", (flagsAndType & (1 << 7)) ? "<false />" : "<true />" );
				printf( "\t\t\t<dontWrap> %s </dontWrap>\n", (flagsAndType & (1 << 5)) ? "<true />" : "<false />" );
				printf( "\t\t\t<dontSearch> %s </dontSearch>\n", (flagsAndType & (1 << 4)) ? "<true />" : "<false />" );
				printf( "\t\t\t<sharedText> %s </sharedText>\n", (flagsAndType & (1 << 3)) ? "<true />" : "<false />" );
				printf( "\t\t\t<fixedLineHeight> %s </fixedLineHeight>\n", (flagsAndType & (1 << 2)) ? "<false />" : "<true />" );
				printf( "\t\t\t<autoTab> %s </autoTab>\n", (flagsAndType & (1 << 1)) ? "<true />" : "<false />" );
				printf( "\t\t\t<enabled> %s </enabled>\n", (flagsAndType & (1 << 0)) ? "<false />" : "<true />" );		// +++ WTF???
				printf( "\t\t\t<lockText> %s </lockText>\n", (flagsAndType & (1 << 0)) ? "<true />" : "<false />" );	// +++ WTF???
				printf( "\t\t\t<rect>\n\t\t\t\t<left>%d</left>\n\t\t\t\t<top>%d</top>\n\t\t\t\t<right>%d</right>\n\t\t\t\t<bottom>%d</bottom>\n\t\t\t</rect>\n", BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +8 )),
							BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +6 )), BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +12 )),
							BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +10 )) );
				int16_t	moreFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +14 ));
				int8_t	styleFromLowNibble = moreFlags & 15;
				printf( "\t\t\t<style>%d</style>\n", styleFromLowNibble );
				moreFlags = moreFlags >> 8;
				printf( "\t\t\t<showName> %s </showName>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				printf( "\t\t\t<autoSelect> %s </autoSelect>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				printf( "\t\t\t<highlight> %s </highlight>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				printf( "\t\t\t<showLines> %s </showLines>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				printf( "\t\t\t<autoHighlight> %s </autoHighlight>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
				printf( "\t\t\t<wideMargins> %s </wideMargins>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
				printf( "\t\t\t<sharedHighlight> %s </sharedHighlight>\n", (moreFlags & (1 << 4)) ? "<false />" : "<true />" );
				printf( "\t\t\t<multipleLines> %s </multipleLines>\n", (moreFlags & (1 << 4)) ? "<true />" : "<false />" );
				int8_t	family = moreFlags & 15;
				printf( "\t\t\t<family>%d</family>\n", family );
				int16_t	titleWidth = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +16 ));
				printf( "\t\t\t<titleWidth>%d</titleWidth>\n", titleWidth );
				printf( "\t\t\t<lastSelectedLine>%d</lastSelectedLine>\n", titleWidth );
				int16_t	iconID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +18 ));
				printf( "\t\t\t<icon>%d</icon>\n", iconID );
				printf( "\t\t\t<selectedLine>%d</selectedLine>\n", iconID );
				int16_t	textAlign = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +20 ));
				printf( "\t\t\t<textAlign>%d</textAlign>\n", textAlign );
				int16_t	textFontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +22 ));
				printf( "\t\t\t<textFontID>%d</textFontID>\n", textFontID );
				int16_t	textSize = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +24 ));
				printf( "\t\t\t<textSize>%d</textSize>\n", textSize );
				int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +26 ));
				printf( "\t\t\t<styles>\n" );
				if( textStyleFlags & (1 << 15) )
					printf( "\t\t\t\t<style>group</style>\n" );
				if( textStyleFlags & (1 << 14) )
					printf( "\t\t\t\t<style>extend</style>\n" );
				if( textStyleFlags & (1 << 13) )
					printf( "\t\t\t\t<style>condense</style>\n" );
				if( textStyleFlags & (1 << 12) )
					printf( "\t\t\t\t<style>shadow</style>\n" );
				if( textStyleFlags & (1 << 11) )
					printf( "\t\t\t\t<style>outline</style>\n" );
				if( textStyleFlags & (1 << 10) )
					printf( "\t\t\t\t<style>underline</style>\n" );
				if( textStyleFlags & (1 << 9) )
					printf( "\t\t\t\t<style>italic</style>\n" );
				if( textStyleFlags & (1 << 8) )
					printf( "\t\t\t\t<style>bold</style>\n" );
				
				printf( "\t\t\t</styles>\n" );
				int16_t	textHeight = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +28 ));
				printf( "\t\t\t<textHeight>%d</textHeight>\n", textHeight );
				
				int x = 0, startOffs = currOffsIntoData +30;
				printf( "\t\t\t<name>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						printf( "&lt;" );
					else if( currCh == '>' )
						printf( "&gt;" );
					else
						printf( "%c", currCh );
				}
				printf( "</name>\n" );
				
				startOffs = x +2;
				printf( "\t\t\t<script>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						printf( "&lt;" );
					else if( currCh == '>' )
						printf( "&gt;" );
					else
						printf( "%c", currCh );
				}
				printf( "</script>\n" );
				
				printf( "\t\t</part>\n" );
				
				currOffsIntoData += partLength;
				currOffsIntoData += (currOffsIntoData % 2);	// Align on even byte.
			}
			printf( "\t</parts>\n" );

			printf( "\t<contents count=%d>\n", numContents );
			for( int x = 0; x < numContents; x++ )
			{
				int16_t		partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				int16_t		partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				printf( "\t\t<content>\n" );
				
				CBuf		theText, theStyles;
				if( partID < 0 )	// It's a card part's contents:
				{
					partID = -partID;
					printf( "\t\t\t<layer>card</layer>\n", partID );
					printf( "\t\t\t<id>%d</id>\n", partID );
					
					int16_t	stylesLength = blockData.int16at( currOffsIntoData +4 );
					if( stylesLength < 0 )
					{
						stylesLength = -stylesLength;
						theStyles.resize( stylesLength );
						theStyles.memcpy( 0, blockData, currOffsIntoData +5, partLength -stylesLength );
					}
					else
						stylesLength = 0;
					theText.resize( partLength -stylesLength );
					theText.memcpy( 0, blockData, currOffsIntoData +5 +stylesLength, partLength -1 -stylesLength );
					theText[theText.size()-1] = 0;
				}
				else	// It's a bg part's contents:
				{
					printf( "\t\t\t<layer>background</layer>\n", partID );
					printf( "\t\t\t<id>%d</id>\n", partID );
					
					int16_t	stylesLength = blockData.int16at( currOffsIntoData +4 );
					if( stylesLength < 0 )
					{
						stylesLength = -stylesLength;
						theStyles.resize( stylesLength );
						theStyles.memcpy( 0, blockData, currOffsIntoData +5, partLength -stylesLength );
					}
					else
						stylesLength = 0;
					theText.resize( partLength -stylesLength );
					theText.memcpy( 0, blockData, currOffsIntoData +5 +stylesLength, partLength -1 -stylesLength );
					theText[theText.size()-1] = 0;
				}
				
				printf( "\t\t\t<text>%s</text>\n", theText.buf() );
				if( theStyles.size() > 0 )
					printf( "\t\t\t<style-runs>%s</style-runs>\n", theStyles.buf() );
				
				currOffsIntoData += partLength +4 +(partLength % 2);	// Align on even byte.
				
				printf( "\t\t</content>\n" );
			}
			printf( "\t</contents>\n" );
			
			int x = 0, startOffs = currOffsIntoData;
			printf( "\t<name>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					printf( "&lt;" );
				else if( currCh == '>' )
					printf( "&gt;" );
				else
					printf( "%c", currCh );
			}
			printf( "</name>\n" );
			
			startOffs = x +1;
			printf( "\t<script>" );
			for( x = startOffs; blockData[x] != 0; x++ )
			{
				char currCh = blockData[x];
				if( currCh == '<' )
					printf( "&lt;" );
				else if( currCh == '>' )
					printf( "&gt;" );
				else
					printf( "%c", currCh );
			}
			printf( "</script>\n" );

			printf( "</background>\n" );
		}
		else if( strcmp(vBlockType,"CARD") == 0 )
		{
			printf( "<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			printf( "<card>\n" );
			printf( "\t<id>%d</id>\n", vBlockID );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int32_t	bitmapID = BIG_ENDIAN_32(blockData.int32at( 4 ));
			printf( "\t<bitmap>%d</bitmap>\n", bitmapID );
			int16_t	flags = BIG_ENDIAN_16(blockData.int16at( 0x0C -4 ));
			printf( "\t<cantDelete> %s </cantDelete>\n", (flags & (1 << 14)) ? "<true />" : "<false />" );
			printf( "\t<showPict> %s </showPict>\n", (flags & (1 << 13)) ? "<false />" : "<true />" );	// showPict is stored reversed.
			printf( "\t<dontSearch> %s </dontSearch>\n", (flags & (1 << 11)) ? "<true />" : "<false />" );
			int16_t	owner = BIG_ENDIAN_16(blockData.int16at( 0x1E -4 ));
			printf( "\t<owner>%d</owner>\n", owner );
			int16_t	numParts = BIG_ENDIAN_16(blockData.int16at( 0x20 -4 ));
			int16_t	numContents = BIG_ENDIAN_16(blockData.int16at( 0x28 -4 ));
			printf( "\t<parts count=%d>\n", numParts );
			size_t	currOffsIntoData = 0x2E -4;
			for( int x = 0; x < numParts; x++ )
			{
				int16_t	partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				
				printf( "\t\t<part>\n" );
				int16_t	partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				printf( "\t\t\t<id>%d</id>\n", partID );
				int16_t	flagsAndType = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +4 ));
				int16_t	partType = flagsAndType >> 8;
				printf( "\t\t\t<type>%s</type>\n", partType == 1 ? "button" : "field" );
				printf( "\t\t\t<visible> %s </visible>\n", (flagsAndType & (1 << 7)) ? "<false />" : "<true />" );
				printf( "\t\t\t<dontWrap> %s </dontWrap>\n", (flagsAndType & (1 << 5)) ? "<true />" : "<false />" );
				printf( "\t\t\t<dontSearch> %s </dontSearch>\n", (flagsAndType & (1 << 4)) ? "<true />" : "<false />" );
				printf( "\t\t\t<sharedText> %s </sharedText>\n", (flagsAndType & (1 << 3)) ? "<true />" : "<false />" );
				printf( "\t\t\t<fixedLineHeight> %s </fixedLineHeight>\n", (flagsAndType & (1 << 2)) ? "<false />" : "<true />" );
				printf( "\t\t\t<autoTab> %s </autoTab>\n", (flagsAndType & (1 << 1)) ? "<true />" : "<false />" );
				printf( "\t\t\t<enabled> %s </enabled>\n", (flagsAndType & (1 << 0)) ? "<false />" : "<true />" );		// +++ WTF???
				printf( "\t\t\t<lockText> %s </lockText>\n", (flagsAndType & (1 << 0)) ? "<true />" : "<false />" );	// +++ WTF???
				printf( "\t\t\t<rect>\n\t\t\t\t<left>%d</left>\n\t\t\t\t<top>%d</top>\n\t\t\t\t<right>%d</right>\n\t\t\t\t<bottom>%d</bottom>\n\t\t\t</rect>\n", BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +8 )),
							BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +6 )), BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +12 )),
							BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +10 )) );
				int16_t	moreFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +14 ));
				int8_t	styleFromLowNibble = moreFlags & 15;
				printf( "\t\t\t<style>%d</style>\n", styleFromLowNibble );
				moreFlags = moreFlags >> 8;
				printf( "\t\t\t<showName> %s </showName>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				printf( "\t\t\t<autoSelect> %s </autoSelect>\n", (moreFlags & (1 << 7)) ? "<true />" : "<false />" );
				printf( "\t\t\t<highlight> %s </highlight>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				printf( "\t\t\t<showLines> %s </showLines>\n", (moreFlags & (1 << 6)) ? "<true />" : "<false />" );
				printf( "\t\t\t<autoHighlight> %s </autoHighlight>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
				printf( "\t\t\t<wideMargins> %s </wideMargins>\n", (moreFlags & (1 << 5)) ? "<true />" : "<false />" );
				printf( "\t\t\t<sharedHighlight> %s </sharedHighlight>\n", (moreFlags & (1 << 4)) ? "<false />" : "<true />" );
				printf( "\t\t\t<multipleLines> %s </multipleLines>\n", (moreFlags & (1 << 4)) ? "<true />" : "<false />" );
				int8_t	family = moreFlags & 15;
				printf( "\t\t\t<family>%d</family>\n", family );
				int16_t	titleWidth = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +16 ));
				printf( "\t\t\t<titleWidth>%d</titleWidth>\n", titleWidth );
				printf( "\t\t\t<lastSelectedLine>%d</lastSelectedLine>\n", titleWidth );
				int16_t	iconID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +18 ));
				printf( "\t\t\t<icon>%d</icon>\n", iconID );
				printf( "\t\t\t<selectedLine>%d</selectedLine>\n", iconID );
				int16_t	textAlign = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +20 ));
				printf( "\t\t\t<textAlign>%d</textAlign>\n", textAlign );
				int16_t	textFontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +22 ));
				printf( "\t\t\t<textFontID>%d</textFontID>\n", textFontID );
				int16_t	textSize = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +24 ));
				printf( "\t\t\t<textSize>%d</textSize>\n", textSize );
				int16_t	textStyleFlags = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +26 ));
				printf( "\t\t\t<styles>\n" );
				if( textStyleFlags & (1 << 15) )
					printf( "\t\t\t\t<style>group</style>\n" );
				if( textStyleFlags & (1 << 14) )
					printf( "\t\t\t\t<style>extend</style>\n" );
				if( textStyleFlags & (1 << 13) )
					printf( "\t\t\t\t<style>condense</style>\n" );
				if( textStyleFlags & (1 << 12) )
					printf( "\t\t\t\t<style>shadow</style>\n" );
				if( textStyleFlags & (1 << 11) )
					printf( "\t\t\t\t<style>outline</style>\n" );
				if( textStyleFlags & (1 << 10) )
					printf( "\t\t\t\t<style>underline</style>\n" );
				if( textStyleFlags & (1 << 9) )
					printf( "\t\t\t\t<style>italic</style>\n" );
				if( textStyleFlags & (1 << 8) )
					printf( "\t\t\t\t<style>bold</style>\n" );
				
				printf( "\t\t\t</styles>\n" );
				int16_t	textHeight = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +28 ));
				printf( "\t\t\t<textHeight>%d</textHeight>\n", textHeight );
				
				int x = 0, startOffs = currOffsIntoData +30;
				printf( "\t\t\t<name>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						printf( "&lt;" );
					else if( currCh == '>' )
						printf( "&gt;" );
					else
						printf( "%c", currCh );
				}
				printf( "</name>\n" );
				
				startOffs = x +2;
				printf( "\t\t\t<script>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						printf( "&lt;" );
					else if( currCh == '>' )
						printf( "&gt;" );
					else
						printf( "%c", currCh );
				}
				printf( "</script>\n" );
				
				printf( "\t\t</part>\n" );
				
				currOffsIntoData += partLength;
				currOffsIntoData += (currOffsIntoData % 2);	// Align on even byte.
			}
			printf( "\t</parts>\n" );

			printf( "\t<contents count=%d>\n", numContents );
			for( int x = 0; x < numContents; x++ )
			{
				int16_t		partID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				int16_t		partLength = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData +2 ));
				printf( "\t\t<content>\n" );
				
				CBuf		theText, theStyles;
				if( partID < 0 )	// It's a card part's contents:
				{
					partID = -partID;
					printf( "\t\t\t<layer>card</layer>\n", partID );
					printf( "\t\t\t<id>%d</id>\n", partID );
					
					int16_t	stylesLength = blockData.int16at( currOffsIntoData +4 );
					if( stylesLength < 0 )
					{
						stylesLength = -stylesLength;
						theStyles.resize( stylesLength );
						theStyles.memcpy( 0, blockData, currOffsIntoData +5, partLength -stylesLength );
					}
					else
						stylesLength = 0;
					theText.resize( partLength -stylesLength );
					theText.memcpy( 0, blockData, currOffsIntoData +5 +stylesLength, partLength -1 -stylesLength );
					theText[theText.size()-1] = 0;
				}
				else	// It's a bg part's contents:
				{
					printf( "\t\t\t<layer>background</layer>\n", partID );
					printf( "\t\t\t<id>%d</id>\n", partID );
					
					int16_t	stylesLength = blockData.int16at( currOffsIntoData +4 );
					if( stylesLength < 0 )
					{
						stylesLength = -stylesLength;
						theStyles.resize( stylesLength );
						theStyles.memcpy( 0, blockData, currOffsIntoData +5, partLength -stylesLength );
					}
					else
						stylesLength = 0;
					theText.resize( partLength -stylesLength );
					theText.memcpy( 0, blockData, currOffsIntoData +5 +stylesLength, partLength -1 -stylesLength );
					theText[theText.size()-1] = 0;
				}
				
				printf( "\t\t\t<text>%s</text>\n", theText.buf() );
				if( theStyles.size() > 0 )
					printf( "\t\t\t<style-runs>%s</style-runs>\n", theStyles.buf() );
				
				currOffsIntoData += partLength +4 +(partLength % 2);	// Align on even byte.
				
				printf( "\t\t</content>\n" );
			}
			printf( "\t</contents>\n" );

			printf( "</card>\n" );
		}
		else if( strcmp(vBlockType,"FTBL") == 0 )
		{
			printf( "<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			CBuf		blockData( vBlockSize -12 );
			theFile.read( blockData.buf(0,vBlockSize -12), vBlockSize -12 );
			int16_t	numFonts = BIG_ENDIAN_16(blockData.int16at( 6 ));
			printf( "<fonts count=%d>\n", numFonts );
			size_t	currOffsIntoData = 8;
			currOffsIntoData += 4;	// Reserved?
			for( int x = 0; x < numFonts; x++ )
			{
				printf( "\t<font>\n" );
				int16_t	fontID = BIG_ENDIAN_16(blockData.int16at( currOffsIntoData ));
				printf( "\t\t<id>%d</id>\n", fontID );
				
				int x = 0, startOffs = currOffsIntoData +2;
				printf( "\t\t<name>" );
				for( x = startOffs; blockData[x] != 0; x++ )
				{
					char currCh = blockData[x];
					if( currCh == '<' )
						printf( "&lt;" );
					else if( currCh == '>' )
						printf( "&gt;" );
					else
						printf( "%c", currCh );
				}
				printf( "</name>\n" );
			
				currOffsIntoData = x +1;
				currOffsIntoData += currOffsIntoData %2;	// Align on even byte.
				
				printf( "\t</font>\n" );
			}
			printf( "</fonts>\n" );
		}
		else
		{
			printf( "<!-- '%4s' #%d (%d bytes) -->\n", vBlockType, vBlockID, vBlockSize );
			theFile.ignore( vBlockSize -12 );	// Skip rest of block data.
		}
	}
	
	return true;
}
