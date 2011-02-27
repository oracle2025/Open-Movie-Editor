/* Frei0rFactoryPlugin.cxx
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include <dlfcn.h>
#include "Frei0rFactoryPlugin.H"
#include "Frei0rEffect.H"
#include "timeline/Clip.H"
#include "VideoEffectClip.H"

namespace nle
{

Frei0rFactoryPlugin::Frei0rFactoryPlugin( string filename )
{
	m_ok = false;
	f0r_init = 0;
	f0r_deinit = 0;
	m_handle = dlopen( filename.c_str(), RTLD_NOW );
	if ( !m_handle ) { return; }
	
	f0r_init = (f0r_init_f)dlsym( m_handle, "f0r_init" );
	if ( !f0r_init ) { return; }
	
	f0r_deinit = (f0r_deinit_f)dlsym( m_handle, "f0r_deinit" );
	if ( !f0r_deinit ) { return; }
	
	f0r_get_plugin_info = (f0r_get_plugin_info_f)dlsym( m_handle, "f0r_get_plugin_info");
	
	if ( f0r_init() == 0 ) {
		return;
	}
	f0r_get_plugin_info( &m_info );
	if ( m_info.plugin_type == F0R_PLUGIN_TYPE_FILTER ) {
		m_ok = true;
	}
}

Frei0rFactoryPlugin::~Frei0rFactoryPlugin()
{
	if ( f0r_deinit ) {
		f0r_deinit();
	}
	if ( m_handle ) {
		dlclose( m_handle );
	}
}

FilterBase* Frei0rFactoryPlugin::get( Clip* clip )
{
	VideoEffectClip* effectClip = dynamic_cast<VideoEffectClip*>(clip);
	if ( !effectClip ) {
		return 0;
	}
	Frei0rEffect* effect = new Frei0rEffect( &m_info, m_handle, effectClip->w(), effectClip->h() );
	return effect;

}
/*
IVideoEffect* Frei0rFactoryPlugin::get( int w, int h )
{
	Frei0rEffect* effect = new Frei0rEffect( &m_info, m_handle, w, h );
	return effect;
}*/
const char* Frei0rFactoryPlugin::name()
{
	return m_info.name;
}
const char* Frei0rFactoryPlugin::identifier()
{
	string result = "effect:frei0r:";
	result += name();
	return result.c_str(); //TODO: this is not OK?
}

} /* namespace nle */
