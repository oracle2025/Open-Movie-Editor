/* VideoWriterQT.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "VideoWriterQT.H"
#include <cassert>
#include "LazyFrame.H"
#include "EncodingPreset.H"
#include <lqt.h>
#include <lqt_version.h>
#include "lqtgavl.h"
#include <iostream>
using namespace std;

namespace nle
{
void convert_pixel_aspect_to_pixel_w_h( float in, int& pw, int& ph )
{
	if ( in > 1.093 && in < 1.095 ) {
		pw = 128;
		ph = 117;
	} else if ( in > 1.4586 && in < 1.4588 ) {
		pw = 512;
		ph = 351;
	} else if ( in > 0.9116 && in < 0.9118 ) {
		pw = 4320;
		ph = 4739;
	} else if ( in > 1.2154 && in < 1.2156 ) {
		pw = 5760;
		ph = 4739;
	} 
	pw = 1;
	ph = 1;
/*
128/117  = 1.094
512/351  = 1.4587
4320/4739 = 0.9117
5760/4739 = 1.21557
*/
}
void convert_ome_format_to_gavl_format( video_format* in, gavl_video_format_t* out )
{
	out->frame_width  = in->w;
	out->frame_height = in->h;
	out->image_width  = in->w;
	out->image_height = in->h;
	convert_pixel_aspect_to_pixel_w_h( in->pixel_aspect_ratio, out->pixel_width, out->pixel_height );
	out->pixelformat = GAVL_RGB_24;
	switch ( in->interlacing ) {
		case INTERLACE_TOP_FIELD_FIRST:
			out->interlace_mode = GAVL_INTERLACE_TOP_FIRST;
			break;
		case INTERLACE_BOTTOM_FIELD_FIRST:
			out->interlace_mode = GAVL_INTERLACE_BOTTOM_FIRST;
			break;
		case INTERLACE_PROGRESSIVE:
			out->interlace_mode = GAVL_INTERLACE_NONE;
			break;
	}
	out->frame_duration = in->framerate.frame_duration;
	out->timescale = in->framerate.timescale;
	out->framerate_mode = GAVL_FRAMERATE_CONSTANT;
}
VideoWriterQT::VideoWriterQT( EncodingPreset* preset, const char* filename )
{
	preset->getFormat( &m_format );
	if ( preset->m_avi_odml ) {
		m_qt = lqt_open_write ( filename, LQT_FILE_AVI_ODML ); /* For new Libquicktime */
	} else {
		m_qt = lqt_open_write( filename, preset->m_file_type ); /* For new Libquicktime */
	}

	lqt_codec_info_t** codec = lqt_find_video_codec_by_name( m_format.video_codec );
	if (!codec || !codec[0]) {
		cerr << "Video Codec missing: " << m_format.video_codec << endl;
	}
	assert(codec);
	assert(codec[0]);

	convert_ome_format_to_gavl_format( &m_format, &m_gavl_format );

	lqt_gavl_add_video_track( m_qt, &m_gavl_format, codec[0] );
	
	lqt_destroy_codec_info( codec );

	codec = lqt_find_audio_codec_by_name( m_format.audio_codec );
	if ( !codec || !codec[0] ) {
		cerr << "Audio Codec missing: " << m_format.audio_codec << endl;
	}
	assert( codec );
	assert( codec[0] );

	lqt_set_audio( m_qt, 2, m_format.samplerate, 16, codec[0] );
	lqt_destroy_codec_info( codec );

	preset->set2( m_qt );
	m_samplerate = m_format.samplerate;
	m_rows = lqt_gavl_rows_create( m_qt, 0 );
	m_timestamp = 0;
}
VideoWriterQT::~VideoWriterQT()
{
	quicktime_close( m_qt );
	lqt_gavl_rows_destroy( m_rows );
}
void VideoWriterQT::encodeVideoFrame( LazyFrame* frame )
{
	frame->set_target( &m_gavl_format );
	gavl_video_frame_t* gavl_frame = frame->target();
	gavl_frame->timestamp = m_timestamp;
	m_timestamp += m_gavl_format.frame_duration;
	lqt_gavl_encode_video ( m_qt, 0, gavl_frame, m_rows );
}
void VideoWriterQT::encodeAudioFrame( float* buffer, int frames )
{
	float *buffer_p[2] = { left_buffer, right_buffer };
	assert( frames <= 32000 );
	for ( int i = 0; i < frames; i++ ) {
		left_buffer[i] = buffer[i*2];
		right_buffer[i] = buffer[i*2+1];
	}
	lqt_encode_audio_track( m_qt, 0, buffer_p, frames, 0 );
}

} /* namespace nle */

