/*  MainFilterFactory.cxx
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

#include "MainFilterFactory.H"
#include "IEffectMenu.H"
#include "sl/sl.h"

#include <iostream>

namespace nle
{

MainFilterFactory* g_mainFilterFactory;
	
MainFilterFactory::MainFilterFactory()
{
	m_factories = 0;
	g_mainFilterFactory = this;
}
MainFilterFactory::~MainFilterFactory()
{
	filter_factory_node* node;
	while ( ( node = (filter_factory_node*)sl_pop( &m_factories ) ) ) {
		delete node;
	}

}
void MainFilterFactory::add( const char* identifier, FilterFactory* factory )
{
	filter_factory_node* n = new filter_factory_node;
	n->next = 0;
	n->identifier = identifier;
	n->factory = factory;
	m_factories = (filter_factory_node*)sl_push( m_factories, n );
}
FilterFactory* MainFilterFactory::get( const char* identifier )
{
	filter_factory_node* p;
	for ( p = m_factories; p; p = p->next ) {
		if ( identifier == p->identifier ) {
			return p->factory;
		}
	}
	return 0;

}
void MainFilterFactory::addAll( IEffectMenu* menu )
{
	filter_factory_node* p;
	for ( p = m_factories; p; p = p->next ) {
		menu->addEffect( p->factory );
	}
}


} /* namespace nle */
