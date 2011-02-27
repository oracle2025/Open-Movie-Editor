/*  DummyClip.H
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

#ifndef _DUMMY_CLIP_
#define _DUMMY_CLIP_

#include "timeline/Clip.H"
#include "global_includes.H"

namespace nle
{

class DummyClip : public Clip
{
	public:
		DummyClip( Track* track, string filename, int64_t position, int64_t length, int64_t A, int64_t B, int id = -1 );
		~DummyClip() {}
		int64_t length() { return ( m_fileLength - ( m_trimA + m_trimB ) ); }
		int64_t fileLength() { return m_fileLength; }
		void reset() {}
		string filename() { return m_filename; }
		int type() { return -1; }
		IClipArtist* getArtist() { return m_artist; }
		ClipData* getClipData() { return 0; }
	private:
		string m_filename;
		int64_t m_fileLength;
		IClipArtist* m_artist;
};

} /* namespace nle */

#endif /* _DUMMY_CLIP_ */
