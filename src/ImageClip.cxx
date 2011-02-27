/*  ImageClip.cxx
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

#include <FL/Fl_Shared_Image.H>

#include "ImageClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "ImageClipArtist.H"
#include "helper.H"
#include "globals.H"
#include "timeline/Track.H"
#include "LazyFrame.H"

namespace nle
{


ImageClip::ImageClip( Track* track, int64_t position, string filename, int64_t length, int id, ClipData* data )
	: FilterClip( track, position, id ), VideoEffectClip( this ), m_filename( filename )
{
	m_ok = false;
	m_artist = 0;
	m_image = Fl_Shared_Image::get( filename.c_str() );
	m_gavl_frame = 0;
	m_lazy_frame = 0;
	if ( !m_image ) {
		ERROR_DETAIL( "This is not an image file" );
		return;
	}
	if ( m_image->w() > 1024 || m_image->h() > 1024 ) {
		int m_scaled_w, m_scaled_h;
		if ( m_image->w() > m_image->h() ) {
			m_scaled_w = 1024;
			m_scaled_h = m_image->h() * 1024 / m_image->w();
		} else {
			m_scaled_w = m_image->w() * 1024 / m_image->h();
			m_scaled_h = 1024;
		}
		m_image->release();
		m_image = Fl_Shared_Image::get( filename.c_str(), m_scaled_w, m_scaled_h );
	}


	if ( length > 0 ) {
		m_length = length;
	} else {
		m_length = NLE_TIME_BASE * 10;
	}
	if ( m_image->d() != 4 && m_image->d() != 3 ) {
		ERROR_DETAIL( "This image file has a wrong color depth,\nonly RGB and RGBA images are supported" );
		return;
	}
	
	if ( m_image->d() == 3 ) {
		gavl_video_format_t format;
		format.frame_width  = m_image->w();
		format.frame_height = m_image->h();
		format.image_width  = m_image->w();
		format.image_height = m_image->h();
		format.pixel_width = 1;
		format.pixel_height = 1;
		format.pixelformat = GAVL_RGB_24;
		format.interlace_mode = GAVL_INTERLACE_NONE;
		m_lazy_frame = new LazyFrame( &format );
	} else {
		m_lazy_frame = new LazyFrame( m_image->w(), m_image->h() );
	}
	m_gavl_frame = gavl_video_frame_create( 0 );
	char** d = (char**)m_image->data();
	m_gavl_frame->planes[0] = (unsigned char *)d[0];
	m_gavl_frame->strides[0] = m_image->w() * m_image->d();
	m_lazy_frame->put_data( m_gavl_frame );
	m_lazy_frame->this_frame_wont_change();
	
	if ( !track->render_mode() ) {
		m_artist = new ImageClipArtist( m_image );
	}
	
	m_ok = true;
	setEffects( data );
}

ImageClip::~ImageClip()
{
	if ( m_gavl_frame ) {
		gavl_video_frame_null( m_gavl_frame );
		gavl_video_frame_destroy( m_gavl_frame );
	}
	if ( m_image ) {
		m_image->release();
		m_image = 0;
	}
	if ( m_artist ) {
		delete m_artist;
	}
	if ( m_lazy_frame ) {
		delete m_lazy_frame;
	}
}
int64_t ImageClip::length()
{
	return m_length;
}

LazyFrame* ImageClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	position_in_file = position - m_position;
	if ( position >= m_position && position <= m_position + m_length ) {
		return m_lazy_frame;
	} else {
		return 0;
	}
}
LazyFrame* ImageClip::getFirstFrame()
{
	return m_lazy_frame;
}

int ImageClip::w()
{
	return m_image->w();
}
int ImageClip::h()
{
	return m_image->h();
}
int64_t ImageClip::trimA( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	if ( m_position + trim < 0 ) {
		trim = (-1) * m_position;
	}
	m_length -= trim;
	m_position += trim;
	return trim;
}
int64_t ImageClip::trimB( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	m_length -= trim;
	return trim;
}
int64_t ImageClip::fileLength()
{
	return -1;
}

} /* namespace nle */
