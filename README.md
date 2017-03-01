stackimport
===========

A command line tool that reads a HyperCard stack and generates a folder with XML and PBM files from it containing
a more easily readable representation of its contents. Based on Tyler Vano's and Rebecca Bettencourt's code from
http://creysoft.com/xtalk/


How to build this
-----------------

There's an Xcode project that creates a command line tool that you can point at a HyperCard stack and it'll extract
all the information in it into a folder of image files, XML files and sound files.

The code for converting 'snd ' resources (sampled sound) to AIFF files currently requires QuickTime, meaning this
tool will only compile on a Mac, and only for 32 bit CPUs (The 32-bit tool will run fine on a 64-bit Mac, of course).

If you are trying to build it on a platform other than Mac, change the MAC_CODE define in CStackFile.h to 0 and
take all the .cpp files and .h files and compile them into a command line tool named 'stackimport'. For full
functionality, you'll also have to write code to replace the Mac-specific code (which extracts resources from the
resource fork of the stack and converts 'snd ' resources to AIFF -- the resource-fork-reading code in
https://github.com/uliwitness/reclassicfication may be a good starting point).

For example, on GNU/Linux-based systems such as Debian, the following has been known to work:

    g++ -o stackimport woba.cpp Tests.cpp picture.cpp main.cpp CStackFile.cpp CBuf.cpp byteutils.cpp -std=gnu++11


How to use this
---------------

Once you have built the stackimport command line tool, just use it. Syntax is

    stackimport [--dumprawblocks] [--nostatus] [--noprogress] <originalStackPath>

where originalStackPath is the HyperCard stack you want to convert.

Where the options are:

dumprawblocks - Create files containing the raw data from each block in the stack,
don't just create the clear-text output. This is useful if you want to extract some
information this program doesn't yet recognize. It'd be appreciated if you could contribute
any additional knowledge you have about HyperCard's file format back to the authors of this.

nostatus - Don't output status messages while converting the file. These are mostly useful
if you are displaying a progress UI, or if conversion aborts and you need to know what bock
caused the error.

noprogress - Do not show 'Progress 1 of 5' etc. messages. These messages are useful for
updating the progress bar in a progress UI.


License
-------

    Copyright (c) 2005,2006 and 2010 Rebecca Bettencourt, Uli Kusterer (Mr. Z)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
