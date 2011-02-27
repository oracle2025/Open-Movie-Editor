/*  SelectDragHandler.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "SelectDragHandler.H"
#include "TimelineView.H"

namespace nle
{

SelectDragHandler::SelectDragHandler( int x, int y )
	: DragHandler( 0, 0 ), m_x( x ), m_y( g_timelineView->y() + y )
{
}
SelectDragHandler::~SelectDragHandler()
{
}
void SelectDragHandler::OnDrag( int x, int y )
{
	g_timelineView->window()->make_current();
	fl_overlay_rect( m_x, m_y, x - m_x, (g_timelineView->y() + y) - m_y );
}
void SelectDragHandler::OnDrop( int x, int y )
{
	g_timelineView->window()->make_current();
	fl_overlay_clear();
	g_timelineView->select_clips( m_x, m_y, x, y + g_timelineView->y() );
	g_timelineView->redraw();
}

} /* namespace nle */

