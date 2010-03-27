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

#ifndef _____PICTURE_H_____
#define _____PICTURE_H_____

#include <cstring>
#include <fstream>
using namespace std;

class CBuf;

int __bitmap_row_width(int width, int height, int depth);
int __bitmap_size(int width, int height, int depth);
unsigned int __pow2(int p);
unsigned int __pow21(int p);

class picture
{
	public:
	
	picture(void);
	picture(int w, int h, int d, bool greymask);
	~picture(void);
	void reinit(int w, int h, int d, bool greymask);
	
	int gwidth(void);
	int gheight(void);
	int gdepth(void);
	int gmaskdepth(void);
	int bitmapsize(void);
	int masksize(void);
	
	int coordbyteoffset(int, int);
	unsigned int coordbitmask(int, int);
	int maskcoordbyteoffset(int, int);
	unsigned int maskcoordbitmask(int, int);
	
	void memcopyin(const char *, int, int);
	void memcopyin(const char *, int, int, int);
	void maskmemcopyin(const char *, int, int);
	void maskmemcopyin(const char *, int, int, int);
	
	void memcopyout(char * dest, int start, int count);
	void memcopyout(char * dest, int x, int y, int count);
	void maskmemcopyout(char * dest, int start, int count);
	void maskmemcopyout(char * dest, int x, int y, int count);

	void memcopyout(CBuf& dest, int start, int count);
	void memcopyout(CBuf& dest, int x, int y, int count);
	void maskmemcopyout(CBuf& dest, int start, int count);
	void maskmemcopyout(CBuf& dest, int x, int y, int count);
	
	void memfill(char, int, int);
	void memfill(char, int, int, int);
	void maskmemfill(char, int, int);
	void maskmemfill(char, int, int, int);
	
	void buildmaskfromsurroundings();
	void scanstartingatpixel( int x, int y );
	
	void copyrow(int, int);
	void maskcopyrow(int, int);
	
	unsigned int fixcolor(unsigned int);
	unsigned int dupcolor(unsigned int);
	
	unsigned int getpixel(int, int);
	void setpixel(int, int, int);
	unsigned int maskgetpixel(int, int);
	void masksetpixel(int, int, int);
	
	void __directcopybmptomask(void);
	
	void bwrite(fstream);
	void bread(fstream);
	void writefile(char *);
	void writebitmapandmasktopbm(char * fn);
	void writebitmaptopbm(char * fn);
	void writemasktopbm(char * fn);

	void readfile(char *);
	
	private:
	
	int width;
	int height;
	int depth;
	bool greyscalemask;
	
	int rowlength;
	int maskrowlength;
	int bitmaplength;
	char * bitmap;
	int masklength;
	char * mask;
};

#endif
