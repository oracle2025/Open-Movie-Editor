/*  VideoEffectClip.H
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

#ifndef _VIDEO_EFFECT_CLIP_H_
#define _VIDEO_EFFECT_CLIP_H_

#include <stdint.h>

#include "IVideoReader.H"
#include <gavl/gavl.h>

namespace nle
{

class FilterFactory;
class IVideoEffect;
class ClipData;
class FilterClip;
class LazyFrame;

class VideoEffectClip : public IVideoReader
{
	public:
		~VideoEffectClip();
		LazyFrame* getFrame( int64_t position );
		virtual LazyFrame* getRawFrame( int64_t position, int64_t &position_in_file ) = 0;
		virtual int w() = 0;
		virtual int h() = 0;
		//virtual int interlacing() = 0;
	protected:
		void setEffects( ClipData* data = 0 );
		VideoEffectClip( FilterClip* filterClip );
		ClipData* vec_getClipData();
	private:
		gavl_video_format_t m_format_dst;
		int m_bits;
		FilterClip* m_filterClip;
		bool m_scale_half_frame;
};

} /* namespace nle */

#endif /* _VIDEO_EFFECT_CLIP_H_ */
