/*  AutoDragHandler.cxx
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

#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "AutoDragHandler.H"
#include "globals.H"
#include "global_includes.H"
#include "TimelineView.H"
#include "AutoTrack.H"
#include "timeline/Track.H"
#include "DocManager.H"


namespace nle
{

static int g_x_off;
static int g_y_off;

static bool inside_node( auto_node* n, Rect& r, AutoTrack* track, int _x, int _y )
{
	int x = g_timelineView->get_screen_position( n->x, NLE_TIME_BASE ) - 5;
	int y = (int)( r.y + ( ( track->h() - 10 ) * ( 1.0 - n->y ) ) );
	int w = 10;
	int h = 10;
	g_x_off = x - _x;
	g_y_off = y - _y;
	Rect node( x, y, w, h );
	return node.inside( _x, _y );
}
static void screen_to_node( int64_t& x, float& y, int in_x, int in_y, auto_node* n, auto_node* n_before, Rect& r, AutoTrack* track )
{
	if ( in_x < r.x ) { in_x = r.x; }
	if ( in_x > r.x + r.w - 10 ) { in_x = r.x + r.w - 10; }
	if ( in_y < r.y ) { in_y = r.y; }
	if ( in_y > r.y + r.h - 10 ) { in_y = r.y + r.h - 10; }
	
	y = 1.0 - ( ((float)in_y - r.y) / ((float)r.h - 10) );
	x = g_timelineView->get_real_position( in_x + 5, NLE_TIME_BASE );
	if ( n->next && x > n->next->x - 1000 ) {
		x = n->next->x - 1000;
	} else if ( n_before && x < n_before->x + 1000 ) {
		x = n_before->x + 1000;
	}	

}

AutoDragHandler::AutoDragHandler( AutoTrack* track, const Rect& rect, int x_off, int y_off )
	: DragHandler( g_timelineView, 0 ), m_x_off( x_off ), m_y_off( y_off )
{
	m_dragging = false;
	m_outline = rect;
	m_autoTrack = track;
	m_nodesOriginal = track->getAutomationPoints();
	m_removed = false;
	auto_node* r = m_nodesOriginal;
	m_firstNode = false;
	m_lastNode = false;
	m_node_before = 0;
	int i = 0;
	for ( ; r; r = r->next ) {
		if ( r->next == 0 ) {
			m_lastNode = true;
		} else if ( r == m_nodesOriginal ) {
			m_firstNode = true;
		}
		if ( inside_node( r, m_outline, m_autoTrack, x_off, y_off ) ) {
			m_node = r;
			m_node_number = i;
			m_dragging = true;
			m_x_off = g_x_off;
			m_y_off = g_y_off;
			if ( !m_firstNode ) {
				auto_node* q = m_nodesOriginal;
				while ( q->next && q->next != m_node ) {
					q = q->next;
				}
				m_node_before = q;
			}
			break;
		}
		m_firstNode = m_lastNode = false;
		i++;
	}
}
AutoDragHandler::~AutoDragHandler()
{
}
void AutoDragHandler::OnDrag( int x, int y )
{
	if ( m_dragging ) {
		screen_to_node( m_node->x, m_node->y, x + m_x_off, y + m_y_off, m_node, m_node_before, m_outline, m_autoTrack );
		if ( !m_firstNode && ( y < m_outline.y - 30 || y > m_outline.y + m_outline.h + 30 ) ) {
			//remove Node
			m_node_before->next = m_node->next;
			delete m_node;
			m_node = 0;
			m_dragging = false;
			m_removed = true;
		}
		g_timelineView->redraw();
	}

}
void AutoDragHandler::OnDrop( int x, int y )
{

	if ( m_removed ) { return; }
	if ( !m_dragging && m_outline.inside( x, y ) ) {
		auto_node* r = m_nodesOriginal;
		int64_t _x = g_timelineView->get_real_position( x + 5, NLE_TIME_BASE );
		int i = 0;
		if ( !r->next ) {
			auto_node* p = new auto_node;
			p->next = 0;
			r->next = p;
			screen_to_node( p->x, p->y, x - 5, y - 5, p, r, m_outline, m_autoTrack );
			g_timelineView->redraw();
		} else {
			for ( ;r ; r = r->next ) {
				if ( r->x < _x && r->next && r->next->x > _x ) {
					auto_node* p = new auto_node;
					p->next = r->next;
					r->next = p;
					screen_to_node( p->x, p->y, x - 5, y - 5, p, r, m_outline, m_autoTrack );
					/*
					 * - 5 to place Point on the Center of the mouse
					 * cursor
					 */
					g_timelineView->redraw();
					break;
				} else if ( !r->next ) {
					auto_node* p = new auto_node;
					p->next = 0;
					r->next = p;
					screen_to_node( p->x, p->y, x - 5, y - 5, p, r, m_outline, m_autoTrack );
					g_timelineView->redraw();
					break;
				}
				i++;
			}
		}
	}
}
} /* namespace nle */
