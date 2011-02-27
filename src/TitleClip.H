/*  TitleClip.H
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

#ifndef _TITLE_CLIP_H_
#define _TITLE_CLIP_H_

#include <FL/x.H>
#include "FilterClip.H"
#include "IVideoReader.H"
#include "global_includes.H"
#include "VideoEffectClip.H"
class Fl_Image;

namespace nle
{

class TitleClip : public FilterClip, public VideoEffectClip
{
	public:
		TitleClip( Track* track, int64_t position, int64_t length = -1, int id = -1, ClipData* data = 0 );
		~TitleClip();
		int64_t length();
		LazyFrame* getRawFrame( int64_t position, int64_t &position_in_file );
		bool ok() { return m_ok; }
		string filename() { return "TitleClip"; }
		void reset() {}
		int type() { return CLIP_TYPE_IMAGE; }
		inline IClipArtist* getArtist() { return m_artist; }
		using Clip::trimA;
		using Clip::trimB;
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		int64_t fileLength();
		int w();
		int h();
		void text( const char* t ) { m_text = t; }
		void x( float x ) { m_x = x; }
		void y( float y ) { m_y = y; }
		void size( int size ) { m_size = size; }
		void font( int font ) { m_font = font; }
		void color( Fl_Color color ) { m_color = color; }
		const char* text() { return m_text.c_str(); }
		float x() { return m_x; }
		float y() { return m_y; }
		int size() { return m_size; }
		int font() { return m_font; }
		Fl_Color color() { return m_color; }
		void touch() { m_dirty = true; }
		ClipData* getClipData();
	private:
		void init();
		Fl_Image* m_image;
		int64_t m_length;
		gavl_video_frame_t* m_gavl_frame;
		LazyFrame* m_lazy_frame;
		bool m_ok;
		IClipArtist* m_artist;
		string m_text;
		int m_font;
		int m_size;
		float m_x;
		float m_y;
		bool m_dirty;
		unsigned char* m_pixels;
		unsigned char* m_alpha;
		Fl_Offscreen m_offscreen;
		Fl_Color m_color;
		bool m_render_mode;
};

} /* namespace nle */

#endif /* _TITLE_CLIP_H_ */

