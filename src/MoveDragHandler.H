/*  MoveDragHandler.H
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

#ifndef _MOVE_DRAG_HANDLER_H_
#define _MOVE_DRAG_HANDLER_H_

#include "Rect.H"
#include "DragHandler.H"

namespace nle
{
class Clip;
class TimelineView;

class MoveDragHandler : public DragHandler
{
	public:
		MoveDragHandler( TimelineView *tlv,
				Clip *clip,
				int x, int y,
				const Rect& rect );
		virtual ~MoveDragHandler() {}
		void OnDrag( int x, int y );
		void OnDrop( int x, int y );
	private:
		Rect m_rect;
		int m_x;
		int m_y;
};

	
} /* namespace nle */

#endif /* _MOVE_DRAG_HANDLER_H_ */

