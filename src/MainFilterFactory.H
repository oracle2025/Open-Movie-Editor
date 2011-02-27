/*  MainFilterFactory.H
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

#ifndef _MAIN_FILTER_FACTORY_H_
#define _MAIN_FILTER_FACTORY_H_

#include <string>
using namespace std;

namespace nle
{

class FilterFactory;

typedef struct _filter_factory_node {
	struct _filter_factory_node* next;
	FilterFactory* factory;
	string identifier;
} filter_factory_node;

class MainFilterFactory;
class IEffectMenu;
extern MainFilterFactory* g_mainFilterFactory;

class MainFilterFactory
{
	public:
		MainFilterFactory();
		~MainFilterFactory();
		void add( const char* identifier, FilterFactory* factory );
		void addAll( IEffectMenu* menu );
		FilterFactory* get( const char* identifier );
	private:
		filter_factory_node* m_factories;	
};
	
} /* namespace nle */

#endif /* _MAIN_FILTER_FACTORY_H_ */
