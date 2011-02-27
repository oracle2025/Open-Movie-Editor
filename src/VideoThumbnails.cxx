/*  VideoThumbnails.cxx
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

#include <FL/Fl_Shared_Image.H>

extern "C" {
#include <gmerlin/avdec.h>
}

#include "VideoThumbnails.H"
#include "DiskCache.H"
#include "helper.H"

#include <cstring>
#include <iostream>

namespace nle
{

bool VideoThumbnails::get( const char* filename, unsigned char* rgb, int &w, int &h )
{
	DiskCache cache( filename, "thumb" );
	if ( cache.isEmpty() ) {
		bgav_t* decoder = bgav_create();
		if ( !bgav_open( decoder, filename ) ) {
			bgav_close( decoder );
			decoder = 0;
		} else if( bgav_is_redirector( decoder ) ) {
			bgav_close( decoder );
			decoder = 0;
		} else if ( bgav_num_video_streams( decoder, 0 ) == 0 ) {
			bgav_close( decoder );
			decoder = 0;
		} else if ( !bgav_select_track( decoder, 0 ) ) {
			bgav_close( decoder );
			decoder = 0;
		} else if ( !bgav_set_video_stream( decoder, 0, BGAV_STREAM_DECODE ) ) {
			bgav_close( decoder );
			decoder = 0;
		} else if ( !bgav_start( decoder ) ) {
			bgav_close( decoder );
			decoder = 0;
		}
		if ( !decoder ) {
			Fl_Shared_Image* image;
			image = Fl_Shared_Image::get( filename );
			if ( !image || image->d() != 3 ) {
				cache.clean();
				return false;
			}
			w = image->w();
			h = image->h();
			Fl_Image* image2 = image->copy( VIDEO_THUMBNAIL_WIDTH, VIDEO_THUMBNAIL_HEIGHT );
			image->release();
			char** d = (char**)image2->data();
			memcpy( rgb, d[0], VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
			delete image2;
			cache.write( &w, sizeof(int) );
			cache.write( &h, sizeof(int) );
			cache.write( rgb, VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
			cache.clean();
			return true;
		}
		const gavl_video_format_t *video_format = bgav_get_video_format( decoder, 0 );
		gavl_video_frame_t *gavl_frame = gavl_video_frame_create( video_format );
		bgav_read_video( decoder, gavl_frame, 0 );
		bgav_close( decoder );
		decoder = 0;
		gavl_video_format_t rgb_format;
		gavl_video_format_copy( &rgb_format, video_format );
		rgb_format.pixelformat = GAVL_RGB_24;
		rgb_format.interlace_mode = GAVL_INTERLACE_NONE;
		rgb_format.image_height = rgb_format.frame_height = VIDEO_THUMBNAIL_HEIGHT;
		rgb_format.image_width = rgb_format.frame_width = VIDEO_THUMBNAIL_WIDTH;
		gavl_video_frame_t *rgb_frame = gavl_video_frame_create( &rgb_format );

		gavl_video_converter_t *converter = gavl_video_converter_create();
		gavl_video_options_t* options = gavl_video_converter_get_options( converter );
		gavl_video_options_set_deinterlace_mode( options, GAVL_DEINTERLACE_SCALE );
		gavl_video_options_set_scale_mode( options, GAVL_SCALE_NEAREST );
		gavl_video_converter_init( converter, video_format, &rgb_format );
		gavl_video_convert( converter, gavl_frame, rgb_frame );
		

		gavl_video_converter_destroy( converter );
		gavl_video_frame_destroy( gavl_frame );

		unsigned char* src = (unsigned char*)rgb_frame->planes[0];
		int strides = rgb_frame->strides[0];
		for ( int i = 0; i < VIDEO_THUMBNAIL_HEIGHT; i++ ) {
			memcpy( &rgb[VIDEO_THUMBNAIL_WIDTH*i*3], &src[i*strides], VIDEO_THUMBNAIL_WIDTH * 3 );
		}
		w = video_format->frame_width;
		h = video_format->frame_height;
		cache.write( &w, sizeof(int) );
		cache.write( &h, sizeof(int) );
		cache.write( rgb, VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
		cache.clean();
		gavl_video_frame_destroy( rgb_frame );
		return true;
	} else {
		if ( cache.size() == 0 ) {
			return 0;
		}
		cache.read( &w, sizeof(int) );
		cache.read( &h, sizeof(int) );
		cache.read( rgb, VIDEO_THUMBNAIL_HEIGHT * VIDEO_THUMBNAIL_WIDTH * 3 );
		return true;
	}
}




} /* namespace nle */
