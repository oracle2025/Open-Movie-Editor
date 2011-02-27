/*  DocManager.H
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

#ifndef _DOC_MANAGER_H_
#define _DOC_MANAGER_H_

namespace nle
{

class Command;

typedef struct _command_node {
	struct _command_node *next;
	Command* command;
} command_node;

class DocManager
{
	public:
		DocManager();
		~DocManager();
		void undo();
		void redo();
		void submit( Command* cmd );
		bool canUndo();
		bool canRedo();
	private:
		void setButtons();
		command_node* m_undoList;
		command_node* m_redoList;
};

extern DocManager* g_docManager;
inline void submit( Command* cmd ) { g_docManager->submit( cmd ); }

} /* namespace nle */

#endif /* _DOC_MANAGER_H_ */
