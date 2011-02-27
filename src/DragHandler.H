/*  DragHandler.H
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

#ifndef _DRAG_HANDLER_H_
#define _DRAG_HANDLER_H_

#include "Rect.H"

namespace nle
{
class Clip;
class TimelineView;

class DragHandler
{
	public:
		virtual ~DragHandler(){}
		virtual void OnDrag( int x, int y ) = 0;
		virtual void OnDrop( int x, int y ) = 0;
	protected:
		DragHandler( TimelineView *tlv, Clip *clip ) : m_tlv( tlv ), m_clip( clip ) {}
		TimelineView *m_tlv;
		Clip *m_clip;
};

	
} /* namespace nle */

#endif /* _DRAG_HANDLER_H_ */

