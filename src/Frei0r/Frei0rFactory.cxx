/*  Frei0rFactory.cxx
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

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include <FL/filename.H>

#include "sl/sl.h"
#include "Frei0rFactory.H"
#include "Frei0rFactoryPlugin.H"
#include "IEffectMenu.H"
#include "MainFilterFactory.H"
#include "globals.H"
#include "nle.h"

#define FREI0R_DIR_1 "/usr/lib/frei0r-1/"
#define FREI0R_DIR_2 "/usr/local/lib/frei0r-1/"
#define FREI0R_DIR_3 "/.frei0r-1/lib/"

namespace nle
{


typedef struct _dir_list_node {
	struct _dir_list_node *next;
	string name;
} dir_node;

Frei0rFactory* g_frei0rFactory;
static string home( getenv( "HOME" ) );

Frei0rFactory::Frei0rFactory( IEffectMenu* menu )
{
	m_effects = 0;
	enumerate( home + FREI0R_DIR_3, menu );
	enumerate( FREI0R_DIR_2, menu );
	enumerate( FREI0R_DIR_1, menu );
}

Frei0rFactory::~Frei0rFactory()
{
	effect_node* node;
	while ( ( node = (effect_node*)sl_pop( &m_effects ) ) ) {
		delete node->effect;
		delete node;
	}

}
void Frei0rFactory::enumerate( string folder, IEffectMenu* menu )
{
	//Scan directorys and enumerate Plugins
	int n;
	dirent** list;
	dir_node* folders = 0;
	dir_node* p;
	effect_node* e;
	g_frei0rFactory = this;
	n = scandir( folder.c_str(), &list, 0, alphasort );
	if ( n <= 0 ) {
		return;
	}
	for ( int i = 0; i < n; i++ ) {
		if ( list[i]->d_name[0] != '.' ) {
			p = new dir_node;
			p->next = 0;
			p->name = string( folder ) + list[i]->d_name;
			folders = (dir_node*)sl_push( folders, p );
		}
	}
	for ( int i = n; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
		free( (void*)( list[--i] ) );
	}
	if ( n > 0 ) {
		free( (void*)list );
	}

	while( folders ) {
		p = (dir_node*)sl_pop( &folders );
		if ( fl_filename_isdir( p->name.c_str() ) ) {
			n = scandir( p->name.c_str(), &list, 0, alphasort );
			while( n-- ) {
				if ( list[n]->d_name[0] != '.'  ) {
					p = new dir_node;
					p->next = 0;
					p->name = string( folder ) + list[n]->d_name;
					folders = (dir_node*)sl_push( folders, p );
				}
			}
			for ( int i = n; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
				free( (void*)( list[--i] ) );
			}
			if ( n > 0 ) {
				free( (void*)list );
			}
		} else {
			Frei0rFactoryPlugin* effect = new Frei0rFactoryPlugin( p->name.c_str() );
			if ( effect->ok() ) {
				menu->addEffect( effect );
				e = new effect_node;
				e->next = 0;
				e->effect = effect;
				m_effects = (effect_node*)sl_push( m_effects, e );
				string identifier = "effect:frei0r:";
				identifier += effect->name();
				g_mainFilterFactory->add( identifier.c_str(), effect );
				g_ui->special_clips->add( effect->name(), PL_VIDEO_EFFECT, identifier.c_str() );
			} else { 
				delete effect;
			}
		}
		
	}
}

FilterFactory* Frei0rFactory::get( string name )
{
	effect_node* p;
	for ( p = m_effects; p; p = p->next ) {
		if ( name == p->effect->name() ) {
			return p->effect;
		}
	}
	return 0;
}


} /* namespace nle */
