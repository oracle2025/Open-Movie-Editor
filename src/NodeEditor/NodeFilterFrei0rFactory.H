#ifndef _NODE_FILTER_FREI0R_FACTORY_H_
#define _NODE_FILTER_FREI0R_FACTORY_H_

#include <string>

class NodeFilterFrei0rFactoryPlugin;

typedef struct _effect_list_node {
	struct _effect_list_node* next;
	NodeFilterFrei0rFactoryPlugin* effect;
} effect_node;

class NodeFilterFrei0rFactory;
extern NodeFilterFrei0rFactory* g_node_filter_frei0r_factory;

class NodeFilterFrei0rFactory
{
	public:
		NodeFilterFrei0rFactory();
		~NodeFilterFrei0rFactory();
		NodeFilterFrei0rFactoryPlugin* get( std::string name );
		effect_node* m_effects;
	private:
		void enumerate( std::string folder );
};

#endif /* _NODE_FILTER_FREI0R_FACTORY_H_ */
