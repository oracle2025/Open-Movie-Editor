/*  AudioClipArtist.cxx
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
#include <FL/filename.H>

#include "AudioClipArtist.H"
#include "AudioClip.H"
#include "WavArtist.H"
#include "TimelineView.H"
#include "timeline/Track.H"
#include "IAudioFilter.H"

namespace nle
{

AudioClipArtist::AudioClipArtist( AudioClip* clip, bool renderFilters )
	: m_clip( clip ), m_renderFilters( renderFilters )
{
}
AudioClipArtist::~AudioClipArtist()
{
}
void AudioClipArtist::render( Rect& rect, int64_t start, int64_t stop )
{
	int64_t start_, stop_;
	if ( m_clip->audioPosition() < start ) {
		start_ = m_clip->audioTrimA() + ( start - m_clip->audioPosition() );
	} else {
		start_ = m_clip->audioTrimA();
	}
	if ( m_clip->audioPosition() + m_clip->audioLength() > stop ) {
		stop_ = m_clip->audioTrimA() + ( stop - m_clip->audioPosition() );
	} else {
		stop_ = m_clip->audioTrimA() + m_clip->audioLength();
	}
	g_wavArtist->render( m_clip->filename(), rect, start_, stop_ );
	fl_push_clip( rect.x, rect.y, rect.w, rect.h );		
	int _x = g_timelineView->get_screen_position( m_clip->position(), m_clip->track()->stretchFactor() );
	fl_color( FL_DARK3 );
	fl_font( FL_HELVETICA, 11 );
	fl_draw( fl_filename_name( m_clip->filename().c_str() ), _x + 6, rect.y + rect.h - 4 );
	fl_color( FL_WHITE );
	fl_draw( fl_filename_name( m_clip->filename().c_str() ), _x + 5, rect.y + rect.h - 5 );
	fl_pop_clip();
	if ( m_renderFilters ) {
		filter_stack* n = m_clip->getFilters();
		IAudioFilter* filter;
		for ( ; n; n = n->next ) {
			filter = dynamic_cast<IAudioFilter*>( n->filter );
			if ( filter ) {
				filter->onDraw(rect);
			}
		}
	}
}

	
} /* namespace nle */
