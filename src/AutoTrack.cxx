/*  AutoTrack.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "AutoTrack.H"
#include "sl/sl.h"
#include <iostream>

namespace nle
{

AutoTrack::AutoTrack( int num, string name )
	: Track( num, name )
{
	m_automationPoints = 0;
	auto_node* r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 40*NLE_TIME_BASE;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 30*NLE_TIME_BASE;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 20*NLE_TIME_BASE;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 10*NLE_TIME_BASE;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 0;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	m_autoCache = 0;

}

AutoTrack::~AutoTrack()
{
	auto_node* node;
	while ( ( node = (auto_node*)sl_pop( &m_automationPoints ) ) ) {
		delete node;
	}	
}
double AutoTrack::getValue( int64_t position )
{
	int64_t pPos = position;
	if ( !m_autoCache ) {
		m_autoCache = m_automationPoints;
	}
	if ( m_autoCache && m_autoCache->x <= pPos &&  m_autoCache->next && m_autoCache->next->x > pPos ) {
		int64_t diff = m_autoCache->next->x - m_autoCache->x;
		float diff_y = m_autoCache->next->y - m_autoCache->y;
		float inc = diff_y / diff;
		return ( inc * ( pPos - m_autoCache->x ) ) + m_autoCache->y;
	} else if ( m_autoCache == m_automationPoints && m_autoCache->x > pPos ) {
		return m_autoCache->y;
	} else if ( m_autoCache ) {
			if ( !m_autoCache->next ) {
				return m_autoCache->y;
			} else {
				m_autoCache = m_autoCache->next;
				return getValue( position );
			}
	} else {
		std::cout << "NOTHING" << std::endl;
	}
	return 0.0;

}
void AutoTrack::reset()
{
	m_autoCache = m_automationPoints;
}



} /* namespace nle */
