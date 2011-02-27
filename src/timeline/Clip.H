/*  Clip.H
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

#ifndef _CLIP_H_
#define _CLIP_H_

#include <string>

#include <stdint.h>

namespace nle
{

class Track;
class IClipArtist;

enum Clip_Types {
	CLIP_TYPE_VIDEO = 0,
	CLIP_TYPE_AUDIO = 1,
	CLIP_TYPE_IMAGE = 2
};

class ClipData
{
	public:
		virtual ~ClipData() {}
};

class Clip
{
	public:
		virtual ~Clip() {}
		virtual int64_t length() = 0;
		virtual int64_t fileLength() = 0;
		virtual void reset() = 0;
		void position( int64_t position ) { m_position = position; }
		const int64_t position() { return m_position; }
		Track* track() { return m_track; }
		void track( Track* track ) { m_track = track; }
		virtual int64_t trimA( int64_t trim );
		inline int64_t trimA() { return m_trimA; }
		virtual int64_t trimB( int64_t trim );
		inline int64_t trimB() { return m_trimB; }
		virtual std::string filename()=0;
		inline int64_t A() { return position(); }
		inline int64_t B() { return m_position + length(); }
		virtual bool has_automation() { return false; }
		virtual int type() = 0;
		virtual IClipArtist* getArtist() { return 0; }
		inline int id() { return m_id; }
		inline void selected( bool s ) { m_selected = s; }
		inline bool selected() { return m_selected; }
		virtual ClipData* getClipData() = 0;
		virtual int getActionCount() { return 0; }
		virtual const char* getActionName( int index ) { return 0; }
		virtual void doAction( int index ) {}
	protected:
		Clip( Track *track, int64_t position, int id = -1 );
		int64_t m_position;
		Track* m_track;
		int64_t m_trimA;
		int64_t m_trimB; // wird von hinten gemessen.
		int m_id;
		bool m_selected;
};

} /* namespace nle */

#endif /* _CLIP_H_ */
