/*  PasteSelectionCommand.cxx
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

#include "PasteSelectionCommand.H"
#include "TimelineView.H"
#include "timeline/Clip.H"
#include "timeline/Track.H"
#include "DocManager.H"
#include "PasteCommand.H"
extern float g_fps;

namespace nle
{


PasteSelectionCommand::PasteSelectionCommand( clip_node* selectedClips )
{
	m_subCmdList = 0;
	for ( clip_node* n = selectedClips; n; n = n->next ) {
		command_node* c = new command_node;
		c->next = 0;
		Command* cmd = new PasteCommand( n->clip );
		c->command = cmd;
		m_subCmdList = (command_node*)sl_push( m_subCmdList, c );
	}
}

PasteSelectionCommand::PasteSelectionCommand( PasteSelectionCommand* command )
{
	m_subCmdList = 0;
	for ( command_node* n = command->m_subCmdList; n; n = n->next ) {
		command_node* c = new command_node;
		c->next = 0;
		Command* cmd = new PasteCommand( dynamic_cast<PasteCommand*>(n->command) );
		c->command = cmd;
		m_subCmdList = (command_node*)sl_push( m_subCmdList, c );
	}

}
void PasteSelectionCommand::position( int64_t position )
{
	//TODO: Compensate for Audio
	//find minimum of all Commands
	int64_t minimum = 0;
	PasteCommand* cmd = dynamic_cast<PasteCommand*>(m_subCmdList->command);
	if ( cmd ) {
		if ( cmd->m_audioClip ) {
			minimum = (int64_t)( cmd->position() / 48000 * NLE_TIME_BASE );
		} else {
			minimum = cmd->position();
		}
	} else {
		return;
	}
	for ( command_node* n = m_subCmdList; n; n = n->next ) {
		cmd = dynamic_cast<PasteCommand*>(n->command);
		if ( cmd->m_audioClip ) {
			minimum = min( minimum, (int64_t)( cmd->position() / 48000 * NLE_TIME_BASE ) );
		} else {
			minimum = min( minimum, cmd->position() );
		}
	}
	int64_t diff = position - minimum;
	for ( command_node* n = m_subCmdList; n; n = n->next ) {
		cmd = dynamic_cast<PasteCommand*>(n->command);
		if ( cmd->m_audioClip ) {
			cmd->position( cmd->position() + (int64_t)( diff * 48000 / NLE_TIME_BASE ) );
		} else {
			cmd->position( cmd->position() + diff );
		}
	}
}


} /* namespace nle */
