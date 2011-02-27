/*  Ruler.H
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

#ifndef _RULER_H_
#define _RULER_H_

#include <FL/Fl_Widget.H>

#include "Rect.H"

namespace nle
{

class Ruler : public Fl_Widget
{
	public:
		Ruler( int x, int y, int w, int h, const char *label = 0 );
		int handle( int event );
		void stylus( long stylus_pos );
  		void draw_stylus();
		void skipForward();
		void skipBackward();
		void skipFirst();
		void skipLast();
	protected:
		void draw();
	private:
		Rect m_stylus;
};


} /* namespace nle */
#endif /* _RULER_H_ */



