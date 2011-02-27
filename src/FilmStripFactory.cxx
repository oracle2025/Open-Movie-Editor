/*  FilmStripFactory.cxx
 *
 *  Copyright (C) 2006, 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#include <cassert>

#include "sl/sl.h"

#include "FilmStripFactory.H"
#include "IVideoFile.H"
#include "FilmStrip.H"

#include <cstring>


namespace nle
{

FilmStripFactory* g_filmStripFactory;
	
FilmStripFactory::FilmStripFactory()
{
	g_filmStripFactory = this;
	m_strips = 0;
}
FilmStripFactory::~FilmStripFactory()
{
	g_filmStripFactory = 0;
	assert( m_strips == 0 );
}

static int find_strip_helper( void* p, void* data )
{
	film_strip_node* node = (film_strip_node*)p;
	const char* filename = (const char*)data;
	if ( strcmp( node->filename.c_str(), filename ) == 0 ) {
		return 1;
	}
	return 0;
}
FilmStrip* FilmStripFactory::get( IVideoFile* vf )
{
	film_strip_node* node = (film_strip_node*)sl_map( m_strips, find_strip_helper, (void*)( vf->filename().c_str() ) );
	if ( node ) {
		return node->strip;
	}
	return 0;
}
void FilmStripFactory::ref( IVideoFile* vf )
{
	film_strip_node* node = (film_strip_node*)sl_map( m_strips, find_strip_helper, (void*)( vf->filename().c_str() ) );
	if ( node ) {
		node->ref_count++;
		return;
	}
	node = new film_strip_node;
	node->next = 0;
	node->ref_count = 1;
	node->filename = vf->filename();
	node->strip = 0;
	FilmStrip* filmstrip = new FilmStrip( this, vf );
	g_jobManager->submitJob( filmstrip );
	m_strips = (film_strip_node*)sl_push( m_strips, node );
	return;
}
static int remove_strip_helper( void* p, void* data )
{
	film_strip_node* node = (film_strip_node*)p;
	const char* filename = (const char*)data;
	if ( strcmp( node->filename.c_str(), filename ) == 0 ) {
		node->ref_count--;
		if ( node->ref_count == 0 ) {
			return 1;
		}
	}
	return 0;
}
void FilmStripFactory::unref( IVideoFile* vf )
{
	film_strip_node* node = (film_strip_node*)sl_remove( &m_strips, remove_strip_helper, (void*)( vf->filename().c_str() ) );
	if ( node ) {
		if ( node->strip ) {
			delete node->strip;
		}
		delete node;
	}
}
void FilmStripFactory::jobDone( Job* job_description )
{
	FilmStrip* filmstrip = dynamic_cast<FilmStrip*>( job_description );
	assert( filmstrip );
	film_strip_node* node = (film_strip_node*)sl_map( m_strips, find_strip_helper, (void*)( filmstrip->filename() ) );
	if ( node ) {
		node->strip = filmstrip;
	} else {
		delete filmstrip;
	}
}

} /* namespace nle */
