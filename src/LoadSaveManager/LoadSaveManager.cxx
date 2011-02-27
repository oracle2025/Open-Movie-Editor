/*  LoadSaveManager.cxx
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

#include "config.h"

#include <algorithm>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>


#include <FL/filename.H>
#include <FL/Fl_Choice.H>

#include <tinyxml.h>

#include "LoadSaveManager.H"
#include "Prefs.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include "globals.H"
#include "SaveAsDialog.H"
#include "TimelineView.H"
#include "nle.h"

namespace nle
{
static string name_to_filename( string name );
static string name_from_projectfile( string filename );

static string home( getenv( "HOME" ) );
LoadSaveManager* g_loadSaveManager;

static void save_timeout( void* )
{
	if ( g_timeline->changed() ) {
		g_loadSaveManager->save();
		g_timeline->saving();
	}
	Fl::repeat_timeout( 10.0, save_timeout );
}

string LoadSaveManager::nodups( string name )
{
	if ( m_projectChoice->find_item( name.c_str() ) ) {
		int i = 1;
		stringstream o;
		string a;
		o << name << " " << i;
		a = o.str();
		while (  m_projectChoice->find_item( a.c_str() ) ) {
			o.str("");
			i++;
			o << name << " " << i;
			a = o.str();
		}
		name = a;
	}
	return name;
}

LoadSaveManager::LoadSaveManager( Fl_Choice* projectChoice, Fl_Button* projectInput )
	: m_projectChoice( projectChoice ), m_projectInput( projectInput )
{
	g_loadSaveManager = this;
	int count = 0;
	dirent** list;
	m_video_projects = home + ("/Video Projects" PREF_FILE_ADD);
	if ( fl_filename_isdir( m_video_projects.c_str() ) ) {
		count = scandir( m_video_projects.c_str(), &list, 0, alphasort );
		m_projectChoice->clear();
		for ( int i = 0; i < count; i++ ) {
			if ( !fl_filename_isdir( string( m_video_projects + "/" + list[i]->d_name ).c_str() ) ) {
				string name = name_from_projectfile( m_video_projects + "/" + list[i]->d_name );
				if ( name != "" ) {
					name = nodups( name );
					char* cname = strdup( list[i]->d_name );
					char* n = strdup( name.c_str() );
					if ( strlen(n) >= 1024 ) {
						n[1023] = '\0';
					}
					m_projectChoice->add( n, 0, 0, cname );
					free(n);
				}
			}
		}
		for ( int i = count; i > 0; ) { // This is some bad ass hacking style from the fltk manual ;)
			free( (void*)( list[--i] ) );
		}
		free( (void*)list );
	} else {
		mkdir( m_video_projects.c_str(), 0755 );
	}
	g_timeline->saving();
	Fl::add_timeout( 10.0, save_timeout );
}
LoadSaveManager::~LoadSaveManager()
{
}
static string name_from_projectfile( string filename )
{
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) {
		return "";
	}
	TiXmlHandle docH( &doc );
	TiXmlText *name = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "name" ).FirstChild().Text();
	if ( name ) {
		return name->Value();
	}
	return "";
}
static string name_to_filename( string name )
{
	char random_end[7];
	string filename = name;
	char* buffer = strdup( filename.c_str() );
	for ( int i = 0; i < 6; i++ ) {
		random_end[i] = '0' + rand() % 10;
	}
	random_end[6] = '\0';
	
	for ( char* p = buffer; *p != '\0'; p++ ) {
		*p = tolower( *p );
		if ( !( ( *p >= 'a' && *p <= 'z' ) || ( *p >= '0' && *p <= '9' ) ) ) {
			*p = '_';
		}
	}
	filename = buffer;
	filename += "_";
	filename += random_end;
	filename += ".vproj";
	free( buffer );
	return filename;
}
void LoadSaveManager::startup()
{
	m_currentName = g_preferences->lastProject();
	if ( m_currentName == "" ) {
		m_currentName = nodups("New Project");
		m_currentFilename = name_to_filename( m_currentName );
		m_projectChoice->add( m_currentName.c_str(), 0, 0, strdup( m_currentFilename.c_str() ) );
	} else {
		const Fl_Menu_Item* item;
		item = m_projectChoice->find_item( m_currentName.c_str() );
		if ( item ) {
			m_currentFilename = (char*)item->user_data();
			g_timeline->read( m_video_projects + "/" + m_currentFilename );
		} else {
			m_currentName = nodups("New Project");
			m_currentFilename = name_to_filename( m_currentName );
			m_projectChoice->add( m_currentName.c_str(), 0, 0, strdup( m_currentFilename.c_str() ) );
		}
	}
	
	/* vv  Put this in a function */
	m_projectInput->label( m_currentName.c_str() );
	g_ui->fps_pb_menu->value( g_timeline->m_playback_fps );
	if ( g_ui->fps_pb_menu->mvalue() ) {
		((Fl_Menu_Item*)g_ui->fps_pb_menu->mvalue())->setonly();
	}
	const Fl_Menu_Item* item;
	item = m_projectChoice->find_item( m_currentName.c_str() );
	if ( item ) {
		m_projectChoice->value( item );
	}
}
void LoadSaveManager::shutdown()
{
	g_timeline->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	g_preferences->lastProject( m_currentName );
	// free all strings from m_projectChoice;
	Fl_Menu_Item* item = (Fl_Menu_Item*)m_projectChoice->menu();
	int i = 0;
	int m = m_projectChoice->size() - 1;
	while ( item && i < m ) {
		if ( item->user_data() ) {
			char* buffer = (char*)item->user_data();
			item->user_data( 0 );
			free( buffer );
		}
		item++;
		i++;
	}
	g_timeline->clear();
}
void LoadSaveManager::save()
{
	g_timeline->write( m_video_projects + "/" + m_currentFilename, m_currentName );
}
void LoadSaveManager::newProject()
{
	g_timeline->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	m_currentName = "New Project";
	m_currentFilename = name_to_filename( m_currentName );
	g_timeline->clear();
	{
		VideoTrack *vt;
		AudioTrack *at;
		vt = new VideoTrack( g_timeline, 0 );
		g_timeline->addTrack( vt );
		vt = new VideoTrack( g_timeline, 1 );
		g_timeline->addTrack( vt );
		at = new AudioTrack( g_timeline, 2 );
		g_timeline->addTrack( at );
		at = new AudioTrack( g_timeline, 3 );
		g_timeline->addTrack( at );
	}
	m_currentName = nodups( m_currentName );
	
	char* cname = strdup( m_currentFilename.c_str() );
	m_projectChoice->add( m_currentName.c_str(), 0, 0, cname );
	m_projectInput->label( m_currentName.c_str() );
	const Fl_Menu_Item* item;
	item = m_projectChoice->find_item( m_currentName.c_str() );
	if ( item ) {
		m_projectChoice->value( item );
	}
	g_timelineView->redraw();
}
void LoadSaveManager::saveAs()
{
	g_timeline->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	SaveAsDialog dlg;
	dlg.show();
	while ( dlg.shown() ) {
		Fl::wait();
	}
	if ( dlg.ok() ) {
		m_currentFilename = name_to_filename( dlg.projectName() );
		m_currentName = dlg.projectName();
		g_timeline->write( m_video_projects + "/" + m_currentFilename, m_currentName );
		char* cname = strdup( m_currentFilename.c_str() );
		m_projectChoice->add( m_currentName.c_str(), 0, 0, cname );
		m_projectInput->label( m_currentName.c_str() );
		const Fl_Menu_Item* item;
		item = m_projectChoice->find_item( m_currentName.c_str() );
		if ( item ) {
			m_projectChoice->value( item );
		}
	}
}
void LoadSaveManager::name( string v )
{
	v = nodups( v );
	string new_file = name_to_filename( v );
	if ( access( string( m_video_projects + "/" + m_currentFilename ).c_str(), R_OK | W_OK ) == 0 ) {
		rename( string( m_video_projects + "/" + m_currentFilename ).c_str(), string( m_video_projects + "/" + new_file).c_str() );
	}
	m_currentFilename = new_file;
	m_currentName = v;
	int n = m_projectChoice->value();
	m_projectChoice->replace( n, m_currentName.c_str() );
	m_projectChoice->redraw();
}
void LoadSaveManager::load( string v )
{
	g_timelineView->clear_selection();
	g_timeline->write( m_video_projects + "/" + m_currentFilename, m_currentName );
	m_currentFilename = v;
	m_currentName = m_projectChoice->mvalue()->text;
	g_timeline->read( m_video_projects + "/" + m_currentFilename );
	const Fl_Menu_Item* item;
	m_projectInput->label( m_currentName.c_str() );
	g_ui->fps_pb_menu->value( g_timeline->m_playback_fps );
	if ( g_ui->fps_pb_menu->mvalue() ) {
		((Fl_Menu_Item*)g_ui->fps_pb_menu->mvalue())->setonly();
	}
	item = m_projectChoice->find_item( m_currentName.c_str() );
	if ( item ) {
		m_projectChoice->value( item );
	}
}


} /* namespace nle */
