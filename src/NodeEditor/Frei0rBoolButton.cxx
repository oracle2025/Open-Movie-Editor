		
#include "Frei0rBoolButton.H"
#include <FL/Fl_Color_Chooser.H>
#include "VideoViewGL.H"
static void frei0r_bool_button_callback( Frei0rBoolButton* o, void* data )
{
	o->toggle_value();
}
		
Frei0rBoolButton::Frei0rBoolButton( int x, int y, int w, int h, const char *l )
	: Fl_Light_Button( x, y, w, h, l )
{
	m_instance = 0;
	m_param_index = 0;
	type(5);
	callback( (Fl_Callback*)frei0r_bool_button_callback );
}
Frei0rBoolButton::~Frei0rBoolButton()
{
}
void Frei0rBoolButton::toggle_value()
{
	if ( !m_instance ) {
		return;
	}
	value( !value() );
	f0r_param_bool bvalue = (double)value();
	f0r_set_param_value( m_instance, &bvalue, m_param_index );
	nle::g_videoView->redraw();
}
void Frei0rBoolButton::set_instance( f0r_instance_t i, f0r_set_param_value_f s, f0r_get_param_value_f g, int param_index )
{
	m_instance = i;
	f0r_set_param_value = s;
	f0r_get_param_value = g;
	m_param_index = param_index;

	f0r_param_bool bvalue;
	f0r_get_param_value( m_instance, &bvalue, param_index );
	value( (int)(bvalue >= 0.5) );
}


