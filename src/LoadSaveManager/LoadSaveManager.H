/*  LoadSaveManager.H
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

#ifndef _LOAD_SAVE_MANAGER_H_
#define _LOAD_SAVE_MANAGER_H_

#include "global_includes.H"

class Fl_Choice;
class Fl_Button;

namespace nle
{
class LoadSaveManager;
extern LoadSaveManager* g_loadSaveManager;
	
class LoadSaveManager
{
	public:
		LoadSaveManager( Fl_Choice* projectChoice, Fl_Button* projectInput );
		~LoadSaveManager();
		void startup();
		void shutdown();
		void newProject();
		void saveAs();
		void save();
		void name( string v );
		void load( string v );
	private:
		string nodups( string name );
		Fl_Choice* m_projectChoice;
		Fl_Button* m_projectInput;
		string m_currentName;
		string m_currentFilename;
		string m_video_projects;
};

} /* namespace nle */

#endif /* _LOAD_SAVE_MANAGER_H_ */
