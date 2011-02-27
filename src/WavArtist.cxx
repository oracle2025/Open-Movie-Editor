/*  WavArtist.cxx
 *
 *  Copyright (C) 2005, 2009 Richard Spindler <richard.spindler AT gmail.com>
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
#include <cassert>

#include <FL/fl_draw.H>

#include "sl/sl.h"

#include "WavArtist.H"
#include "IAudioFile.H"
#include "WaveForm.H"

namespace nle
{

WavArtist* g_wavArtist;
	
WavArtist::WavArtist()
{
	g_wavArtist = this;
	m_peaks = 0;
}

WavArtist::~WavArtist()
{
	peakfile_node* node;
	while ( ( node = (peakfile_node*)sl_pop( &m_peaks ) ) ) {
		delete node->wav;
		delete node;
	}
}

void WavArtist::add( IAudioFile* file )
{
	peakfile_node* node = m_peaks;
	while ( node ) {
		if ( node->filename == file->filename() ) {
			node->refCount++;
			return;
		}
		node = node->next;
	}
       	node = new peakfile_node;
	node->wav = 0;
	WaveForm* waveform = new WaveForm( this, file->filename() );
	g_jobManager->submitJob( waveform );
	node->filename = file->filename();
	node->refCount = 1;
	m_peaks = (peakfile_node*)sl_push( m_peaks, node );
}
static int remove_peakfile_helper( void* p, void* data )
{
	peakfile_node* node = (peakfile_node*)p;
	const char* filename = (const char*)data;
	if ( strcmp( node->filename.c_str(), filename ) == 0 ) {
		node->refCount--;
		if ( node->refCount == 0 ) {
			return 1;
		}
	}
	return 0;
}
void WavArtist::remove( string filename )
{
	peakfile_node* node = (peakfile_node*)sl_remove( &m_peaks, remove_peakfile_helper, (void*)filename.c_str() );
	if ( node ) {
		if ( node->wav ) {
			delete node->wav;
		}
		delete node;
	}
}
static int find_peakfile_helper( void* p, void* data )
{
	peakfile_node* node = (peakfile_node*)p;
	const char* filename = (const char*)data;
	if ( strcmp( node->filename.c_str(), filename ) == 0 ) {
		return 1;
	}
	return 0;
}
void WavArtist::jobDone( Job* job_description )
{
	WaveForm* waveform = dynamic_cast<WaveForm*>( job_description );
	assert( waveform );
	peakfile_node* node = (peakfile_node*)sl_map( m_peaks, find_peakfile_helper, (void*)( waveform->filename() ) );
	if ( node ) {
		node->wav = waveform;
	} else {
		delete waveform;
	}
}
void WavArtist::render( string filename, Rect& rect, int64_t start, int64_t stop )
{
	fl_color( FL_GREEN );
	peakfile_node* node = (peakfile_node*)sl_map( m_peaks, find_peakfile_helper, (void*)filename.c_str() );
	if ( !node || !node->wav ) {
		return;
	}
	int64_t first = start / PEAK_RANGE;
	int64_t last = stop / PEAK_RANGE;
	float factor = (float)( last - first ) / rect.w;
	int y;
	int h2 = rect.y + rect.h / 2;
	for ( int i = 0; i < rect.w; i++ ) {
		y = first + (int)( i * factor );
		int h;
		if ( y < node->wav->length() ) {
			h = (int)( rect.h / 2 * node->wav->m_peaks[y] );
			fl_line( rect.x + i, h2 - h, rect.x + i, h2 + h );
		} else {
			break;
		}
	}
	
}


} /* namespace nle */

