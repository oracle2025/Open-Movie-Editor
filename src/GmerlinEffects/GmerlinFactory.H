/*  GmerlinFactory.H
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _GMERLIN_FACTORY_H_
#define _GMERLIN_FACTORY_H_

#include <gavl/gavl.h>
extern "C" {
	#include <gmerlin/pluginregistry.h>
}

namespace nle
{

class IEffectMenu;
class FilterFactory;

class GmerlinFactory
{
	public:
		GmerlinFactory( IEffectMenu* menu );
		~GmerlinFactory();
		FilterFactory* get( const char* name );
	private:
		bg_cfg_registry_t*    m_cfg_registry;
		bg_cfg_section_t*     m_plugin_cfg;
		bg_plugin_registry_t* m_plugin_registry;
		char** m_filters;
};

} /* namespace nle */

#endif /* _GMERLIN_FACTORY_H_ */
