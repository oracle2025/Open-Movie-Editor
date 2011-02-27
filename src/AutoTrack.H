/*  AutoTrack.H
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

#ifndef _AUTO_TRACK_H_
#define _AUTO_TRACK_H_

#include "timeline/Track.H"
#include "AudioVolumeFilter.H"
#include "timebase.H"

namespace nle
{

class AutoTrack : public Track
{
	public:
		AutoTrack( int num, string name = "Auto" );
		~AutoTrack();
		int type() { return TRACK_TYPE_AUTO; }
		float stretchFactor() { return (float)NLE_TIME_BASE; }
		void addFile( int64_t position, std::string filename, int64_t trimA = 0, int64_t trimB = 0, int mute = 0, int id = -1, int64_t length = -1, ClipData* data = 0 ) {}
		bool render_mode() { return true; }
		int getClipId() { return 0; }
		void updateClipId( int ) {}
		auto_node* getAutomationPoints() { return m_automationPoints; }
		double getValue( int64_t position );
		void reset();
	private:
		auto_node* m_automationPoints;
		auto_node* m_autoCache;
};

} /* namespace nle */

#endif /* _AUTO_TRACK_H_ */
