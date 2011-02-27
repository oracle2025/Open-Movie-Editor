/*  AudioFileSnd.cxx
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

#include "AudioFileSnd.H"
#include "ErrorDialog/IErrorHandler.H"
#include "globals.H"

namespace nle
{

AudioFileSnd::AudioFileSnd( string filename )
{
	m_mono = false;
	m_ok = false;
	SF_INFO sfinfo;
	m_sndfile = sf_open( filename.c_str(), SFM_READ, &sfinfo );	
	if ( SF_ERR_NO_ERROR != sf_error( m_sndfile ) ) {
		ERROR_DETAIL( "This is not a supported audio file format" );
		return;
	}
	if ( sfinfo.channels != 2 && sfinfo.channels != 1 ) {
		ERROR_DETAIL( "Only Stereo and Mono audio files are supported" );
		return;
	}
	if ( sfinfo.frames==0 ) {
		ERROR_DETAIL( "This is an empty audio file" );
		return;
	}
	m_samplerate = sfinfo.samplerate;
	if ( sfinfo.samplerate != 48000 && sfinfo.samplerate != 44100 ) {
		ERROR_DETAIL( "Audio samplerates other than 48000 and 44100 are not supported" );
		return;
	}
	m_length = sfinfo.frames;
//	m_length = sfinfo.frames * NLE_TIME_BASE / m_samplerate;
	m_filename = filename;
	if ( sfinfo.channels == 1 ) {
		m_mono = true;
	}
	m_ok = true;
}
AudioFileSnd::~AudioFileSnd()
{
	if ( m_sndfile ) {
		sf_close( m_sndfile );
		m_sndfile = 0;
	}
}

void AudioFileSnd::seek( int64_t position )
{
	//sf_seek( m_sndfile, position * m_samplerate / NLE_TIME_BASE, SEEK_SET );
	sf_seek( m_sndfile, position, SEEK_SET );
}

int AudioFileSnd::fillBuffer( float* output, unsigned long frames )
{
	int read;
	if ( m_mono ) {
		read = sf_readf_float( m_sndfile, output, frames );
		for ( int i = read - 1; i >= 0; i-- ) {
			output[2 * i + 1] = output[i];
			output[2 * i] = output[i];
		}
		return read;
	} else {
		return sf_readf_float( m_sndfile, output, frames );
	}
}





} /* namespace nle */
