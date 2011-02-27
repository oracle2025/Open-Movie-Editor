/*  AudioFileProject.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include "AudioFileProject.H"
#include "Timeline.H"

namespace nle
{

AudioFileProject::AudioFileProject( Timeline* timeline, std::string filename )
{
	m_filename = filename;
	m_timeline = timeline;
	m_samplerate = 48000;
	m_timeline->sort();
	m_timeline->seek( 0 );
	m_length = m_timeline->soundLength();
	m_ok = true;
	m_delete_timeline_on_destroy = false;
}
AudioFileProject::AudioFileProject( std::string filename )
{
	m_filename = filename;
	Timeline* x = g_timeline;
	m_timeline = new Timeline;
	g_timeline = x;
	m_timeline->read( filename );
	m_timeline->render_mode( true );
	m_samplerate = 48000;
	m_timeline->sort();
	m_timeline->seek( 0 );
	m_length = m_timeline->soundLength();
	m_ok = true;
	m_delete_timeline_on_destroy = true;
}
AudioFileProject::~AudioFileProject()
{
	if ( m_delete_timeline_on_destroy ) {
		delete m_timeline;
	}
}
void AudioFileProject::seek( int64_t sample )
{
	m_timeline->sampleseek( 1, sample );
}
int AudioFileProject::fillBuffer( float* output, unsigned long frames )
{
	int r = m_timeline->fillBuffer( output, frames );
	m_timeline->sampleseek( 0, r );
	return r;
}

} /* namespace nle */
