/*  DummyClipArtist.cxx
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

#include <FL/fl_draw.H>

#include "DummyClipArtist.H"
#include "DummyClip.H"
#include "emblem-unreadable.xpm"
#include "TimelineView.H"
#include "timeline/Track.H"

namespace nle
{

DummyClipArtist::DummyClipArtist( DummyClip* clip )
	: m_clip( clip )
{
}

void DummyClipArtist::render( Rect& rect, int64_t, int64_t )
{
	int x_ = g_timelineView->get_screen_position( m_clip->position(), m_clip->track()->stretchFactor() );
	fl_push_clip( rect.x, rect.y, rect.w, rect.h );
	fl_color( FL_WHITE );
	fl_font( FL_HELVETICA, 11 );
	fl_draw( m_clip->filename().c_str(), x_ + 28, rect.y + rect.h - 5 );
	fl_draw_pixmap( emblem_unreadable_xpm, x_ + 10, rect.y );
	fl_color( FL_RED );
	fl_draw( "File missing or unreadable", x_ + 28, rect.y + 12 );
	fl_pop_clip();
}

} /* namespace nle */
