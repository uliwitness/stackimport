/*

 WOBA Decoder for C++
 (c) 2005 Rebecca Bettencourt / Kreative Korporation

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

#define DEBUGOUTPUT		0


#if DEBUGOUTPUT
#include <iostream>
#endif
#include "picture.h"
#include "woba.h"
#include <arpa/inet.h>
#include "CBuf.h"
#include "byteutils.h"


using namespace std;


inline int __min(int x, int y)
{
	return (x > y) ? y : x;
}



void woba_decode(picture & p, char * woba)
{
	#if DEBUGOUTPUT
	std::cout << "===== NEXT BMAP =====" << endl;
	#endif
	
	int totalRectTop = 0,
		totalRectLeft = 0,
		totalRectBottom = 0,
		totalRectRight = 0; /* total rectangle for whole picture */
	int maskBoundRectTop = 0,
		maskBoundRectLeft = 0,
		maskBoundRectBottom = 0,
		maskBoundRectRight = 0; /* mask bounding rect */
	int pictureBoundRectTop = 0,
		pictureBoundRectLeft = 0,
		pictureBoundRectBottom = 0,
		pictureBoundRectRight = 0; /* picture bounding rect */
	int maskDataLength = 0,
		pictureDataLength = 0; /* mask and picture data length */
	
	int bx = 0, bx8 = 0, x = 0, y = 0;
	int rowwidth = 0, rowwidth8 = 0, height = 0;
	int dx = 0;
	int dy = 0;
	int repeat = 1;
	CBuf patternbuffer(8);
	CBuf buffer1;
	CBuf buffer2;
	int i = 0, j = 0;
	
	int opcode = 0;
	int operand = 0;
	CBuf operandata(256);
	int numberOfZeroBytes = 0, numberOfDataBytes = 0;
	int k = 0;
	
	/*
		-16 0 - block size & type (already stripped)
		-8  8 - block ID & filler (already stripped)
		8  16 - something
		12 24 - total rect
		20 32 - mask rect
		28 40 - picture rect
		36 48 - nothing
		48 56 - length
		52 64 - start of mask (or bitmap if mask length == 0)
	*/
		#define MASK_START	52
		#define INT16_AT(woba,pos)	ntohs(*(u_int16_t*)(woba+pos))
		#define INT32_AT(woba,pos)	ntohl(*(u_int32_t*)(woba+pos))
		
		totalRectTop = INT16_AT(woba,12);
		totalRectLeft = INT16_AT(woba,14);
		totalRectBottom = INT16_AT(woba,16);
		totalRectRight = INT16_AT(woba,18);
		maskBoundRectTop = INT16_AT(woba,20);
		maskBoundRectLeft = INT16_AT(woba,22);
		maskBoundRectBottom = INT16_AT(woba,24);
		maskBoundRectRight = INT16_AT(woba,26);
		pictureBoundRectTop = INT16_AT(woba,28);
		pictureBoundRectLeft = INT16_AT(woba,30);
		pictureBoundRectBottom = INT16_AT(woba,32);
		pictureBoundRectRight = INT16_AT(woba,34);
		maskDataLength = INT32_AT(woba,44);
		pictureDataLength = INT32_AT(woba,48);
		
		#if DEBUGOUTPUT
		std::cout << "Total Rect: " << totalRectLeft << "," << totalRectTop << "," << totalRectRight << "," << totalRectBottom << endl;
		std::cout << "Bitmap Rect: " << pictureBoundRectLeft << "," << pictureBoundRectTop << "," << pictureBoundRectRight << "," << pictureBoundRectBottom << endl;
		std::cout << "Mask Rect: " << maskBoundRectLeft << "," << maskBoundRectTop << "," << maskBoundRectRight << "," << maskBoundRectBottom << endl;
		std::cout << "Bitmap Size: " << pictureDataLength << endl;
		std::cout << "Mask Size: " << maskDataLength << endl;
		#endif
		
		p.reinit( totalRectRight -totalRectLeft, totalRectBottom -totalRectTop, 1, false);
		p.__directcopybmptomask(); /* clear the mask to zero */
		i= MASK_START;
		
		/* decode mask */
		if( maskDataLength )
		{
			bx8 = maskBoundRectLeft & (~ 0x1F);	// Get high 11 bits (mask out low 5 bits).
			bx = bx8 / 8;						// Bitshift by 3?
			x = 0;
			y = maskBoundRectTop;
			rowwidth8 = ( (maskBoundRectRight & 0x1F)?((maskBoundRectRight | 0x1F)+1):maskBoundRectRight ) - (maskBoundRectLeft & (~ 0x1F));
			rowwidth = rowwidth8 / 8;
			height = maskBoundRectBottom - maskBoundRectTop;
			dx = dy = 0;
			repeat = 1;
			
			// Build a 50% grey checkerboard pattern:
			patternbuffer[0] = patternbuffer[2] = patternbuffer[4] = patternbuffer[6] = 170;	// Even rows: 170 == 10101010 binary.
			patternbuffer[1] = patternbuffer[3] = patternbuffer[5] = patternbuffer[7] = 85;		// Odd rows:   85 == 01010101 binary.
			
			// Make both buffers large enough to hold a full row of pixels:
			buffer1.resize(rowwidth);
			buffer2.resize(rowwidth);
			
			j = 0;
			
			#if DEBUGOUTPUT
			std::cout << "DECODE MASK:" << endl;
			std::cout << "BX8: " << bx8 << endl << "BX: " << bx << endl << "X: " << x << endl << "Y: " << y << endl;
			std::cout << "RW8: " << rowwidth8 << endl << "RW: " << rowwidth << endl << "H: " << height << endl;
			#endif
			
			while( j < maskDataLength )
			{
				opcode = (unsigned char)woba[i];
				
				#if DEBUGOUTPUT
				std::cout << "Opcode: " << __hex(opcode) << endl;
				std::cout << "Repeat: " << repeat << endl;
				std::cout << "i: " << i << endl << "j: " << j << endl;
				std::cout << "x: " << x << endl << "y: " << y << endl;
				std::cout << "dx: " << dx << endl << "dy: " << dy << endl;
				#endif
				
				i++; j++;
				if( (opcode & 0x80) == 0 )
				{
					/* zeros followed by data */
					numberOfDataBytes = opcode >> 4;	// nd = number of data bytes?
					numberOfZeroBytes = opcode & 15;	// nz = number of zeroes?
					
					#if DEBUGOUTPUT
					std::cout << "nd: " << numberOfDataBytes << endl << "nz: " << numberOfZeroBytes << endl;
					#endif
					
					if( numberOfDataBytes )
					{
						operandata.memcpy( 0, woba, i, numberOfDataBytes );
						i += numberOfDataBytes; j += numberOfDataBytes;
					}
					
					while( repeat )
					{
						for( k = numberOfZeroBytes; k > 0; k-- )
						{
							buffer1[x] = 0;
							x++;
						}
						buffer1.memcpy( x, operandata, 0, numberOfDataBytes );
						x += numberOfDataBytes;
						repeat--;
					}
					repeat = 1;
				}
				else if( (opcode & 0xE0) == 0xC0 )
				{
					/* opcode & 1F * 8 bytes of data */
					numberOfDataBytes = (opcode & 0x1F) * 8;
					#if DEBUGOUTPUT
					std::cout << "nd: " << numberOfDataBytes << endl;
					#endif
					if (numberOfDataBytes)
					{
						operandata.memcpy( 0, woba, i, numberOfDataBytes );
						i += numberOfDataBytes; j += numberOfDataBytes;
					}
					while( repeat )
					{
						buffer1.memcpy(x, operandata, 0, numberOfDataBytes);
						x += numberOfDataBytes;
						repeat--;
					}
					repeat = 1;
				}
				else if( (opcode & 0xE0) == 0xE0 )
				{
					/* opcode & 1F * 16 bytes of zero */
					numberOfZeroBytes = (opcode & 0x1F)*16;
					#if DEBUGOUTPUT
					std::cout << "nz: " << numberOfZeroBytes << endl;
					#endif
					while( repeat )
					{
						for (k=numberOfZeroBytes; k>0; k--)
						{
							if( x < buffer1.size() )
								buffer1[x] = 0;
							x++;
						}
						repeat--;
					}
					repeat=1;
				}
				
				if( (opcode & 0xE0) == 0xA0 )	// Repeat the next opcode a certain number of times.
				{
					/* repeat opcode */
					repeat = (opcode & 0x1F);
				}
				else
				{
					switch (opcode)
					{
						case 0x80: /* uncompressed data */
							x = 0;
							while( repeat )
							{
								p.maskmemcopyin(woba+i, bx8, y, rowwidth);
								y++;
								repeat--;
							}
							repeat=1;
							i += rowwidth; j += rowwidth;
							break;
						
						case 0x81: /* white row */
							x = 0;
							while( repeat )
							{
								p.maskmemfill(0, bx8, y, rowwidth);
								y++;
								repeat--;
							}
							repeat=1;
							break;
						
						case 0x82: /* black row */
							x = 0;
							while( repeat )
							{
								p.maskmemfill(0xFF, bx8, y, rowwidth);
								y++;
								repeat--;
							}
							repeat=1;
							break;
							
						case 0x83: /* pattern */
							operand = (unsigned char)woba[i];
							#if DEBUGOUTPUT
							std::cout << "patt: " << __hex(operand) << endl;
							#endif
							i++; j++;
							x = 0;
							while( repeat )
							{
								patternbuffer[y & 7] = operand;
								p.maskmemfill(operand, bx8, y, rowwidth);
								y++;
								repeat--;
							}
							repeat=1;
							break;
							
						case 0x84: /* last pattern */
							x = 0;
							while( repeat )
							{
								operand = patternbuffer[y & 7];
								#if DEBUGOUTPUT
								std::cout << "patt: " << __hex(operand) << endl;
								#endif
								p.maskmemfill(operand, bx8, y, rowwidth);
								y++;
								repeat--;
							}
							repeat=1;
							break;
							
						case 0x85: /* previous row */
							x = 0;
							while( repeat )
							{
								p.maskcopyrow(y, y-1);
								y++;
								repeat--;
							}
							repeat=1;
							break;
							
						case 0x86: /* two rows back */
							x = 0;
							while( repeat )
							{
								p.maskcopyrow(y, y-2);
								y++;
								repeat--;
							}
							repeat=1;
							break;
							
						case 0x87: /* three rows back */
							x = 0;
							while( repeat )
							{
								p.maskcopyrow(y, y-3);
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x88:
							dx = 16; dy = 0;
							break;
							
						case 0x89:
							dx = 0; dy = 0;
							break;
							
						case 0x8A:
							dx = 0; dy = 1;
							break;
							
						case 0x8B:
							dx = 0; dy = 2;
							break;
							
						case 0x8C:
							dx = 1; dy = 0;
							break;
							
						case 0x8D:
							dx = 1; dy = 1;
							break;
							
						case 0x8E:
							dx = 2; dy = 2;
							break;
							
						case 0x8F:
							dx = 8; dy = 0;
							break;
							
						default: /* it's not a repeat or a whole row */
							if( x >= rowwidth )
							{
								x = 0;
								if (dx)
								{
									buffer2.memcpy( 0, buffer1, 0, rowwidth );
									for( k = rowwidth8 / dx; k > 0; k-- )
									{
										buffer2.shiftnstr(0, rowwidth, dx);
										buffer1.xornstr(0, buffer2, 0, rowwidth);
									}
								}
								if (dy)
								{
									p.maskmemcopyout(buffer2, bx8, y-dy, rowwidth);
									buffer1.xornstr(0, buffer2, 0, rowwidth);
								}
								p.maskmemcopyin(buffer1.buf(), bx8, y, rowwidth);
								y++;
							}
							break;
					}
				}
			}
			
			buffer1.resize(0);
			buffer2.resize(0);
		}
		else if( maskBoundRectTop | maskBoundRectLeft | maskBoundRectBottom | maskBoundRectRight )
		{
			/* mask is a simple rectangle */
			bx = maskBoundRectLeft / 8;
			x = 0;
			rowwidth = (maskBoundRectRight -maskBoundRectLeft) / 8;
			if( rowwidth > 0 )
			{
				buffer1.resize(rowwidth);
				for( k = bx; x < rowwidth; k++, x++ )
				{
					buffer1[x] = 0xFF;	// was k as index.
				}
				for( k = maskBoundRectTop; k < maskBoundRectBottom; k++ )
				{
					p.maskmemcopyin( buffer1.buf(), 0, k, rowwidth );
				}
				buffer1.resize(0);
			}
		}
		
		/* decode bitmap */
		if( pictureDataLength )
		{
			bx8 = pictureBoundRectLeft & (~ 0x1F);
			bx = bx8/8;
			x = 0;
			y = pictureBoundRectTop;
			rowwidth8 = ( (pictureBoundRectRight & 0x1F)?((pictureBoundRectRight | 0x1F)+1):pictureBoundRectRight ) - (pictureBoundRectLeft & (~ 0x1F));
			rowwidth = rowwidth8/8;
			height = pictureBoundRectBottom - pictureBoundRectTop;
			dx = dy = 0;
			repeat = 1;
			
			// Build a 50% grey pattern, even rows are 10101010 and odd rows are 01010101:
			patternbuffer[0] = patternbuffer[2] = patternbuffer[4] = patternbuffer[6] = 170;
			patternbuffer[1] = patternbuffer[3] = patternbuffer[5] = patternbuffer[7] = 85;
			
			buffer1.resize(rowwidth);
			buffer2.resize(rowwidth);
			j = 0;
			
			#if DEBUGOUTPUT
			std::cout << "DECODE BITMAP:" << endl;
			std::cout << "BX8: " << bx8 << endl << "BX: " << bx << endl << "X: " << x << endl << "Y: " << y << endl;
			std::cout << "RW8: " << rowwidth8 << endl << "RW: " << rowwidth << endl << "H: " << height << endl;
			#endif
			
			while( j < pictureDataLength )
			{
				opcode = (unsigned char)woba[i];
				#if DEBUGOUTPUT
				std::cout << "Opcode: " << __hex(opcode) << endl;
				std::cout << "Repeat: " << repeat << endl;
				std::cout << "i: " << i << endl << "j: " << j << endl;
				std::cout << "x: " << x << endl << "y: " << y << endl;
				std::cout << "dx: " << dx << endl << "dy: " << dy << endl;
				#endif
				i++; j++;
				if( (opcode & 0x80) == 0 )
				{
					/* zeros followed by data */
					numberOfDataBytes = opcode >> 4;
					numberOfZeroBytes = opcode & 15;
					
					#if DEBUGOUTPUT
					std::cout << "nd: " << numberOfDataBytes << endl << "nz: " << numberOfZeroBytes << endl;
					#endif
					
					if( numberOfDataBytes )
					{
						operandata.memcpy( 0, woba,i, numberOfDataBytes );
						i += numberOfDataBytes; j += numberOfDataBytes;
					}
					while( repeat )
					{
						for( k = numberOfZeroBytes; k > 0; k-- )
						{
							buffer1[x] = 0;
							x++;
						}
						buffer1.memcpy( x, operandata, 0, numberOfDataBytes );
						x += numberOfDataBytes;
						repeat--;
					}
					repeat = 1;
				}
				else if( (opcode & 0xE0) == 0xC0 )
				{
					/* opcode & 1F * 8 bytes of data */
					numberOfDataBytes = (opcode & 0x1F) * 8;
					#if DEBUGOUTPUT
					std::cout << "nd: " << numberOfDataBytes << endl;
					#endif
					if( numberOfDataBytes )
					{
						operandata.memcpy( 0, woba, i, numberOfDataBytes );
						i += numberOfDataBytes; j += numberOfDataBytes;
					}
					while( repeat )
					{
						buffer1.memcpy( x, operandata, 0, __min(buffer1.size() -x, numberOfDataBytes) );
						x += numberOfDataBytes;
						repeat--;
					}
					repeat = 1;
				}
				else if( (opcode & 0xE0) == 0xE0 )
				{
					/* opcode & 1F * 16 bytes of zero */
					numberOfZeroBytes = (opcode & 0x1F) * 16;
					#if DEBUGOUTPUT
					std::cout << "nz: " << numberOfZeroBytes << endl;
					#endif
					while( repeat )
					{
						for( k = numberOfZeroBytes; k > 0; k-- )
						{
							buffer1[ __min(x,buffer1.size()-1) ] = 0;
							x++;
						}
						repeat--;
					}
					repeat = 1;
				}
				
				if( (opcode & 0xE0) == 0xA0 )
				{
					/* repeat opcode */
					repeat = (opcode & 0x1F);
				}
				else
				{
					switch (opcode)
					{
						case 0x80: /* uncompressed data */
							x = 0;
							while( repeat )
							{
								p.memcopyin( woba+i, bx8, y, rowwidth );
								y++;
								repeat--;
							}
							repeat = 1;
							i += rowwidth; j += rowwidth;
							break;
							
						case 0x81: /* white row */
							x = 0;
							while( repeat )
							{
								p.memfill( 0, bx8, y, rowwidth );
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x82: /* black row */
							x = 0;
							while( repeat )
							{
								p.memfill( 0xFF, bx8, y, rowwidth );
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x83: /* pattern */
							operand = (unsigned char)woba[i];
							#if DEBUGOUTPUT
							std::cout << "patt: " << __hex(operand) << endl;
							#endif
							i++; j++;
							x = 0;
							while( repeat )
							{
								patternbuffer[y & 7] = operand;
								p.memfill( operand, bx8, y, rowwidth );
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x84: /* last pattern */
							x = 0;
							while( repeat )
							{
								operand = patternbuffer[y & 7];
								#if DEBUGOUTPUT
								std::cout << "patt: " << __hex(operand) << endl;
								#endif
								p.memfill( operand, bx8, y, rowwidth );
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x85: /* previous row */
							x = 0;
							while( repeat )
							{
								p.copyrow( y, y -1 );
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x86: /* two rows back */
							x = 0;
							while( repeat )
							{
								p.copyrow( y, y -2 );
								y++;
								repeat--;
							}
							repeat=1;
							break;
							
						case 0x87: /* three rows back */
							x = 0;
							while( repeat )
							{
								p.copyrow( y, y -3 );
								y++;
								repeat--;
							}
							repeat = 1;
							break;
							
						case 0x88:
							dx = 16; dy = 0;
							break;
							
						case 0x89:
							dx = 0; dy = 0;
							break;
							
						case 0x8A:
							dx = 0; dy = 1;
							break;
							
						case 0x8B:
							dx = 0; dy = 2;
							break;
							
						case 0x8C:
							dx = 1; dy = 0;
							break;
							
						case 0x8D:
							dx = 1; dy = 1;
							break;
							
						case 0x8E:
							dx = 2; dy = 2;
							break;
							
						case 0x8F:
							dx = 8; dy = 0;
							break;
							
						default: /* it's not a repeat or a whole row */
							if( x >= rowwidth )
							{
								x = 0;
								if( dx )
								{
									buffer2.memcpy( 0, buffer1, 0, rowwidth );
									for( k = rowwidth8 / dx; k > 0; k-- )
									{
										buffer2.shiftnstr( 0, rowwidth, dx );
										buffer1.xornstr( 0, buffer2, 0, rowwidth );
									}
								}
								if( dy )
								{
									p.memcopyout( buffer2, bx8, y-dy, rowwidth );
									buffer1.xornstr( 0, buffer2, 0, rowwidth );
								}
								p.memcopyin( buffer1.buf(), bx8, y, rowwidth );
								y++;
							}
							break;
					}
				}
			}
			
			buffer1.resize(0);
			buffer2.resize(0);
		}
		
		if( ! (maskDataLength | maskBoundRectTop | maskBoundRectLeft | maskBoundRectBottom | maskBoundRectRight) )
		{
			/* mask needs to be copied from picture */
			p.__directcopybmptomask();
		}
}
