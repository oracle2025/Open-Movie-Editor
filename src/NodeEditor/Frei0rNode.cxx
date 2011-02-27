#include <dlfcn.h>
#include "Frei0rNode.H"
#include "Frei0rDoubleSlider.H"
#include "Frei0rColorButton.H"
#include "Frei0rBoolButton.H"
#include <tinyxml.h>
#include <string>
#include <FL/Fl_Group.H>


Frei0rNode::Frei0rNode( f0r_plugin_info_t* info, void* handle, int w, int h)
{
	f0r_construct = (f0r_construct_f)dlsym( handle, "f0r_construct" );
	f0r_destruct = (f0r_destruct_f)dlsym( handle, "f0r_destruct" );
	f0r_update = (f0r_update_f)dlsym( handle, "f0r_update" );
	f0r_update2 = (f0r_update2_f)dlsym( handle, "f0r_update2" );
	f0r_get_param_info = (f0r_get_param_info_f)dlsym( handle, "f0r_get_param_info" );
	f0r_set_param_value = (f0r_set_param_value_f)dlsym( handle, "f0r_set_param_value" );
	f0r_get_param_value = (f0r_get_param_value_f)dlsym( handle, "f0r_get_param_value" );
	m_info = info;
	m_instance = f0r_construct( w, h );
	m_frame = new uint32_t[w*h];
}
Frei0rNode::~Frei0rNode()
{
	if ( f0r_destruct ) {
		f0r_destruct( m_instance );
	}
	delete_widgets();
	delete [] m_frame;
}
void Frei0rNode::setInput( int input, INode* node )
{
}
int Frei0rNode::getInputCount()
{
	switch ( m_info->plugin_type ) {
		case F0R_PLUGIN_TYPE_FILTER:
			return 1;
		case F0R_PLUGIN_TYPE_SOURCE:
			return 0;
		case F0R_PLUGIN_TYPE_MIXER2:
			return 2;
		case F0R_PLUGIN_TYPE_MIXER3:
			return 3;
	};
	return 0;
}
int Frei0rNode::getOutputCount()
{
	return 1;
}
void Frei0rNode::init_widgets()
{
	f0r_param_info param_info;
	int k = 0;
	for ( int i = 0; i < m_info->num_params; i++ ) {
		f0r_get_param_info( &param_info, i );
		if ( param_info.type == F0R_PARAM_DOUBLE ) {
			Frei0rDoubleSlider* slider = new Frei0rDoubleSlider( 0, 0, 25, 25 );
			slider->tooltip( param_info.explanation );
			slider->set_instance( m_instance, f0r_set_param_value, f0r_get_param_value, i );
			slider->show();
			node->widgets[k] = slider;
			k++;
		} else if ( param_info.type == F0R_PARAM_COLOR ) {
			Frei0rColorButton* button = new Frei0rColorButton( 0, 0, 25, 25, param_info.name );
			button->tooltip( param_info.explanation );
			button->set_instance( m_instance, f0r_set_param_value, f0r_get_param_value, i );
			button->show();
			node->widgets[k] = button;
			k++;
		} else if ( param_info.type == F0R_PARAM_BOOL ) {
			Frei0rBoolButton* button = new Frei0rBoolButton( 0, 0, 25, 25, param_info.name );
			button->tooltip( param_info.explanation );
			button->set_instance( m_instance, f0r_set_param_value, f0r_get_param_value, i );
			button->show();
			node->widgets[k] = button;
			k++;
		}
	}
	node->widgets[k] = 0;
}
void Frei0rNode::delete_widgets()
{
	for ( int i = 0; node->widgets[i]; i++ ) {
		node->widgets[i]->parent()->remove( node->widgets[i] );
		delete node->widgets[i];
		node->widgets[i] = 0;
	}
}
void Frei0rNode::init()
{
}

