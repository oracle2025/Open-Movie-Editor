/*  RemoveTrackCommand.H
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


#ifndef _REMOVE_TRACK_COMMAND_H_
#define _REMOVE_TRACK_COMMAND_H_

#include "Command.H"

namespace nle
{

class Track;

class RemoveTrackCommand : public Command
{
	public:
		RemoveTrackCommand( Track* track );
		~RemoveTrackCommand();
		void doo();
		void undo();
	private:
		int m_track;
		int m_trackPosition;
		int m_type;
		command_node* m_subCmdList;
};

} /* namespace nle */


#endif /* _REMOVE_TRACK_COMMAND_H_ */
