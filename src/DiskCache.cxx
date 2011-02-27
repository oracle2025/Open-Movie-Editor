/*  DiskCache.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <cstdio>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>

#include "DiskCache.H"
#include "globals.H"
#include "helper.H"

#define BUFFER_LEN 1024

namespace nle
{

DiskCache::DiskCache( string filename, string type )
{
	struct stat statbuf;
	struct stat statbuf2;
	char buffer[BUFFER_LEN];
	findpath( filename.c_str(), buffer, BUFFER_LEN );
	string cachepath = string(g_homefolder) + ("/.openme/cache" PREF_FILE_ADD)+ buffer;
	m_cachefile = string(g_homefolder) + ("/.openme/cache" PREF_FILE_ADD) + filename + "." + type;
	m_file = fopen( m_cachefile.c_str(), "r" );
	m_size = -1;
	m_dirty = false;
	if ( m_file ) {
		m_empty = false;
		int r = fstat( fileno( m_file ), &statbuf );
		assert( r != -1 );
		m_size = statbuf.st_size;
		r = stat( filename.c_str(), &statbuf2 );
		if ( r == -1 ) {
			cout << "filename: " << filename << endl;
			cout << strerror( errno ) << endl;
		}
		assert( r != -1 );
		if ( statbuf2.st_mtime > statbuf.st_mtime ) {
			fclose( m_file );
			unlink( m_cachefile.c_str() );
			m_file = fopen( m_cachefile.c_str(), "w" );
			m_dirty = true;
			m_empty = true;
		}
	} else {
		m_dirty = true;
		m_empty = true;
		mkdirp( cachepath.c_str() );
		m_file = fopen( m_cachefile.c_str(), "w" );
		assert( m_file );
	}
}

DiskCache::~DiskCache()
{
	if ( m_file ) {
		fclose( m_file );
		m_file = 0;
	}
	if ( m_dirty ) {
		unlink( m_cachefile.c_str() );
	}
}

bool DiskCache::isEmpty()
{
	return m_empty;
}

int DiskCache::read( void* buffer, int count )
{
	assert( !m_empty );
	return fread( buffer, 1, count, m_file );
}

void DiskCache::write( void* buffer, int count )
{
	assert( m_empty );
	fwrite( buffer, 1, count, m_file );
}

	
} /* namespace nle */


