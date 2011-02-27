/* Prefs.cxx
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

#include <cstdlib>
#include "strlcpy.h"

#include <tinyxml.h>

#include "Prefs.H"
#include "globals.H"
#include "config.h"

namespace nle
{

Prefs* g_preferences;
static string preferences_filename;

Prefs::Prefs()
{
	preferences_filename = "";
	if ( !getenv("HOME") ) {
		return;
	}
	preferences_filename += getenv( "HOME" );
	preferences_filename += ("/.openme.prefs" PREF_FILE_ADD);
	g_preferences = this;
	m_browserFolder = getenv( "HOME" );
	m_mediaFolder = getenv( "HOME" );
	m_lastRenderFilename = "";
	m_lastVideoCodec = 0;
	m_lastAudioCodec = 0;
	m_lastFramesize = 0;
	m_lastFramerate = 0;
	m_window_x = -1;
	m_window_y = -1;
	m_window_w = -1;
	m_window_h = -1;
	TiXmlDocument doc( preferences_filename.c_str() );
	if ( !doc.LoadFile() ) {
		return;
	}

	TiXmlHandle docH( &doc );
	TiXmlText* text = docH.FirstChildElement( "browserFolder" ).Child( 0 ).Text();
	if ( text ) {
		m_browserFolder = text->Value();
	}
	
	text = docH.FirstChildElement( "lastProject" ).Child( 0 ).Text();
	if ( text ) {
		m_lastProject = text->Value();
	} else {
		m_lastProject = "";
	}
	text = docH.FirstChildElement( "mediaFolder" ).Child( 0 ).Text();
	if ( text ) {
		m_mediaFolder = text->Value();
	}
	text = docH.FirstChildElement( "lastRenderFilename" ).Child( 0 ).Text();
	if ( text ) {
		m_lastRenderFilename = text->Value();
	}
	text = docH.FirstChildElement( "colorScheme" ).Child( 0 ).Text();
	if ( text ) {
		m_colorScheme = text->Value();
	}
	TiXmlElement* j = docH.FirstChildElement( "lastVideoCodec" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_lastVideoCodec );
	}
	j = docH.FirstChildElement( "lastAudioCodec" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_lastAudioCodec );
	}
	j = docH.FirstChildElement( "lastFramesize" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_lastFramesize );
	}
	j = docH.FirstChildElement( "lastFramerate" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_lastFramerate );
	}

	j = docH.FirstChildElement( "windowX" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_window_x );
	}
	j = docH.FirstChildElement( "windowY" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_window_y );
	}
	j = docH.FirstChildElement( "windowW" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_window_w );
	}
	j = docH.FirstChildElement( "windowH" ).Element();
	if ( j ) {
		j->Attribute( "value", &m_window_h );
	}
}

Prefs::~Prefs()
{
	if ( preferences_filename.length() == 0 ) {
		return;
	}
	TiXmlDocument doc( preferences_filename.c_str() );
	TiXmlDeclaration *dec = new TiXmlDeclaration( "1.0", "", "no" );
	doc.LinkEndChild( dec );
	
	TiXmlElement *item = new TiXmlElement( "version" );
	doc.LinkEndChild( item );
	TiXmlText* text = new TiXmlText( VERSION );
	item->LinkEndChild( text );
	
	item = new TiXmlElement( "browserFolder" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_browserFolder.c_str() );
	item->LinkEndChild( text );
	
	item = new TiXmlElement( "mediaFolder" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_mediaFolder.c_str() );
	item->LinkEndChild( text );

	item = new TiXmlElement( "lastProject" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_lastProject.c_str() );
	item->LinkEndChild( text );
	
	item = new TiXmlElement( "lastRenderFilename" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_lastRenderFilename.c_str() );
	item->LinkEndChild( text );

	item = new TiXmlElement( "colorScheme" );
	doc.LinkEndChild( item );
	text = new TiXmlText( m_colorScheme.c_str() );
	item->LinkEndChild( text );

	item = new TiXmlElement( "lastVideoCodec" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_lastVideoCodec );
	item = new TiXmlElement( "lastAudioCodec" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_lastAudioCodec );
	item = new TiXmlElement( "lastFramesize" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_lastFramesize );
	item = new TiXmlElement( "lastFramerate" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_lastFramerate );

	item = new TiXmlElement( "windowX" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_window_x );
	item = new TiXmlElement( "windowY" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_window_y );
	item = new TiXmlElement( "windowW" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_window_w );
	item = new TiXmlElement( "windowH" );
	doc.LinkEndChild( item );
	item->SetAttribute( "value", m_window_h );

	doc.SaveFile();
}
string Prefs::getBrowserFolder()
{ return m_browserFolder; }

void Prefs::setBrowserFolder( string filename )
{ m_browserFolder = filename; }

string Prefs::lastProject()
{ return m_lastProject; }

void Prefs::lastProject( string filename )
{ m_lastProject = filename; }
void Prefs::setWindowPosition( int x, int y, int w, int h )
{
	m_window_x = x;
	m_window_y = y;
	m_window_w = w;
	m_window_h = h;
}
void Prefs::getWindowPosition( int& x, int& y, int& w, int& h )
{
	x = m_window_x;
	y = m_window_y;
	w = m_window_w;
	h = m_window_h;
}

} /* namespace nle */

