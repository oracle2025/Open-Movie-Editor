/*  MoveDragHandler.cxx
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
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "MoveDragHandler.H"
#include "TimelineView.H"
#include "timeline/Track.H"
#include "globals.H"


namespace nle
{

class Track;

MoveDragHandler::MoveDragHandler( TimelineView *tlv,
		Clip *clip, int x, int y,
		const Rect& rect )
	: DragHandler(tlv, clip)
{
	m_rect = rect;
	m_x = x;
	m_y = y;
}
void MoveDragHandler::OnDrag( int x, int y )
{
	int64_t tmp_x;
	Rect tmp( x - ( m_x - m_rect.x ),
			m_tlv->y() + y - ( m_y - m_rect.y ),
			m_rect.w, m_rect.h );
	Track *tr = m_tlv->get_track( x, y );
	if ( tr ) {
		Rect tr_rect = m_tlv->get_track_rect( tr );
		tmp.y = m_tlv->y() + tr_rect.y;
		if ( g_snap ) {
			tmp_x = m_tlv->get_real_position( tmp.x, tr->stretchFactor() );
			tmp_x = tr->getSnapA( m_clip, tmp_x );
			tmp_x = tr->getSnapB( m_clip, tmp_x );
			tmp.x = m_tlv->get_screen_position( tmp_x, tr->stretchFactor() );
			
		} else {
			tmp.x = m_tlv->get_screen_position( m_tlv->get_real_position( tmp.x, tr->stretchFactor() ), tr->stretchFactor() );
		}
		tmp.h = tr->h();
	}
	m_tlv->window()->make_current();
	fl_overlay_rect( tmp.x, tmp.y, tmp.w, tmp.h );
}
void MoveDragHandler::OnDrop( int x, int y )
{
	m_tlv->window()->make_current();
	fl_overlay_clear();
	m_tlv->move_clip( m_clip, x, y, m_x - m_rect.x );
}


} /* namespace nle */
