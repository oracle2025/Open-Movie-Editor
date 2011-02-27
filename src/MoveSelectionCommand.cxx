/*  MoveSelectionCommand.cxx
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

#include "MoveSelectionCommand.H"
#include "TimelineView.H"
#include "timeline/Clip.H"
#include "timeline/Track.H"
#include "DocManager.H"
#include "MoveCommand.H"


namespace nle
{

MoveSelectionCommand::MoveSelectionCommand( Clip* clip, Track* track, int64_t position, clip_node* m_selectedClips )
{
	m_subCmdList = 0;
	float diff = ( ( position - clip->position() ) / clip->track()->stretchFactor() );
	for ( clip_node* n = m_selectedClips; n; n = n->next ) {
		Track* tr = track ? track : n->clip->track();
		command_node* c = new command_node;
		c->next = 0;
		Command* cmd = new MoveCommand( n->clip, tr, (int64_t)( n->clip->position() + (diff * n->clip->track()->stretchFactor() ) ) );
		c->command = cmd;
		m_subCmdList = (command_node*)sl_push( m_subCmdList, c );
	}
}


} /* namespace nle */
