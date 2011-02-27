#ifndef _NODE_FILTER_FREI0R_FACTORY_PLUGIN_H_
#define _NODE_FILTER_FREI0R_FACTORY_PLUGIN_H_

#include "frei0r.h"
#include "INodeFilterFactoryPlugin.H"
#include "Frei0rNode.H"

#include <string>

typedef void (*f0r_deinit_f)(void);
typedef int (*f0r_init_f)(void);
typedef void (*f0r_get_plugin_info_f)(f0r_plugin_info_t *info);

class Frei0rNode;

class NodeFilterFrei0rFactoryPlugin : public INodeFilterFactoryPlugin
{
	public:
		NodeFilterFrei0rFactoryPlugin( std::string filename );
		~NodeFilterFrei0rFactoryPlugin();
		Frei0rNode* get( int w, int h );
		INode* get_i_node( int w, int h ) { return (Frei0rNode*)get( w, h ); }
		bool ok() { return m_ok; }
		const char* name();
		const char* identifier();
	private:
		f0r_init_f            f0r_init;
		f0r_deinit_f          f0r_deinit;
		f0r_get_plugin_info_f f0r_get_plugin_info;
		void*                 m_handle;
		f0r_plugin_info_t     m_info;
		bool                  m_ok;
};

#endif /* _NODE_FILTER_FREI0R_FACTORY_PLUGIN_H_ */
