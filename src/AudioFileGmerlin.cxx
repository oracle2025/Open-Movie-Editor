/*  AudioFileGmerlin.cxx
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


#include "AudioFileGmerlin.H"
#include "ErrorDialog/IErrorHandler.H"
#include <cstring>
namespace nle
{

AudioFileGmerlin::AudioFileGmerlin( string filename )
{
	m_ok = false;
	m_filename = filename;
	m_frame1 = 0;
	m_frame2 = 0;
	m_converter = 0;
	bgav_options_t* opt;
	m_file = bgav_create();
	opt = bgav_get_options( m_file );
	bgav_options_set_sample_accurate( opt, 1 );
	if( !bgav_open( m_file, filename.c_str() ) ) {
		ERROR_DETAIL( "Could not open Audio file" );
		return;
	}
	if( bgav_is_redirector( m_file ) ) {
		ERROR_DETAIL( "Could not open Audio file" );
		return;
	}
	if ( bgav_num_tracks( m_file ) == 0 ) {
		ERROR_DETAIL( "This file has zero tracks" );
		return;
	}
	//bgav_get_duration( m_file, 0 );
	if ( bgav_num_audio_streams( m_file, 0 ) == 0 ) {
		ERROR_DETAIL( "This file has zero audio streams" );
		return;
	}
	if ( !bgav_can_seek_sample( m_file ) ) {
		ERROR_DETAIL( "This file does not support sample accurate seeking" );
		return;
	}
	if ( !bgav_select_track( m_file, 0 ) ) {
		ERROR_DETAIL( "Selecting a track in this file failed" );
		return;
	}
	if ( !bgav_set_audio_stream( m_file, 0, BGAV_STREAM_DECODE ) ) {
		ERROR_DETAIL( "Setting an audio stream in this file failed" );
		return;
	}
	if ( !bgav_start( m_file ) ) {
		ERROR_DETAIL( "Starting the decoding failed" );
		return;
	}

	gavl_audio_format_copy( &m_format, bgav_get_audio_format( m_file, 0 ) );

	gavl_audio_format_copy( &m_float_format, &m_format );

	m_samplerate = m_format.samplerate;
	m_length = bgav_audio_duration( m_file, 0 );

	m_float_format.num_channels = 2;
	m_float_format.interleave_mode = GAVL_INTERLEAVE_2;
	m_float_format.sample_format = GAVL_SAMPLE_FLOAT;
	m_float_format.channel_locations[0] = GAVL_CHID_FRONT_LEFT;
	m_float_format.channel_locations[1] = GAVL_CHID_FRONT_RIGHT;

	m_samples_per_frame = 0;
	m_converter = gavl_audio_converter_create();
	gavl_audio_converter_init( m_converter, &m_format, &m_float_format );
	m_frame2 = gavl_audio_frame_create( &m_float_format );
	//Create converter that transforms audio to 2chan float interleaved,
	//resampling happens outside.
	m_ok = true;
}
AudioFileGmerlin::~AudioFileGmerlin()
{
	bgav_close( m_file );
	if ( m_frame1 ) {
		gavl_audio_frame_destroy( m_frame1 );
	}
	if ( m_frame2 ) {
		gavl_audio_frame_destroy( m_frame2 );
	}
	m_frame1 = 0;
	m_frame2 = 0;
}
void AudioFileGmerlin::seek( int64_t sample )
{
	bgav_seek_audio( m_file, 0, sample );
}
void AudioFileGmerlin::reinit_frame( unsigned long frames )
{
	m_samples_per_frame = frames;
	m_format.samples_per_frame = frames;
	m_float_format.samples_per_frame = frames;
	if ( m_frame1 ) {
		gavl_audio_frame_destroy( m_frame1 );
	}
	m_frame1 = gavl_audio_frame_create( &m_format );
	gavl_audio_frame_destroy( m_frame2 );
	m_frame2 = gavl_audio_frame_create( &m_float_format );
}
int AudioFileGmerlin::fillBuffer( float* output, unsigned long frames )
{
	if ( frames != m_samples_per_frame ) {
		reinit_frame( frames );
	}

	/*float* tmp = m_frame2->samples.f;
	m_frame2->samples.f = output;*/

	int ret = bgav_read_audio( m_file, m_frame1, 0, frames );
	gavl_audio_convert( m_converter, m_frame1, m_frame2 );
	memcpy( output, m_frame2->samples.f, sizeof(float) * frames * 2 );

	/*m_frame2->samples.f = tmp;*/

	return ret;
}

	
} /* namespace nle */
