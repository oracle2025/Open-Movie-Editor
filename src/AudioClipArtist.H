/*  AudioClipArtist.H
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

#ifndef _AUDIO_CLIP_ARTIST_H_
#define _AUDIO_CLIP_ARTIST_H_

#include <stdint.h>

#include "IClipArtist.H"

namespace nle
{

class AudioClip;

class AudioClipArtist : public IClipArtist
{
	public:
		AudioClipArtist( AudioClip* clip, bool renderFilters = true );
		~AudioClipArtist();
		void render( Rect& rect, int64_t start, int64_t stop );
	private:
		AudioClip* m_clip;
		bool m_renderFilters;
};

} /* namespace nle */

#endif /* _AUDIO_CLIP_ARTIST_H_ */
