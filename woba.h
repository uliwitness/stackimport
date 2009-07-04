/*

 WOBA Decoder for C++
 (c) 2005 Jonathan Bettencourt / Kreative Korporation

 This decodes the compressed bitmap format that HyperCard uses to store card images.
 The format is called WOBA, which stands for Wrath Of Bill Atkinson, because it was
 written by Bill Atkinson and we had a heck of a time figuring it out.


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

#ifndef _____WOBA_H_____
#define _____WOBA_H_____

#include "picture.h"
using namespace std;

inline int __min(int x, int y);

void xornstr(char * dest, char * src, int n);
void xorstr(char * dest, char * src);
void shiftnstr(char * s, int n, int sh);
void shiftstr(char * s, int sh);

void woba_decode(picture & p, char * woba);

#endif
