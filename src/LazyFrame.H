/*  LazyFrame.H
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

#ifndef _LAZY_FRAME_H_
#define _LAZY_FRAME_H_

/*

The Idea of the Lazy Frame approach is to defer any kind of color conversion to
the last stage of processing, and to eventually omit it, or perform the fastest
conversion possible

*/

#include <gavl/gavl.h>

namespace nle
{

class LazyFrame
{
	public:
		LazyFrame( const gavl_video_format_t *format );
		LazyFrame( int w, int h );
		~LazyFrame();
		gavl_video_frame_t* RGBA();
		gavl_video_frame_t* native();
		gavl_video_frame_t* target();
		gavl_video_format_t* format();
		void set_target( gavl_video_format_t *format );
		void set_rgb_target( int w, int h );
		unsigned char* get_target_buffer();
		int get_target_buffer_strides();
		//compressed(); //for DV direct Rendering
		float alpha();
		void alpha( float value );
		void put_data( gavl_video_frame_t* frame );
		bool cacheable() { return m_cacheable; }
		void dirty( bool val ) { m_dirty = val; }
		bool dirty() { return m_dirty; }
		void this_frame_wont_change() { m_cacheable = true; }
	private:
		bool m_cacheable;
		bool m_dirty;
		float m_alpha;
		gavl_video_converter_t *m_converter_RGBA;
		gavl_video_converter_t *m_converter_target;
		gavl_video_frame_t* m_frame;
		gavl_video_frame_t* m_rgba_frame;
		gavl_video_frame_t* m_target_frame;
		gavl_video_format_t m_src_format;
		gavl_video_format_t m_target_format;
};

} /* namespace nle */



#endif /* _LAZY_FRAME_H_ */
