/*  PasteCommand.H
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

#ifndef _PASTE_COMMAND_H_
#define _PASTE_COMMAND_H_

#include "Command.H"
#include "global_includes.H"

namespace nle
{
typedef struct _auto_node auto_node;

class PasteCommand : public Command
{
	public:
		PasteCommand( Clip* clip );
		PasteCommand( PasteCommand* command );
		~PasteCommand();
		void position( int64_t position ) { m_position = position; }
		int64_t position() { return m_position; }
		void doo();
		void undo();
		bool m_audioClip;
	private:
		int m_track;
		int m_clip;
		string m_filename;
		auto_node* m_automationPoints;
		int m_automationsCount;
		int64_t m_trimA;
		int64_t m_trimB;
		int64_t m_position;
		int64_t m_length;
		bool m_mute;
};

} /* namespace nle */

#endif /* _PASTE_COMMAND_H_ */
