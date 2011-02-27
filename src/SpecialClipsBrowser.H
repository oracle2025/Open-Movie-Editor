/*  SpecialClipsBrowser.H
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

#ifndef _SPECIAL_CLIPS_BROWSER_H_
#define _SPECIAL_CLIPS_BROWSER_H_

#include "global_includes.H"
#include <FL/Fl_Browser_.H>

namespace nle
{

typedef enum {
	PL_VIDEO_SRC,
	PL_AUDIO_FILTER,
	PL_VIDEO_EFFECT
} plugin_type;

typedef struct plugin_item_ {
	struct plugin_item_* next;
	struct plugin_item_* prev;
	string value;
	plugin_type type;
	string identifier;
} plugin_item;


class SpecialClipsBrowser : public Fl_Browser_
{
	public:
		SpecialClipsBrowser( int x, int y, int w, int h, const char *l = 0 );
		~SpecialClipsBrowser();
		int handle( int event );
		void add( const char* s, plugin_type type, const char* identifier );

	protected:
		void item_draw( void* p, int x, int y, int w, int h ) const;
		void* item_first() const;
		int item_height( void* p ) const;
		void* item_next( void* p ) const;
		void* item_prev( void* p ) const;
		int item_width( void* p ) const;
	private:
		void clear();
		plugin_item* m_items;
		plugin_item* m_last;
		plugin_item* m_item_selected;

};

} /* namespace nle */
#endif /* _SPECIAL_CLIPS_BROWSER_H_ */
