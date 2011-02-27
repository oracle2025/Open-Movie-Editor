/*  GmerlinFactory.cxx
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

#include "GmerlinFactory.H"
#include "GmerlinFactoryPlugin.H"
#include "MainFilterFactory.H"
#include "nle.h"

#include <iostream>

namespace nle
{

GmerlinFactory::GmerlinFactory( IEffectMenu* menu )
{
	m_cfg_registry = bg_cfg_registry_create();
	m_plugin_cfg = bg_cfg_registry_find_section( m_cfg_registry, "plugins" );
	m_plugin_registry = bg_plugin_registry_create( m_plugin_cfg );
	m_filters = bg_plugin_registry_get_plugins( m_plugin_registry, BG_PLUGIN_FILTER_VIDEO, BG_PLUGIN_ALL );

	for ( int i = 0; m_filters[i]; i++ ) {

		const bg_plugin_info_t* plugin_info = bg_plugin_find_by_name ( m_plugin_registry, m_filters[i] );
		bg_plugin_handle_t* plugin_handle = bg_plugin_load( m_plugin_registry, plugin_info );
		GmerlinFactoryPlugin* effect = new GmerlinFactoryPlugin( plugin_handle );
		//TODO: Those are not deleted !!!
		menu->addEffect( effect );
		g_mainFilterFactory->add( effect->identifier(), effect );
		g_ui->special_clips->add( effect->name(), PL_VIDEO_EFFECT, effect->identifier() );


	}

}

GmerlinFactory::~GmerlinFactory()
{
	bg_plugin_registry_free_plugins( m_filters );
	bg_plugin_registry_destroy( m_plugin_registry );
	bg_cfg_registry_destroy( m_cfg_registry );
}

FilterFactory* GmerlinFactory::get( const char* name )
{
	const bg_plugin_info_t* plugin_info = bg_plugin_find_by_name( m_plugin_registry, name );
	bg_plugin_handle_t* plugin_handle = bg_plugin_load( m_plugin_registry, plugin_info );
	return new GmerlinFactoryPlugin( plugin_handle );
}

} /* namespace nle */
