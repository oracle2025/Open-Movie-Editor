/*  VideoClip.H
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

#ifndef _VIDEO_CLIP_H_
#define _VIDEO_CLIP_H_

#include <stdint.h>

#include "global_includes.H"
#include "AudioClip.H"
#include "VideoEffectClip.H"
#include "timeline/Track.H"
namespace nle
{
class IVideoFile;
class FilmStrip;
class IClipArtist;
class LazyFrame;
class VideoClip : public AudioClip, public VideoEffectClip
{
	public:
		VideoClip( Track* track, int64_t position, IVideoFile* vf, int64_t A = 0, int64_t B = 0, int id = -1, ClipData* data = 0 );
		~VideoClip();
		int64_t length();
		void reset();
		LazyFrame* getRawFrame( int64_t position, int64_t &position_in_file );
		string filename();
		inline IVideoFile* file() { return m_videoFile; }
		int type() { return CLIP_TYPE_VIDEO; }
		inline IClipArtist* getArtist() { return m_artist; }
		bool hasAudio();
		int64_t fileLength();
		int w();
		int h();
		ClipData* getClipData() { return vec_getClipData(); }
		using Clip::trimA;
		using Clip::trimB;
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		int interlacing();
	protected:
		int64_t audioTrimA();
		int64_t audioTrimB();
		int64_t audioPosition();
		int64_t maxAudioLength();
	private:
		IVideoFile* m_videoFile;
		int64_t m_lastFrame;

		int64_t m_audioPosition;
		bool m_noAudio;
		IClipArtist* m_artist;
};

} /* namespace nle */

#endif /* _VIDEO_CLIP_H_ */
