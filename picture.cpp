/*

 Picture Class for C++
 (c) 2005 Rebecca Bettencourt / Kreative Korporation

 This is a class that lets us manipulate bitmapped graphics in memory.


 This code is under the MIT license.

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <cstring>
#include <fstream>
#include "picture.h"
#include "woba.h"
#include "CBuf.h"

using namespace std;

int __bitmap_row_width(int width, int height, int depth)
{
	return ( ((width * depth) / 8) + ( ((width * depth) % 8)?1:0 ) );
}

int __bitmap_size(int width, int height, int depth)
{
	return __bitmap_row_width(width,height,depth) * height;
}

unsigned int __pow2(int p)
{
	/* finds 2^p */
	/* also returns bit p set */
	int i;
	unsigned int m = 1;
	for (i=p; i>=0; i--) {
		m += m;
	}
	return m;
}

unsigned int __pow21(int p)
{
	/* finds 2^p - 1 */
	/* also returns bits 0 - p-1 set */
	/* or the least significant p bits set */
	int i;
	unsigned int m = 0;
	for (i=p; i>=0; i--) {
		m += m+1;
	}
	return m;
}

picture::picture(void)
{
	width = 0;
	height = 0;
	depth = 0;
	greyscalemask = false;
	rowlength = 0;
	maskrowlength = 0;
	bitmaplength = 0;
	masklength = 0;
}

picture::picture(int w, int h, int d, bool greymask)
{
	width = w;
	height = h;
	depth = d;
	greyscalemask = greymask;
	rowlength = __bitmap_row_width(w,h,d);
	maskrowlength = __bitmap_row_width(w,h,greymask?8:1);
	bitmaplength = __bitmap_size(w,h,d);
	masklength = __bitmap_size(w,h,greymask?8:1);
	bitmap = new char[bitmaplength];
	mask = new char[masklength];
	memset(bitmap,0,bitmaplength);
	memset(mask,0xFF,masklength);
}

picture::~picture(void)
{
	if (bitmaplength) {
		delete [] bitmap;
	}
	if (masklength) {
		delete [] mask;
	}
}

void picture::reinit(int w, int h, int d, bool greymask)
{
	if (bitmaplength) {
		delete [] bitmap;
	}
	if (masklength) {
		delete [] mask;
	}
	width = w;
	height = h;
	depth = d;
	greyscalemask = greymask;
	rowlength = __bitmap_row_width(w,h,d);
	maskrowlength = __bitmap_row_width(w,h,greymask?8:1);
	bitmaplength = __bitmap_size(w,h,d);
	masklength = __bitmap_size(w,h,greymask?8:1);
	bitmap = new char[bitmaplength];
	mask = new char[masklength];
	memset(bitmap,0,bitmaplength);
	memset(mask,0xFF,masklength);
}

int picture::gwidth(void) { return width; }
int picture::gheight(void) { return height; }
int picture::gdepth(void) { return depth; }
int picture::gmaskdepth(void) { return greyscalemask?8:1; }
int picture::bitmapsize(void) { return bitmaplength; }
int picture::masksize(void) { return masklength; }

int picture::coordbyteoffset(int x, int y)
{
	return (rowlength * y) + ((x * depth) / 8);
}

unsigned int picture::coordbitmask( int x, int y )
{
	int				i;
	unsigned int	m = __pow21(depth);
	if( depth < 8 )
	{
		switch (depth)
		{
			case 1:
				i= 7-(x%8);
				break;
			case 2:
				i=2*(3-(x%4));
				break;
			case 4:
				i=4*(1-(x%2));
				break;
		}
		m <<= i;
	}
	return m;
}

int picture::maskcoordbyteoffset(int x, int y)
{
	return (maskrowlength * y) + (greyscalemask?x:(x/8));
}

unsigned int picture::maskcoordbitmask(int x, int y)
{
	unsigned int m = greyscalemask ? 0xFF : 1;
	if( !greyscalemask )
		m <<= 7 -(x % 8);
	return m;
}



void picture::memcopyin(const char * src, int start, int count)
{
	memcpy(bitmap + start, src, count);
}

void picture::memcopyin(const char * src, int x, int y, int count)
{
	memcpy(bitmap + coordbyteoffset(x,y), src, count);
}

void picture::maskmemcopyin(const char * src, int start, int count)
{
	memcpy(mask + start, src, count);
}

void picture::maskmemcopyin(const char * src, int x, int y, int count)
{
	memcpy(mask + maskcoordbyteoffset(x,y), src, count);
}



void picture::memcopyout(char * dest, int start, int count)
{
	memcpy(dest, bitmap + start, count);
}

void picture::memcopyout(char * dest, int x, int y, int count)
{
	memcpy(dest, bitmap + coordbyteoffset(x,y), count);
}

void picture::maskmemcopyout(char * dest, int start, int count)
{
	memcpy(dest, mask + start, count);
}

void picture::maskmemcopyout(char * dest, int x, int y, int count)
{
	memcpy(dest, mask + maskcoordbyteoffset(x,y), count);
}



