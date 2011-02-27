/*  DiskCache.H
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

#ifndef _DISK_CACHE_H_
#define _DISK_CACHE_H_

#include "global_includes.H"
namespace nle
{

class DiskCache
{
	public:
		DiskCache( string filename, string type );
		~DiskCache();
		bool isEmpty();
		void size( int64_t s ) { m_size = s; }
		inline int64_t size() { return m_size; }
		int read( void* buffer, int count );
		void write( void* buffer, int count );
		inline void clean() { m_dirty = false; }
	private:
		bool m_empty;
		int64_t m_size;
		FILE* m_file;
		bool m_dirty;
		string m_cachefile;
};
} /* namespace nle */

#endif /* _DISC_CACHE_H_ */
