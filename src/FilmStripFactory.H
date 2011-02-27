/*  FilmStripFactory.H
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

#ifndef _FILM_STRIP_FACTORY_H_
#define _FILM_STRIP_FACTORY_H_

#include "global_includes.H"
#include "JobManager.H"

namespace nle
{

class FilmStripFactory;
class FilmStrip;
class IVideoFile;

extern FilmStripFactory* g_filmStripFactory;

typedef struct s_film_strip_node {
	struct s_film_strip_node* next;
	FilmStrip* strip;
	string filename;
	int ref_count;
} film_strip_node;


class FilmStripFactory : public JobDoneListener
{
	public:
		FilmStripFactory();
		~FilmStripFactory();
		FilmStrip* get( IVideoFile* vf );
		void ref( IVideoFile* vf );
		void unref( IVideoFile* vf );
		void jobDone( Job* job_description );
	private:
		film_strip_node* m_strips;
		
};
} /* namespace nle */

#endif /* _FILM_STRIP_FACTORY_H_ */
