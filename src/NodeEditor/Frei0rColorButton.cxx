		
#include "Frei0rColorButton.H"
#include <FL/Fl_Color_Chooser.H>
#include "VideoViewGL.H"
static void frei0r_color_button_callback( Frei0rColorButton* o, void* data )
{
	o->select_color();
}
		
Frei0rColorButton::Frei0rColorButton( int x, int y, int w, int h, const char *l )
	: Fl_Button( x, y, w, h, l )
{
	m_instance = 0;
	m_param_index = 0;
	type(5);
	callback( (Fl_Callback*)frei0r_color_button_callback );
}
Frei0rColorButton::~Frei0rColorButton()
{
}
void Frei0rColorButton::select_color()
{
	if ( !m_instance ) {
		return;
	}
	f0r_param_color_t colors;
	f0r_get_param_value( m_instance, &colors, m_param_index );
	double r, g, b;
	r = colors.b;
	g = colors.g;
	b = colors.r;
	if ( fl_color_chooser( "Choose Color", r, g, b ) ) {
		colors.r = b;
		colors.g = g;
		colors.b = r;
		f0r_set_param_value( m_instance, &colors, m_param_index );
		color( fl_rgb_color( (uchar)( r * 255 ), (uchar)( g * 255), (uchar)( b * 255 ) ) );
		labelcolor( fl_contrast( FL_FOREGROUND_COLOR, color() ) );
		nle::g_videoView->redraw();
	}
}
void Frei0rColorButton::set_instance( f0r_instance_t i, f0r_set_param_value_f s, f0r_get_param_value_f g, int param_index )
{
	m_instance = i;
	f0r_set_param_value = s;
	f0r_get_param_value = g;
	m_param_index = param_index;

	f0r_param_color_t colors;
	f0r_get_param_value( m_instance, &colors, param_index );
	color( fl_rgb_color( (uchar)( colors.b * 255 ), (uchar)( colors.g * 255), (uchar)( colors.r * 255 ) ) );
	labelcolor( fl_contrast( FL_FOREGROUND_COLOR, color() ) );
}


