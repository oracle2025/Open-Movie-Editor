/*  WaveForm.cxx
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

#include <cstdio>
#include <cassert>

#include "WaveForm.H"
#include "AudioFileFactory.H"
#include "IAudioFile.H"
#include "TimelineView.H"
#include "SwitchBoard.H"
#include "globals.H"
#include "DiskCache.H"

#define PEAK_RANGE 1000
#define BUFFER_LEN 1024
namespace nle
{

WaveForm::WaveForm( JobDoneListener* listener, string filename )
	: Job( listener )
{
	m_filename = filename;
	m_peaks = 0;
	m_peakLength = 0;
	m_finalLength = 0;
	m_af = 0;
	m_cache = new DiskCache( filename, "wavform" );

	if ( m_cache->isEmpty() ) {
		m_af = AudioFileFactory::get( filename );
		m_finalLength = m_af->length() / PEAK_RANGE;
		m_af->seek( 0 );
	} else {
		m_finalLength = m_cache->size() / sizeof(float);
	}
	m_peaks = new float[m_finalLength];
}

WaveForm::~WaveForm()
{
	if ( m_af ) {
		delete m_af;
	}
	if ( m_peaks ) {
		delete [] m_peaks;
	}
	if ( m_cache ) {
		delete m_cache;
		m_cache = 0;
	}
}

bool WaveForm::done()
{
	if ( m_peakLength >= m_finalLength ) {
		return true;
	}
	return false;
}
bool WaveForm::process( double& progress )
{
	assert( m_cache );
	if ( m_peakLength >= m_finalLength ) {
		if ( m_af ) {
			delete m_af;
			m_af = 0;
		}
		if ( m_cache ) {
			m_cache->clean();
			delete m_cache;
			m_cache = 0;
		}
		g_timelineView->redraw();
		return false;
	}
	if ( m_cache->isEmpty() ) {
		static float buffer[PEAK_RANGE * 2];
		unsigned long range;
		float max;
		range = m_af->fillBuffer( buffer, PEAK_RANGE );
		max = 0.0;
		for ( int j = 0; j < (int)(range - 1) * 2; j++ ) {
			max = buffer[j] > max ? buffer[j] : max;
		}
		m_peaks[m_peakLength] = max;
		m_cache->write( &m_peaks[m_peakLength], sizeof(float) );
		m_peakLength++;
	} else {
		int64_t c = m_cache->read( &m_peaks[m_peakLength], 1000 );
		m_peakLength += ( c / sizeof(float) );
		if ( c == 0 ) {
			cout << "WARNING WaveForm::process c == 0" << endl;
			m_peakLength = m_finalLength;
		}
	}
	return true;
}
const char* WaveForm::filename()
{
	return m_filename.c_str();
}

} /* namespace nle */
