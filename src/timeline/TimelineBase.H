/*  TimelineBase.H
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

#ifndef _TIMELINE_BASE_H_
#define _TIMELINE_BASE_H_

#include "ClipIdProvider.H"

#include <string>
#include <stdint.h>

namespace nle
{

class Track;
class Clip;
class ClipData;

typedef struct _track_list_node {
	struct _track_list_node *next;
	Track *track;
} track_node;

class TimelineBase : public ClipIdProvider
{
	public:
		// constructors and destructors
		TimelineBase();
		virtual ~TimelineBase();

		// method declerations
		void sort();
		void addClip( int track, Clip *clip );
		void addFile( int track, int64_t position, std::string filename, int64_t trimA = 0, int64_t trimB = 0, int mute = 0, int id = -1, int64_t length = -1, ClipData* data = 0 );
		void removeClip( int track, Clip *clip );
		void addTrack( Track* track, int position = -1 );
		void removeTrack( int track );
		Clip* find( int track, int64_t position );
		int64_t length();
		inline track_node* getTracks() { return m_allTracks; }
		virtual void clear();
		void trackUp( Track* track );
		void trackDown( Track* track );
		Track* getTrack( int track );
		Clip* getClip( int track, int clip );
		int getClipId();
		void updateClipId( int id );
	protected:
		// member variable declerations
		track_node* m_allTracks;
		int m_clipId;
};
	
} /* namespace nle */

#endif /* _TIMELINE_BASE_H_ */
