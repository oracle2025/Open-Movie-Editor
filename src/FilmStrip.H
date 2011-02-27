/*  FilmStrip.H
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _FILM_STRIP_H_
#define _FILM_STRIP_H_

#include <stdint.h>
#include <string>

#include "JobManager.H"

namespace nle
{

typedef struct
{
	unsigned char* data;
	int w;
	int h;
} pic_struct;

class IVideoFile;
class DiskCache;

class FilmStrip : public Job
{
	public:
		FilmStrip( JobDoneListener* listener, IVideoFile* vfile );
		~FilmStrip();
		bool process( double& percentage );
		bool done();
		const char* name() { return "Generating Film Strip"; }
		const char* description() { return "Generating the little thumnails for the timeline"; }
		inline int64_t count() { return m_count; }
		inline pic_struct* get_pic( int64_t index )
			{ return index < m_count ? &m_pics[index] : 0; }
		const char* filename();
	private:
		int64_t m_countAll;
		int64_t m_count;
		pic_struct *m_pics;
		IVideoFile* m_vfile;
		DiskCache* m_cache;
		std::string m_filename;
};


} /* namespace nle */

#endif /* _FILM_STRIP_H_ */

