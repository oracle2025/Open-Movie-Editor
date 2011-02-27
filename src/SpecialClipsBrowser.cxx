/*  SpecialClipsBrowser.cxx
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

#include <cstring>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "SpecialClipsBrowser.H"

#include "source.xpm"
#include "audio.xpm"
#include "video.xpm"

namespace nle
{
	
SpecialClipsBrowser::SpecialClipsBrowser( int x, int y, int w, int h, const char *l )
	: Fl_Browser_( x, y, w, h, l )
{
	m_last = m_items = 0;
	m_item_selected = 0;
}
SpecialClipsBrowser::~SpecialClipsBrowser()
{
	clear();
}
void SpecialClipsBrowser::clear()
{
	while ( m_items ) {
		plugin_item* f = m_items;
		m_items = m_items->next;
		delete f;
	}
	m_last = 0;
	m_item_selected = 0;

}
int SpecialClipsBrowser::handle( int event )
{
	int x, y, w, h;
	bbox( x, y, w, h );
	if ( !Fl::event_inside( x, y, w, h ) ) {
		return Fl_Browser_::handle( event );
	}
	switch ( event ) {
		case FL_PUSH:
			m_item_selected = (plugin_item*)find_item( Fl::event_y() );
			if ( m_item_selected ) {
				redraw_line( m_item_selected );
			}
			return 1;
		case FL_RELEASE:
			if ( m_item_selected ) {
				redraw_line( m_item_selected );
				m_item_selected = 0;
			}
			return 1;
		case FL_FOCUS:
			return 1;
		case FL_DRAG:
			if ( m_item_selected ) {
				//Fl::copy( "TitleClip", strlen("TitleClip") + 1, 0 );
				Fl::copy( m_item_selected->identifier.c_str(), m_item_selected->identifier.length() + 1, 0 );
				// "src:TitleClip"
				// "effect:frei0r:Name"
				// "effect:builtin:Name"
				// "filter:builtin:Name"
				Fl::dnd();
				redraw_line( m_item_selected );
				m_item_selected = 0;
			}
			return 1;
	}
	return Fl_Browser_::handle( event );
}
void SpecialClipsBrowser::add( const char* s, plugin_type type, const char* identifier )
{
	plugin_item* f = new plugin_item;
	f->value = s;
	f->type = type;
	f->identifier = identifier;
	f->next = 0;
	f->prev = m_last;
	if ( m_items ) {
		m_last->next = f;
		m_last = f;
	} else {
		m_last = m_items = f;
	}
}
void SpecialClipsBrowser::item_draw( void* p, int x, int y, int w, int h ) const
{
	plugin_item* f = (plugin_item*)p;
	Fl_Color bg_color = FL_BACKGROUND2_COLOR;
	if ( m_item_selected == f ) {
		bg_color = FL_SELECTION_COLOR;
		fl_draw_box( FL_FLAT_BOX, x, y, w, h, FL_SELECTION_COLOR );
	}
	fl_font( FL_HELVETICA, 14 );
	fl_color( FL_FOREGROUND_COLOR );
	switch ( f->type ) {
		case PL_VIDEO_SRC: 
			fl_draw_pixmap( source_xpm, x + 2, y + h - 16, bg_color );
			break;
		case PL_AUDIO_FILTER:
			fl_draw_pixmap( audio_xpm, x + 2, y + h - 16, bg_color );
			break;
		case PL_VIDEO_EFFECT:
			fl_draw_pixmap( video_xpm, x + 2, y + h - 16, bg_color );
			break;
	}
	fl_draw( f->value.c_str(), x + 20, y + h - fl_descent() );
}
void* SpecialClipsBrowser::item_first() const
{
	return m_items;
}
int SpecialClipsBrowser::item_height( void* ) const
{
	fl_font( FL_HELVETICA, 14 );
	return fl_height();
}
void* SpecialClipsBrowser::item_next( void* p ) const
{
	plugin_item* f = (plugin_item*)p;
	return f->next;
}
void* SpecialClipsBrowser::item_prev( void* p ) const
{
	plugin_item* f = (plugin_item*)p;
	return f->prev;
}
int SpecialClipsBrowser::item_width( void* ) const
{
	return 20;
}

} /* namespace nle */
