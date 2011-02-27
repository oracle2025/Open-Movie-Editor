/*  VideoClipArtist.cxx
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

#include <cassert>
#include <math.h>

#include <FL/fl_draw.H>
#include <FL/filename.H>

#include "VideoClipArtist.H"
#include "VideoClip.H"
#include "FilmStrip.H"
#include "TimelineView.H"
#include "globals.H"
#include "timeline/Track.H"
#include "mute.xpm"
#include "SwitchBoard.H"
#include "AudioClipArtist.H"
#include "FilterBase.H"
#include "FilmStripFactory.H"

namespace nle
{

VideoClipArtist::VideoClipArtist( VideoClip* clip )
	: m_clip( clip )
{
	m_audioClipArtist = 0;
	if ( m_clip->hasAudio() ) {
		m_audioClipArtist = new AudioClipArtist( m_clip, false );
	}
}
VideoClipArtist::~VideoClipArtist()
{
	if ( m_audioClipArtist ) {
		delete m_audioClipArtist;
	}
}
void VideoClipArtist::render( Rect& rect_in, int64_t start, int64_t stop )
{
	Rect rect = rect_in;
	if ( rect_in.h > 30 ) {
		rect.h = 30;
	}
	fl_push_clip( rect.x, rect.y, rect.w, rect.h );		
	int _x;
	FilmStrip* fs = g_filmStripFactory->get( m_clip->file() );
	int64_t s = m_clip->trimA() / (4 * NLE_TIME_BASE);
	int64_t e = ( m_clip->length() / (4 * NLE_TIME_BASE) ) + s;
	//int64_t off = m_clip->trimA() % (NLE_TIME_BASE * 4);
	int64_t inc = 1 + (int64_t)( 10.0 / GetZoom() );
	assert( inc >= 1 );
	if ( fs ) {
		for ( int64_t k = s; k < e + 2; k += inc ) {
			// TODO: g_timeline should not be used, rect, start and stop are
			// sufficient

			_x = g_timelineView->get_screen_position( m_clip->position() + (k - s) * NLE_TIME_BASE * 4, m_clip->track()->stretchFactor() );
			
			pic_struct* f = fs->get_pic(k);
			if ( f ) {
				fl_draw_image( f->data, _x, rect.y, f->w, f->h );
			}
		}
	}
	if ( m_clip->m_mute ) {
		fl_draw_pixmap( mute_xpm, rect.x + 20, rect.y + 5 );
	}

	_x = g_timelineView->get_screen_position( m_clip->position(), m_clip->track()->stretchFactor() );
	fl_color( FL_DARK3 );
	fl_font( FL_HELVETICA, 11 );
	fl_draw( fl_filename_name( m_clip->filename().c_str() ), _x + 6, rect.y + rect.h - 4 );
	fl_color( FL_WHITE );
	fl_draw( fl_filename_name( m_clip->filename().c_str() ), _x + 5, rect.y + rect.h - 5 );
	
	fl_pop_clip();
	
	fl_push_clip( rect_in.x, rect_in.y, rect_in.w, rect_in.h );		
	if ( rect_in.h > 30 && m_audioClipArtist && !m_clip->m_mute ) {
		rect.y += 35;
		rect.h = rect_in.h - 35;
		m_audioClipArtist->render( rect, llrint( start * 48000 / NLE_TIME_BASE ), llrint( stop * 48000 / NLE_TIME_BASE ) );
		fl_color( FL_WHITE );
		fl_rectf( rect_in.x, rect_in.y + 30, rect_in.w, 5 );
	}
	filter_stack* n = m_clip->getFilters();
	for ( ; n; n = n->next ) {
		n->filter->onDraw(rect_in);
	}
	fl_pop_clip();

}

} /* namespace nle */
