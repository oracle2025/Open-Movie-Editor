
#include <dlfcn.h>
#include "NodeFilterFrei0rFactoryPlugin.H"
#include "Frei0rNode.H"
#include <iostream>
#include <cstring>


NodeFilterFrei0rFactoryPlugin::NodeFilterFrei0rFactoryPlugin( std::string filename )
{
	m_ok = false;
	f0r_init = 0;
	f0r_deinit = 0;
	m_handle = dlopen( filename.c_str(), RTLD_NOW );
	if ( !m_handle ) { std::cout << dlerror() << std::endl;return; }
	
	f0r_init = (f0r_init_f)dlsym( m_handle, "f0r_init" );
	if ( !f0r_init ) { return; }
	
	f0r_deinit = (f0r_deinit_f)dlsym( m_handle, "f0r_deinit" );
	if ( !f0r_deinit ) { return; }
	
	f0r_get_plugin_info = (f0r_get_plugin_info_f)dlsym( m_handle, "f0r_get_plugin_info");
	
	if ( f0r_init() == 0 ) {
		return;
	}
	f0r_get_plugin_info( &m_info );
	m_ok = true;
}

NodeFilterFrei0rFactoryPlugin::~NodeFilterFrei0rFactoryPlugin()
{
	if ( f0r_deinit ) {
		f0r_deinit();
	}
	if ( m_handle ) {
		dlclose( m_handle );
	}
}

Frei0rNode* NodeFilterFrei0rFactoryPlugin::get( int w, int h )
{
	Frei0rNode* effect = new Frei0rNode( &m_info, m_handle, w, h );
	return effect;

}
const char* NodeFilterFrei0rFactoryPlugin::name()
{
	return m_info.name;
}
const char* NodeFilterFrei0rFactoryPlugin::identifier()
{
	static char buffer[255];
	std::string result = "effect:frei0r:";
	result += name();
	strcpy( buffer, result.c_str() );
	return buffer;
}
