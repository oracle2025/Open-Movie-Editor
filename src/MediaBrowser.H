/*  MediaBrowser.H
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

#ifndef _MEDIA_BROWSER_H_
#define _MEDIA_BROWSER_H_

#include <string>
#include <FL/Fl_Browser_.H>

using namespace std;

namespace nle
{

typedef struct file_item_ {
	struct file_item_* next;
	struct file_item_* prev;
	string value;
	int w;
	int h;
	unsigned char* rgb;
	bool empty;
} file_item;

class MediaBrowser : public Fl_Browser_
{
	public:
		MediaBrowser( int x, int y, int w, int h, const char *l = 0 );
		~MediaBrowser();
		void load( string folder );
		int handle( int event );

		void item_draw( void* p, int x, int y, int w, int h ) const;
		void* item_first() const;
		int item_height( void* p ) const;
		void* item_next( void* p ) const;
		void* item_prev( void* p ) const;
		int item_width( void* p ) const;
	private:
		void clear();
		void append( file_item* f );
		string m_folder;
		file_item* m_items;
		file_item* m_last;
		file_item* m_item_selected;
};

} /* namespace nle */

#endif /* _MEDIA_BROWSER_H_ */