void picture::memcopyout(CBuf& dest, int start, int count)
{
	memcpy(dest.buf(0,count), bitmap + start, count);
}

void picture::memcopyout(CBuf& dest, int x, int y, int count)
{
	memcpy(dest.buf(0,count), bitmap + coordbyteoffset(x,y), count);
}

void picture::maskmemcopyout(CBuf& dest, int start, int count)
{
	memcpy(dest.buf(0,count), mask + start, count);
}

void picture::maskmemcopyout(CBuf& dest, int x, int y, int count)
{
	memcpy(dest.buf(0,count), mask + maskcoordbyteoffset(x,y), count);
}



void picture::memfill(char ch, int start, int count)
{
	memset(bitmap + start, ch, count);
}

void picture::memfill(char ch, int x, int y, int count)
{
	memset(bitmap + coordbyteoffset(x,y), ch, count);
}

void picture::maskmemfill(char ch, int start, int count)
{
	memset(mask + start, ch, count);
}

void picture::maskmemfill(char ch, int x, int y, int count)
{
	memset(mask + maskcoordbyteoffset(x,y), ch, count);
}


void picture::buildmaskfromsurroundings()
{
	maskmemfill( 0xFF, 0, rowlength * height );		// All black.
	
	int x = 0, y = 0;
	for( x = 0; x < width; x++ )
		scanstartingatpixel( x, y );
	
	y = height -1;
	for( x = 0; x < width; x++ )
		scanstartingatpixel( x, y );
	
	x = 0;
	for( y = 0; y < height; y++ )
		scanstartingatpixel( x, y );
	
	x = width -1;
	for( y = 0; y < height; y++ )
		scanstartingatpixel( x, y );
}


void picture::scanstartingatpixel( int x, int y )
{
	// We've not fallen off the edge, have we?
	if( x < 0 || y < 0 || x > width || y > height )
		return;
	
	// This is a white pixel and the mask for it is still opaque?
	//	i.e. we haven't processed it yet?
	if( 0 == getpixel( x, y ) && 1 == maskgetpixel( x, y ) )
	{
		masksetpixel( x, y, 0x00 );	// Punch hole in the mask.
		
		// Scan surrounding pixels:
		scanstartingatpixel( x -1, y );
		scanstartingatpixel( x +1, y );
		scanstartingatpixel( x, y -1 );
		scanstartingatpixel( x, y +1 );
	}
}


void picture::copyrow(int dest, int src)
{
	memcpy(bitmap + coordbyteoffset(0,dest),
	       bitmap + coordbyteoffset(0,src),
	       rowlength);
}

void picture::maskcopyrow(int dest, int src)
{
	memcpy(mask + maskcoordbyteoffset(0,dest),
	       mask + maskcoordbyteoffset(0,src),
	       maskrowlength);
}

unsigned int picture::fixcolor(unsigned int c)
{
	return (depth >= 32)?c:(c & __pow21(depth));
}

unsigned int picture::dupcolor(unsigned int c)
{
	int i;
	unsigned int f,p,d;
	
	if (depth >= 32) { return c; }
	
	f = (c & __pow21(depth));
	p = __pow2(depth);
	d = 0;
	
	for (i=32/depth; i>=0; i--) {
		d *= p;
		d += f;
	}
	return d;
}

unsigned int picture::getpixel(int x, int y)
{
	unsigned int p,p1;
	int i = coordbyteoffset(x,y);
	int j;
	if (depth < 8) {
		p = __pow2(depth);
		p1 = p-1;
		i = bitmap[i] & coordbitmask(x,y);
		while (i && (!(i & p1))) { i /= p; }
		return i;
	} else if (depth == 8) {
		return (unsigned char)bitmap[i];
	} else {
		j = depth / 8;
		p1 = 0;
		while (j) {
			p1 = (p1*256)+(unsigned char)bitmap[i];
			i++;
			j--;
		}
		return p1;
	}
}

void picture::setpixel(int x, int y, int c)
{
	int i = coordbyteoffset(x,y);
	unsigned int d = dupcolor(c);
	unsigned int f = fixcolor(c);
	int j;
	if (depth < 8) {
		bitmap[i] = ( (bitmap[i] & (~coordbitmask(x,y))) | (d & coordbitmask(x,y)) );
	} else if (depth == 8) {
		bitmap[i] = f;
	} else {
		j = depth / 8;
		while (j) {
			j--;
			bitmap[i+j] = (f & 0xFF);
			f /= 256;
		}
	}
}

unsigned int picture::maskgetpixel(int x, int y)
{
	int i = maskcoordbyteoffset(x,y);
	if (greyscalemask) {
		return (unsigned char)mask[i];
	} else {
		i = mask[i] & maskcoordbitmask(x,y);
		return i ? 1 : 0;
	}
}

void picture::masksetpixel(int x, int y, int c)
{
	int i = maskcoordbyteoffset(x,y);
	unsigned int d = dupcolor(c);
	if (greyscalemask) {
		mask[i] = (d & 0xFF);
	} else {
		int		bitpos = maskcoordbitmask(x,y);
		mask[i] = ( (mask[i] & (~bitpos)) | (d & bitpos) );
	}
}

