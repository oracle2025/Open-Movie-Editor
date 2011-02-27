/*  AudioClip.H
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

#ifndef _AUDIO_CLIP_H_
#define _AUDIO_CLIP_H_

#include "FilterClip.H"
#include "Rect.H"
#include "global_includes.H"

namespace nle
{

class IAudioFile;
class DragHandler;
class IClipArtist;
class IAudioReader;

class AudioClip : public FilterClip
{
	public:
		AudioClip( Track* track, int64_t position, IAudioFile* af = 0, int64_t trimA = 0, int64_t trimB = 0, int id = -1 );
		AudioClip( Track* track, int64_t position, IAudioFile* af = 0, int id = -1 );
		virtual ~AudioClip();
		int fillBufferRaw( float* output, unsigned long frames, int64_t position );
		int fillBuffer( float* output, unsigned long frames, int64_t position );
		virtual void reset();
		using Clip::trimA;
		using Clip::trimB;
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		
		DragHandler* onMouseDown( Rect& rect, int x, int y, bool shift );

		virtual int64_t audioTrimA();
		virtual int64_t audioTrimB();
		int64_t audioLength();
		virtual int64_t audioPosition();

		int64_t length();
		string filename();
		int64_t fileLength();
		int type() { return CLIP_TYPE_AUDIO; }
		IClipArtist* getArtist() { return m_artist; }
		ClipData* getClipData() { return 0; }
		bool m_mute;
	protected:
		IAudioFile* m_audioFile;
		IAudioReader* m_audioReader;
	private:
		int64_t m_lastSamplePosition;
		IClipArtist* m_artist;
};
	
} /* namespace nle */

#endif /* _AUDIO_CLIP_H_ */
