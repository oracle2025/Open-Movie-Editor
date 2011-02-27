/*  MediaPanel.H
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

#ifndef _MEDIA_PANEL_H_
#define _MEDIA_PANEL_H_

#include <FL/Fl.H>
//#include <FL/Fl_Tile.H>
#include "FolderBrowser.H"
#include "MediaBrowser.H"
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include "Fl_Split.h"

namespace nle
{

class MediaPanel : public Fl_Group
{
	public:
		MediaPanel( int x, int y, int w, int h, const char *l = 0 );
		~MediaPanel();
		void load( const char* folder );
		MediaBrowser *files;
		Fl_Output *folderDisplay;
		Fl_Split *bt;
		nle::FolderBrowser *browser;
	private:
		inline void cb_browser_i(nle::FolderBrowser*, void*);
		static void cb_browser(nle::FolderBrowser*, void*);
		inline void cb_8_i(Fl_Button*, void*);
		static void cb_8(Fl_Button*, void*);
		inline void cb_81_i(Fl_Button*, void*);
		static void cb_81(Fl_Button*, void*);
		inline void cb__i(Fl_Button*, void*);
		static void cb_(Fl_Button*, void*);
};

} /* namespace nle */

#endif /* _MEDIA_PANEL_H_ */