void picture::__directcopybmptomask(void)
{
	memcpy(mask, bitmap, masklength);
}

void picture::bwrite(fstream fp)
{
	int stuff[8];
	char * buf;
	stuff[0] = 0x12AAB175;
	stuff[1] = 0;
	stuff[2] = width;
	stuff[3] = height;
	stuff[4] = depth;
	stuff[5] = greyscalemask?8:1;
	stuff[6] = bitmaplength;
	stuff[7] = masklength;
	buf = new char[32+bitmaplength+masklength];
	memcpy(buf, (char *)stuff, 32);
	memcpy(buf+32, bitmap, bitmaplength);
	memcpy(buf+32+bitmaplength, mask, masklength);
	fp.write(buf, 32+bitmaplength+masklength);
	delete [] buf;
	/* fp.write(reinterpret_cast<char *>(stuff), 32); */
	/* fp.write(bitmap, bitmaplength); */
	/* fp.write(mask, masklength); */
}

void picture::bread(fstream fp)
{
	int stuff[8];
	fp.read(reinterpret_cast<char *>(stuff), 32);
	if (stuff[0] == 0x12AAB175) {
		if (bitmaplength) { delete [] bitmap; }
		if (masklength) { delete [] mask; }
		width = stuff[2];
		height = stuff[3];
		depth = stuff[4];
		greyscalemask = (stuff[5] == 8);
		bitmaplength = stuff[6];
		masklength = stuff[7];
		bitmap = new char[bitmaplength];
		mask = new char[masklength];
		fp.read(bitmap, bitmaplength);
		fp.read(mask, masklength);
		
		rowlength = __bitmap_row_width(width,height,depth);
		maskrowlength = __bitmap_row_width(width,height,greyscalemask?8:1);
	}
}

void picture::writefile(char * fn)
{
	fstream fp(fn, ios::out|ios::binary|ios::app);
	int stuff[8];
	char * buf;
	stuff[0] = 0x12AAB175;
	stuff[1] = 0;
	stuff[2] = width;
	stuff[3] = height;
	stuff[4] = depth;
	stuff[5] = greyscalemask?8:1;
	stuff[6] = bitmaplength;
	stuff[7] = masklength;
	buf = new char[32+bitmaplength+masklength];
	memcpy(buf, (char *)stuff, 32);
	memcpy(buf+32, bitmap, bitmaplength);
	memcpy(buf+32+bitmaplength, mask, masklength);
	fp.write(buf, 32+bitmaplength+masklength);
	delete [] buf;
	/* fp.write(reinterpret_cast<char *>(stuff), 32); */
	/* fp.write(bitmap, bitmaplength); */
	/* fp.write(mask, masklength); */
	fp.close();
}


void picture::writebitmapandmasktopbm(char * fn)
{
	fstream fp(fn, ios::out|ios::binary|ios::trunc);
	char		str[256];
	
	// Write PBM Header:
	snprintf( str, sizeof(str), "P4\n%d %d\n", width, height );
	fp.write( str, strlen(str) );
	
	fp.write( bitmap, bitmaplength );
	
	// Write second PBM Header:
	snprintf( str, sizeof(str), "\nP4\n%d %d\n", width, height );
	fp.write( str, strlen(str) );
	
	fp.write( mask, masklength );
	fp.close();
}


void picture::writebitmaptopbm(char * fn)
{
	fstream fp(fn, ios::out|ios::binary|ios::trunc);
	char		str[256];
	
	// Write PBM Header:
	snprintf( str, sizeof(str), "P4\n%d %d\n", width, height );
	fp.write( str, strlen(str) );
	
	fp.write( bitmap, bitmaplength );
	fp.close();
}


void picture::writemasktopbm(char * fn)
{
	fstream fp(fn, ios::out|ios::binary|ios::trunc);
	char		str[256];
	
	// Write PBM Header:
	snprintf( str, sizeof(str), "P4\n%d %d\n", width, height );
	fp.write( str, strlen(str) );
	
	fp.write( mask, masklength );
	fp.close();
}


void picture::readfile(char * fn)
{
	fstream fp(fn, ios::binary|ios::in);
	int stuff[8];
	fp.read(reinterpret_cast<char *>(stuff), 32);
	if (stuff[0] == 0x12AAB175) {
		if (bitmaplength) { delete [] bitmap; }
		if (masklength) { delete [] mask; }
		width = stuff[2];
		height = stuff[3];
		depth = stuff[4];
		greyscalemask = (stuff[5] == 8);
		bitmaplength = stuff[6];
		masklength = stuff[7];
		bitmap = new char[bitmaplength];
		mask = new char[masklength];
		fp.read(bitmap, bitmaplength);
		fp.read(mask, masklength);
		
		rowlength = __bitmap_row_width(width,height,depth);
		maskrowlength = __bitmap_row_width(width,height,greyscalemask?8:1);
	}
	fp.close();
}
