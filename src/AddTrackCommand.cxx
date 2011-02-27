/*  AddTrackCommand.cxx
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

#include <cassert>

#include "AddTrackCommand.H"
#include "TimelineView.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include "AutoTrack.H"


namespace nle
{

AddTrackCommand::AddTrackCommand( int type )
	: m_type( type )
{
	track_node* p;
	m_track = g_timeline->getTrackId();
	m_position = 0;
	for ( p = g_timeline->getTracks(); p; p = p->next ) {
		if ( type == TRACK_TYPE_VIDEO && p->track->type() == TRACK_TYPE_AUDIO ) {
			break;
		}
		m_position++;
	}
}
AddTrackCommand::~AddTrackCommand()
{
}
void AddTrackCommand::doo()
{
	Track* track = 0;
	switch ( m_type ) {
		case TRACK_TYPE_VIDEO:
			track = new VideoTrack( g_timeline, m_track );
			break;
		case TRACK_TYPE_AUDIO:
			track = new AudioTrack( g_timeline, m_track );
			break;
		case TRACK_TYPE_AUTO:
			track = new AutoTrack( m_track );
			break;
	}
	if ( track ) {
		g_timeline->addTrack( track, m_position );
		g_timelineView->redraw();
	}
}
void AddTrackCommand::undo()
{
	g_timeline->removeTrack( m_track );
}

} /* namespace nle */
