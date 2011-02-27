/*  DummyClipArtist.H
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

#ifndef _DUMMY_CLIP_ARTIST_
#define _DUMMY_CLIP_ARTIST_

#include "IClipArtist.H"

namespace nle
{

class DummyClip;

class DummyClipArtist : public IClipArtist
{
	public:
		DummyClipArtist( DummyClip* clip );
		~DummyClipArtist() {}
		void render( Rect& rect, int64_t start, int64_t stop );
	private:
		DummyClip* m_clip;
};

}

#endif /* _DUMMY_CLIP_ARTIST_ */

