/*  Ruler.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "Ruler.H"
#include "globals.H"
#include "TimelineView.H"
#include "helper.H"
#include "SwitchBoard.H"
#include "Timeline.H"
#include "fps_helper.H"

namespace nle
{

Ruler* g_ruler;


Ruler::Ruler( int x, int y, int w, int h, const char *label )
	: Fl_Widget( x, y, w, h, label )
{
	m_stylus.x = LEFT_TRACK_SPACING - 12 + x;
	m_stylus.x -= 25; /* adjust for snapping tool */
	m_stylus.y = 0;
	m_stylus.w = 25;
	m_stylus.h = 25;
	g_ruler = this;
}

void Ruler::draw()
{
	fl_push_clip( x(), y(), w(), h());
	
	fl_draw_box( FL_UP_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );

	fl_color( FL_FOREGROUND_COLOR );
	fl_font( FL_HELVETICA, 11 );
	int pixel_start = - (int)( g_timelineView->scrollPosition() * GetZoom() ) - 100 + LEFT_TRACK_SPACING;
	pixel_start = pixel_start % 100;
	int pixel_step = 100;
	int pixel_count = w() / 100 + 2;
	for ( int i = 0; i < pixel_count; i++ ) {
		int off = x() + pixel_start + pixel_step * i;
		fl_draw( timestamp_to_string( g_timelineView->get_real_position( off ) ), off, y() + 14 );
		fl_line( off, y() + 20, off, y() + h() );
	}
	draw_stylus();
	fl_pop_clip();
}

int Ruler::handle( int event )
{
	static int __x = 0;
	int _x = Fl::event_x();
	int _y = Fl::event_y() - y();
	switch ( event ) {
		case FL_PUSH:
			if ( m_stylus.inside( _x, _y ) ) {
				__x = _x - m_stylus.x;
			}
			return 1;
		case FL_DRAG:
			if ( !__x )
				break;
			{
				long new_x = _x - __x;
				g_timelineView->stylus( new_x + ( m_stylus.w / 2 ) );
				return 1;
			}
		case FL_RELEASE:
			if ( __x ) {
				__x = 0;
				return 1;
			} else {
				g_timelineView->stylus( _x );
				return 1;
			}
			break;
	}
	return Fl_Widget::handle( event );
}
void Ruler::stylus( long stylus_pos )
{
	m_stylus.x = stylus_pos - ( m_stylus.w / 2 );
	redraw(); //FIXME: OpenGL Window is redrawn
	// Maybe it should be somehow draw from within VideoViewGL
}

/** 
 * draw the stylus diamond to the ruler
 */
void Ruler::draw_stylus()
{
  fl_draw_box( FL_DIAMOND_UP_BOX, m_stylus.x, y() + m_stylus.y, 
	       m_stylus.w, m_stylus.h, FL_BACKGROUND_COLOR );
}

void Ruler::skipForward()
{
	int64_t framelen = single_frame_length( g_timeline->m_playback_fps );
	if ( framelen > 0 ) {
		g_timelineView->move_cursor_by(framelen);
	}
}
void Ruler::skipBackward()
{
	int64_t framelen = single_frame_length( g_timeline->m_playback_fps );
	if ( framelen > 0 ) {
		g_timelineView->move_cursor_by((-1)*framelen);
	}

}
void Ruler::skipFirst()
{
	g_timelineView->move_cursor( 0 );
}
void Ruler::skipLast()
{
	g_timelineView->move_cursor( g_timeline->length() );
}

} /* namespace nle */

