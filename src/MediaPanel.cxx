/*  MediaPanel.cxx
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
#include <FL/filename.H>
#include "MediaPanel.H"
#include "Prefs.H"
#include "Fl_Split.h"

namespace nle
{

	inline void MediaPanel::cb_browser_i(nle::FolderBrowser*, void*) {
	  browser->click();
	  if ( !fl_filename_isdir( browser->folder() ) ) {
		  browser->load( "/" );
	  } 
	  load( browser->folder() );
	}
	void MediaPanel::cb_browser(nle::FolderBrowser* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_browser_i(o,v);
	}


	inline void MediaPanel::cb_8_i(Fl_Button*, void*) {
	  browser->up();
	  if ( !fl_filename_isdir( browser->folder() ) ) {
		  browser->load( "/" );
	  } 
	  load( browser->folder() );
	}
	void MediaPanel::cb_8(Fl_Button* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_8_i(o,v);
	}

	inline void MediaPanel::cb_81_i(Fl_Button*, void*) {
	  browser->size( bt->w(), bt->h() / 2 );
	  int x, y, w, h;
	  x =  bt->x();
	  y = bt->y() + bt->h() / 2;
	  w = bt->w();
	  h = bt->h() - bt->h() / 2;
	  if ( bt->h() % 2 == 1 ) {
		  h++;
	  }
	files->resize( x, y, w, h );
	bt->remove( browser );
	bt->remove( files );
	bt->add( browser );
	bt->add( files );
	bt->redraw();
	}
	void MediaPanel::cb_81(Fl_Button* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb_81_i(o,v);
	}

	inline void MediaPanel::cb__i(Fl_Button*, void*) {
	  browser->resize( bt->x(), bt->y(), bt->w() / 2, bt->h() );
	  int x, y, w, h;
	  x = bt->x() + bt->w() / 2;
	  y = bt->y();
	  w = bt->w() / 2;
	  h = bt->h();
	  if ( bt->w() % 2 == 1 ) {
		  w++;
	  }
	files->resize( x, y, w, h );
	bt->remove( browser );
	bt->remove( files );
	bt->add( browser );
	bt->add( files );
	bt->redraw();
	}
	void MediaPanel::cb_(Fl_Button* o, void* v) {
	  ((MediaPanel*)(o->parent()->parent()->user_data()))->cb__i(o,v);
	}

	MediaPanel::~MediaPanel()
	{
	}

	MediaPanel::MediaPanel( int x, int y, int w, int h, const char *l )
		: Fl_Group( 0, 0, 360, 245, l )
	{
	  {
	    Fl_Group* o = this;
	    o->user_data((void*)(this));
	    { Fl_Split* o = bt = new Fl_Split(0, 20, 360, 225);
	      { nle::FolderBrowser* o = browser = new nle::FolderBrowser(0, 20, 185, 225);
		o->box(FL_NO_BOX);
		o->color(FL_BACKGROUND2_COLOR);
		o->selection_color(FL_SELECTION_COLOR);
		o->labeltype(FL_NORMAL_LABEL);
		o->labelfont(0);
		o->labelsize(14);
		o->labelcolor(FL_BLACK);
		o->callback((Fl_Callback*)cb_browser);
		o->align(FL_ALIGN_BOTTOM);
		o->when(FL_WHEN_RELEASE_ALWAYS);
	      }
	      { nle::MediaBrowser* o = files = new nle::MediaBrowser(185, 20, 175, 225);
		o->box(FL_NO_BOX);
		o->color(FL_BACKGROUND2_COLOR);
		o->selection_color(FL_SELECTION_COLOR);
		o->labeltype(FL_NORMAL_LABEL);
		o->labelfont(0);
		o->labelsize(14);
		o->labelcolor(FL_BLACK);
		o->align(FL_ALIGN_BOTTOM);
		o->when(FL_WHEN_RELEASE_ALWAYS);
	      }
	      o->end();
	      Fl_Group::current()->resizable(o);
	    }
	    { Fl_Group* o = new Fl_Group(0, 0, 360, 20);
	      { Fl_Button* o = new Fl_Button(0, 0, 20, 20, "@-28>");
		o->callback((Fl_Callback*)cb_8);
	      }
	      { Fl_Output* o = folderDisplay = new Fl_Output(20, 0, 300, 20);
                o->box(FL_FLAT_BOX);
		o->color(FL_BACKGROUND_COLOR);
		Fl_Group::current()->resizable(o);
	      }
	      { Fl_Button* o = new Fl_Button(340, 0, 20, 20, "@-28||");
		o->callback((Fl_Callback*)cb_81);
	      }
	      { Fl_Button* o = new Fl_Button(320, 0, 20, 20, "@-2||");
		o->callback((Fl_Callback*)cb_);
	      }
	      o->end();
	    }
	    o->end();
	  }
	  resize( x, y, w, h );
	  const char* folder = g_preferences->getMediaFolder().c_str();
	  browser->load( folder );
	  load( folder );
	}
	void MediaPanel::load( const char* folder )
	{
		folderDisplay->value( folder );
		files->load( folder );
		g_preferences->setMediaFolder( folder );
	}

} /* namespace nle */
