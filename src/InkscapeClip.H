/*  InkscapeClip.H
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

#ifndef _INKSCAPE_CLIP_H_
#define _INKSCAPE_CLIP_H_
#include "FilterClip.H"
#include "IVideoReader.H"
#include "global_includes.H"
#include "VideoEffectClip.H"
#include <tinyxml.h>
class Fl_Shared_Image;
namespace nle
{

class ImageClip;

class InkscapeClip : public FilterClip, public VideoEffectClip
{
	public:
		InkscapeClip( Track* track, int64_t position, string filename, int64_t length = -1, int id = -1, ClipData* data = 0 );
		~InkscapeClip();
		int64_t length();
		LazyFrame* getRawFrame( int64_t position, int64_t &position_in_file );
		bool ok();
		string filename() { return m_filename; }
		void reset() {}
		int type() { return CLIP_TYPE_IMAGE; }
		inline IClipArtist* getArtist();
		using Clip::trimA;
		using Clip::trimB;
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		int64_t fileLength();
		int w();
		int h();
		ClipData* getClipData() { return vec_getClipData(); }
		int getActionCount() { return 1; }
		// vv- Index starts at 0, good old C-Style. ;-)
		const char* getActionName( int index )
			{ if ( index == 0 ) { return "Edit with Inkscape"; } return 0; }
		void doAction( int index );
		void detectSvgUpdate();
	private:
		ImageClip* m_image_clip;
		string m_filename;
		int64_t m_length;
};

} /* namespace nle */

#endif /* _INKSCAPE_CLIP_H_ */
