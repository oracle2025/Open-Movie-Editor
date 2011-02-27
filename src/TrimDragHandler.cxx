/*  TrimDragHandler.cxx
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

#include "TrimDragHandler.H"
#include "TimelineView.H"
#include "VideoViewGL.H"
#include "VideoClip.H"
#include "globals.H"
#include "IVideoFile.H"
#include "timeline/Track.H"

void preview_callback( void* data )
{
	nle::TrimDragHandler* tdh = (nle::TrimDragHandler*)data;
	tdh->preview();
}

namespace nle
{

TrimDragHandler::TrimDragHandler( TimelineView *tlv, Clip *clip,
		int track, int left, int right,
		bool trimRight )
	: DragHandler( tlv, clip ), m_track( track ), m_left( left ),
	m_right( right ), m_trimRight( trimRight )
{
}
int get_track_top( Track* track );
void TrimDragHandler::OnDrag( int x, int )
{
	m_x = x;
	int64_t pos = g_timelineView->get_real_position( x, m_clip->track()->stretchFactor() ) - ( m_clip->position() - m_clip->trimA() );
	if ( m_clip->fileLength() > 0 && ( pos < 0 || pos > m_clip->fileLength() ) ) {
		if ( m_trimRight ) {
			x = g_timelineView->get_screen_position( m_clip->position() - m_clip->trimA() + m_clip->fileLength(), m_clip->track()->stretchFactor() );
		} else {
			x = g_timelineView->get_screen_position( m_clip->position() - m_clip->trimA(), m_clip->track()->stretchFactor() ); 
		}
	}
	
	m_tlv->window()->make_current();
	fl_overlay_rect( x,
			m_tlv->y() + get_track_top( m_clip->track() ),
			1, m_clip->track()->h() );
	if ( m_clip->track()->type() == TRACK_TYPE_VIDEO ) {
		Fl::add_check( preview_callback, this );
	}
}
void TrimDragHandler::OnDrop( int x, int )
{
	m_tlv->window()->make_current();
	fl_overlay_clear();
	m_tlv->trim_clip( m_clip, x, m_trimRight );
}
void TrimDragHandler::preview()
{
	Fl::remove_check( preview_callback, this );
	VideoClip* cl = dynamic_cast<VideoClip*>(m_clip);
	if ( !cl ) {
		return;
	}
	int64_t pos = g_timelineView->get_real_position( m_x ) - ( cl->position() - cl->trimA() );
	if ( pos < 0 || pos > cl->file()->length() ) {
		if ( m_trimRight ) {
			cl->file()->seek( cl->file()->length() );
		} else {
			cl->file()->seek( 0 );
		}
	} else {
		cl->file()->seek( pos );
	}
	LazyFrame* fs = cl->file()->read();
	
	g_videoView->pushFrame( fs, false );
}
	
} /* namespace nle */
