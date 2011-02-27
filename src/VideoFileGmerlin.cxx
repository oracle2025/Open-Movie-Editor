/*  VideoFileGmerlin.cxx
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

#include "VideoFileGmerlin.H"
#include "LazyFrame.H"
#include "ErrorDialog/IErrorHandler.H"

#include "ProgressDialog/IProgressListener.H"

#include "render_helper.H"

#include <cassert>

namespace nle
{

void gmerlin_index_callback( void* data, float percentage )
{
	VideoFileGmerlin* file = (VideoFileGmerlin*)data;
	if ( file->m_progress ) {
		file->m_progress->progress( percentage );
	}
}

VideoFileGmerlin::VideoFileGmerlin( std::string filename, IProgressListener* progress )
	: IVideoFile()
{
	m_ok = false;
	m_decoder = 0;
	m_gavl_frame = 0;
	m_lazy_frame = 0;
	m_decoder = bgav_create();
	m_progress = progress;
	assert( m_decoder );
	bgav_options_t* options = bgav_get_options( m_decoder );
	bgav_options_set_sample_accurate( options, 1 );
	bgav_options_set_index_callback( options, gmerlin_index_callback, this );
	if ( !bgav_open( m_decoder, filename.c_str() ) ) {
		ERROR_DETAIL( "Could not open Video file" );
		return;
	}
	if( bgav_is_redirector( m_decoder ) ) {
		ERROR_DETAIL( "Could not open Video file" );
		return;
	}
	if ( bgav_num_tracks( m_decoder ) == 0 ) {
		ERROR_DETAIL( "This file has zero tracks" );
		return;
	}
	if ( bgav_num_video_streams( m_decoder, 0 ) == 0 ) {
		ERROR_DETAIL( "This file has zero video streams" );
		return;
	}
	if ( !bgav_can_seek_sample( m_decoder ) ) {
		ERROR_DETAIL( "Sample accurate seeking not possible for this file" );
		return;
	}
	if ( !bgav_select_track( m_decoder, 0 ) ) {
		ERROR_DETAIL( "Selecting a track in this file failed" );
		return;
	}
	if ( !bgav_set_video_stream( m_decoder, 0, BGAV_STREAM_DECODE ) ) {
		ERROR_DETAIL( "Setting a video stream in this file failed" );
		return;
	}
	if ( !bgav_start( m_decoder ) ) {
		ERROR_DETAIL( "Could not start Codec" );
		return;
	}
	m_video_format = bgav_get_video_format( m_decoder, 0 );
	int64_t frame_duration = m_video_format->frame_duration;
	int64_t time_scale = m_video_format->timescale;
	m_ticksPerFrame = ( frame_duration * NLE_TIME_BASE ) / time_scale;

	m_width = m_video_format->frame_width;
	m_height = m_video_format->frame_height;

	m_gavl_frame = gavl_video_frame_create( m_video_format );
	m_lazy_frame = new LazyFrame( m_video_format );
	m_lazy_frame->put_data( m_gavl_frame );

	m_filename = filename;
	m_ok = true;
	m_progress = 0;
}
VideoFileGmerlin::~VideoFileGmerlin()
{
	bgav_close( m_decoder );
	if ( m_lazy_frame ) {
		delete m_lazy_frame;
	}
	if ( m_gavl_frame ) {
		gavl_video_frame_destroy( m_gavl_frame );
	}
}
bool VideoFileGmerlin::ok()
{
	return m_ok;
}
int64_t VideoFileGmerlin::length()
{
	return bgav_video_duration( m_decoder, 0 ) * m_ticksPerFrame / m_video_format->frame_duration;
}
LazyFrame* VideoFileGmerlin::read()
{
	bgav_read_video( m_decoder, m_gavl_frame, 0 );
	m_lazy_frame->alpha( 1.0 );
	return m_lazy_frame;
}
void VideoFileGmerlin::seek( int64_t position )
{
	bgav_seek_video( m_decoder, 0, position / m_ticksPerFrame * m_video_format->frame_duration );
}
int64_t VideoFileGmerlin::ticksPerFrame()
{
	return m_ticksPerFrame;
}
void VideoFileGmerlin::seekToFrame( int64_t frame )
{

	bgav_seek_video( m_decoder, 0, frame * m_video_format->frame_duration );
}

} /* namespace nle */
