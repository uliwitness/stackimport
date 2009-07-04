/*
 *  CStackFile.h
 *  stackimport
 *
 *  Created by Uli Kusterer on 06.10.06.
 *  Copyright 2006 M. Uli Kusterer. All rights reserved.
 *
 */

#include <deque>
#include <map>
#include <stdint.h>


class CStackFile
{
protected:
	
public:
	bool	LoadFile( const std::string& fpath );
};