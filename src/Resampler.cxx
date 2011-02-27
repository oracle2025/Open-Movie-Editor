/*  Resampler.cxx
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

#include "global_includes.H"

#include "Resampler.H"


namespace nle
{

static long resampler_callback( void* data, float** output )
{
	Resampler* res = (Resampler*)data;
	long frames = res->m_audiofile->fillBuffer( res->m_buffer, 4096 );
	*output = res->m_buffer;
	return frames;
}

Resampler::Resampler( IAudioFile* audiofile )
	: m_audiofile( audiofile )
{
	m_filename = audiofile->filename();
	m_state = src_callback_new( resampler_callback, SRC_SINC_MEDIUM_QUALITY, 2, &m_error, this  );
	m_srcSamplerate = audiofile->samplerate();
	src_set_ratio( m_state, ( 48000.0 / ((float)m_srcSamplerate) ) );
	m_length = m_audiofile->length() * 48000 / m_srcSamplerate;
	m_samplerate = 48000;
	m_ok = true;
}

Resampler::~Resampler()
{
	delete m_audiofile;
	m_state = src_delete( m_state );
}

void Resampler::seek( int64_t position )
{
	m_audiofile->seek( position * m_srcSamplerate / 48000 );
	src_reset( m_state );
}

int Resampler::fillBuffer( float* output, unsigned long frames )
{
	long c = src_callback_read ( m_state, ( 48000.0 / ((float)m_srcSamplerate) ), frames, output );
	return c;
}

} /* namespace nle */