uint32_t* Frei0rNode::getFrame( int output, double position )
{
	uint32_t* input_frame[3];
	for ( int i = 0; i < node->input_count && i < 3; i++ ) {
		if ( node->inputs[i] == 0 || !node->inputs[i]->node ) {
			return 0;
		}
		input_frame[i] = node->inputs[i]->node->getFrame( 0, position );
		if ( !input_frame[i] ) {
			return 0;
		}
	}
	switch ( m_info->plugin_type ) {
		case F0R_PLUGIN_TYPE_FILTER:
			f0r_update( m_instance, position , input_frame[0], m_frame );
			return m_frame;
		case F0R_PLUGIN_TYPE_SOURCE:
			f0r_update( m_instance, position , 0, m_frame );
			return m_frame;
		case F0R_PLUGIN_TYPE_MIXER2:
			f0r_update2( m_instance, position , input_frame[0], input_frame[1], 0, m_frame );
			return m_frame;
		case F0R_PLUGIN_TYPE_MIXER3:
			f0r_update2( m_instance, position , input_frame[0], input_frame[1], input_frame[2], m_frame );
			return m_frame;
	};
	return 0;
}
void Frei0rNode::readXML( TiXmlElement* xml_node )
{
	TiXmlElement* parameterXml = TiXmlHandle( xml_node ).FirstChildElement( "parameter" ).Element();
	for ( ; parameterXml; parameterXml = parameterXml->NextSiblingElement( "parameter" ) ) {
		std::string paramName = parameterXml->Attribute( "name" );
		f0r_param_info_t pinfo;
		for ( int i = 0; i < m_info->num_params; i++ ) {
			f0r_get_param_info( &pinfo, i );
			if ( paramName == pinfo.name ) {
				switch ( pinfo.type ) {
					case F0R_PARAM_DOUBLE:
						{
							double dval;
							f0r_param_double dvalue;
							parameterXml->Attribute( "value", &dval );
							dvalue = dval;
							f0r_set_param_value( m_instance, &dvalue, i );
							break;
						}
					case F0R_PARAM_BOOL:
						{
							int bval;
							f0r_param_bool bvalue;
							parameterXml->Attribute( "value", &bval );
							bvalue = (double)bval;
							f0r_set_param_value( m_instance, &bvalue, i );
							break;
						}
					case F0R_PARAM_COLOR:
						{
							double r = 0;
							double g = 0;
							double b = 0;
							parameterXml->Attribute( "r", &r );
							parameterXml->Attribute( "g", &g );
							parameterXml->Attribute( "b", &b );
							f0r_param_color_t cvalue;
							cvalue.r = r;
							cvalue.g = g;
							cvalue.b = b;
							f0r_set_param_value( m_instance, &cvalue, i );
							break;
						}
				}
				break;
			}
		}
	}

}
void Frei0rNode::writeXML( TiXmlElement* xml_node )
{
	TiXmlElement* parameter;
	TiXmlElement* effect = xml_node;
	f0r_param_info_t pinfo;
	for ( int i = 0; i < m_info->num_params; i++ ) {
		f0r_get_param_info( &pinfo, i );
		parameter = new TiXmlElement( "parameter" );
		effect->LinkEndChild( parameter );
		parameter->SetAttribute( "name", pinfo.name );
		switch ( pinfo.type ) {
			case F0R_PARAM_DOUBLE: //Seems to be always between 0.0 and 1.0
				{
					f0r_param_double dvalue;
					f0r_get_param_value( m_instance, &dvalue, i );
					parameter->SetDoubleAttribute( "value", (double)dvalue );
					break;
				}
			case F0R_PARAM_BOOL:
				{
					f0r_param_bool bvalue;
					f0r_get_param_value( m_instance, &bvalue, i );
					parameter->SetAttribute( "value", (int)(bvalue >= 0.5) );
					break;
				}
			case F0R_PARAM_COLOR:
				{
					f0r_param_color_t cvalue;
					f0r_get_param_value( m_instance, &cvalue, i );
					parameter->SetDoubleAttribute( "r", cvalue.r );
					parameter->SetDoubleAttribute( "g", cvalue.g );
					parameter->SetDoubleAttribute( "b", cvalue.b );
					break;
				}
			case F0R_PARAM_POSITION:
				{
					f0r_param_position_t pos;
					f0r_get_param_value( m_instance, &pos, i );
					parameter->SetDoubleAttribute( "x", pos.x );
					parameter->SetDoubleAttribute( "y", pos.y );
					break;
				}
			default:
				break;

		}
	}
}
const char* Frei0rNode::identifier()
{
	std::string identifierstring = "effect:frei0r:";
	identifierstring += m_info->name;
	return identifierstring.c_str();
}

