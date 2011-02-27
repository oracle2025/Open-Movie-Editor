/*  FilterClip.cxx
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

#include <cassert>
#include "sl/sl.h"
#include "FilterClip.H"
#include "FilterBase.H"
#include "FilterFactory.H"

namespace nle
{

FilterClip::FilterClip( Track* track, int64_t position, int id )
	: Clip( track, position, id )
{
	m_filters = 0;
}
FilterClip::~FilterClip()
{
	filter_stack* node;
	while ( ( node = (filter_stack*)sl_pop( &m_filters ) ) ) {
		delete node->filter;
		delete node;
	}
}
void FilterClip::reset()
{
	for ( filter_stack* node = m_filters; node; node = node->next ) {
		node->filter->reset();
	}
}
//TODO: Either push or append should be removed
FilterBase* FilterClip::pushFilter( FilterFactory* factory )
{
	//TODO change FilterFactory to FilterClip
	FilterBase* filter = factory->get( this );
	if ( !filter ) {
		return 0;
	}
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = filter;
	m_filters = (filter_stack*)sl_push( m_filters, n );
	return filter;
}
FilterBase* FilterClip::appendFilter( FilterFactory* factory )
{
	FilterBase* filter = factory->get( this );
	if ( !filter ) {
		return 0;
	}
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = filter;
	m_filters = (filter_stack*)sl_unshift( m_filters, n );
	return filter;
}
FilterBase* FilterClip::insertFilter( FilterFactory* factory, int position )
{
	FilterBase* filter = factory->get( this );
	if ( !filter ) {
		return 0;
	}
	filter_stack* n = new filter_stack;
	n->next = 0;
	n->filter = filter;
	if ( position == 1 ) {
		n->next = m_filters;
		m_filters = n;
	} else {
		filter_stack* c = m_filters;
		for ( int i = 2; i < position; i++ ) {
			c = c->next;
		}
		n->next = c->next;
		c->next = n;
	}
	return filter;
}
static filter_stack* sl_swap( filter_stack* root )
{
	filter_stack* q = root;
	filter_stack* r;
	if ( !q || !q->next ) {
		return q;
	}
	r = q->next;
	q->next = r->next;
	r->next = q;
	return r;
}


void FilterClip::moveFilterUp( int num )
{
	if ( !m_filters || num <= 1 ) {
		return;
	}
	if ( num == 2 ) {
		m_filters = sl_swap( m_filters );
		return;
	}
	filter_stack* p = m_filters;
	for ( int i = 3; i < num; i++ ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );

}
void FilterClip::moveFilterUp( FilterBase* filter )
{
	if ( !m_filters || filter == m_filters->filter ) {
		return;
	}
	if ( m_filters->next && m_filters->next->filter == filter ) {
		m_filters = sl_swap( m_filters );
		return;
	}
	filter_stack* p = m_filters;
	for ( ; p->next && p->next->next && p->next->next->filter != filter ; ) {
		p = p->next;
	}
	if ( p->next && p->next->next ) {
		p->next = sl_swap( p->next );
	}
}
void FilterClip::moveFilterDown( int num )
{
	if ( !m_filters ) {
		return;
	}
	if ( num == 1 ) {
		m_filters = sl_swap( m_filters );
		return;
	}
	filter_stack* p = m_filters;
	for ( int i = 2; i < num; i++ ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );

}
void FilterClip::moveFilterDown( FilterBase* filter )
{
	if ( !m_filters ) {
		return;
	}
	if ( m_filters->filter == filter ) {
		m_filters = sl_swap( m_filters );
		return;
	}
	filter_stack* p = m_filters;
	for ( ; p->next && p->next->filter != filter ; ) {
		p = p->next;
	}
	p->next = sl_swap( p->next );

}
static int remove_filter_helper( void*, void* data )
{
	int* num = (int*)data;
	if ( *num ) {
		(*num)--;
		return 0;
	}
	return 1;
}

void FilterClip::removeFilter( int num )
{
	int count = num - 1;
	filter_stack* node = (filter_stack*)sl_remove( &m_filters, remove_filter_helper, &count );
	if ( node ) {
		delete node->filter;
		delete node;
	}

}
FilterBase* FilterClip::getFilter( int num )
{
	filter_stack* node = m_filters;
	for ( int i = 1; i < num; i++ ) {
		assert( node );
		node = node->next;
	}
	assert( node );
	return node->filter;
}

void FilterClip::popFilter()
{
	filter_stack* node = (filter_stack*)sl_shift( &m_filters );
	if ( node ) {
		delete node->filter;
		delete node;
	}
}
	
} /* namespace nle */

