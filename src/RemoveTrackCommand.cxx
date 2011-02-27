/*  RemoveTrackCommand.cxx
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

#include "sl/sl.h" 
#include "DocManager.H"
#include "RemoveTrackCommand.H"
#include "Timeline.H"
#include "TimelineView.H"
#include "RemoveCommand.H"
#include "VideoTrack.H"
#include "AudioTrack.H"

#include "timeline/Clip.H"

namespace nle
{

RemoveTrackCommand::RemoveTrackCommand( Track* track )
{
	m_track = track->num();
	m_trackPosition = 0;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		if ( i->track == track ) {
			break;
		}
		m_trackPosition++;
	}
	m_type = track->type();
	m_subCmdList = 0;
	for ( clip_node* n = track->getClips(); n; n = n->next ) {
		command_node* c = new command_node;
		c->next = 0;
		Command* cmd = new RemoveCommand( n->clip );
		c->command = cmd;
		m_subCmdList = (command_node*)sl_push( m_subCmdList, c );
	}
}

RemoveTrackCommand::~RemoveTrackCommand()
{
	command_node* n;
	while ( ( n = (command_node*)sl_pop( &m_subCmdList ) ) ) {
		delete n->command;
		delete n;
	}
}

void RemoveTrackCommand::doo()
{
//	Track* t = g_timeline->getTrack( m_track );
	g_timeline->removeTrack( m_track );
	g_timelineView->redraw();
}

void RemoveTrackCommand::undo()
{
	Track* track = 0;
	switch ( m_type ) {
		case TRACK_TYPE_VIDEO:
			track = new VideoTrack( g_timeline, m_track );
			break;
		case TRACK_TYPE_AUDIO:
			track = new AudioTrack( g_timeline, m_track );
			break;
	}
	if ( track ) {
		g_timeline->addTrack( track, m_trackPosition );
		for( command_node* n = m_subCmdList; n; n = n->next ) {
			n->command->undo();
		}
		g_timelineView->redraw();
	}

}

} /* namespace nle */
