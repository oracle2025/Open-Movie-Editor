/*  Track.H
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

#ifndef _TRACK_H_
#define _TRACK_H_

#include <string>

using namespace std;

#include "ClipIdProvider.H"
#include <stdint.h>
#include <sl.h>

namespace nle
{
class Clip;
class ClipData;
class ClipIdProvider;

typedef struct clip_node_ {
	struct clip_node_ *next;
	Clip* clip;
} clip_node;

enum Track_Types {
	TRACK_TYPE_VIDEO = 0,
	TRACK_TYPE_AUDIO = 1,
	TRACK_TYPE_AUTO = 2
};

class Track : public ClipIdProvider
{
	public:
		virtual ~Track();
		clip_node* getClips() { return m_clips; }
		virtual int type() = 0;
		virtual float stretchFactor() = 0;
		virtual void sort();
		virtual void addFile( int64_t position, std::string filename, int64_t trimA = 0, int64_t trimB = 0, int mute = 0, int id = -1, int64_t length = -1, ClipData* data = 0) = 0;
		
		void addClip( Clip* clip );
		virtual void removeClip( Clip* clip );
		Clip* find( int64_t position );
		int64_t length(); // <= normalized to stretch factor, therefore not accurate
		virtual bool render_mode() = 0;

	//	void remove( Clip* clip );
		inline int num() { return m_num; }
		int64_t getSnapB( Clip* clip, int64_t a );
		int64_t getSnapA( Clip* clip, int64_t a );
		Clip* getClip( int id );
		inline string name() { return m_name; }
		inline void name( string n ) { m_name = n; }
		int h() { return m_height; }
		void h( int h ) { m_height = h; }
	protected:
		Track( int num, string name );
		clip_node* m_clips;
	private:
		int m_num;
		string m_name;
		int m_height;
};
	
} /* namespace nle */

#endif /* _TRACK_H_ */

