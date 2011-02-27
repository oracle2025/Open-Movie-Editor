#ifndef _FREIOR_NODE_H_
#define _FREIOR_NODE_H_

#include "frei0r.h"
#include "Frei0rGraphEditor.H"
#include "INode.H"

typedef f0r_instance_t (*f0r_construct_f)(unsigned int, unsigned int);
typedef void (*f0r_destruct_f)(f0r_instance_t);
typedef void (*f0r_update_f)(f0r_instance_t, double, const uint32_t*, uint32_t*);
typedef void (*f0r_update2_f)(f0r_instance_t, double, const uint32_t*, const uint32_t*, const uint32_t*, uint32_t*);
typedef void (*f0r_get_param_info_f)(f0r_param_info_t *info, int param_index);
typedef void (*f0r_set_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);
typedef void (*f0r_get_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);  




class Frei0rNode : public INode
{
	public:
		Frei0rNode( f0r_plugin_info_t* info, void* handle, int w, int h);
		~Frei0rNode();
		void setInput( int input, INode* node );
		int getInputCount();
		int getOutputCount();
		uint32_t* getFrame( int output, double position );
		void init();
		void init_widgets();
		void delete_widgets();
		void readXML( TiXmlElement* xml_node );
		void writeXML( TiXmlElement* xml_node );
		const char* identifier();
	private:
		f0r_instance_t m_instance;
		f0r_construct_f f0r_construct;
		f0r_destruct_f f0r_destruct;
		f0r_update_f f0r_update;
		f0r_update2_f f0r_update2;
		f0r_get_param_info_f  f0r_get_param_info;
		f0r_set_param_value_f f0r_set_param_value;
		f0r_get_param_value_f f0r_get_param_value;
		f0r_plugin_info_t* m_info;
		uint32_t* m_frame;
};

#endif /* _FREIOR_NODE_H_ */

