
#include "color_schemes.H"
#include <string.h>
#include <FL/Fl.H>
#include <FL/Fl_Tooltip.H>


static char* current_scheme = 0;

const char* get_current_scheme()
{
	return current_scheme;
}

void set_scheme( const char* c )
{
	if ( strcmp( "dark_shark", c ) == 0 ) {
		dark_shark();
		return;
	} else if ( strcmp( "deep_purple", c ) == 0 ) {
		deep_purple();
		return;
	} else if ( strcmp( "shiny_plastic", c ) == 0 ) {
		shiny_plastic();
		return;
	}
}

void dark_shark()
{
	Fl_Tooltip::color( fl_rgb_color( 255, 255, 255 ) );
	Fl::background2( 153, 153, 153 );
	Fl::background( 102, 102, 102 );
	Fl::foreground( 255, 255, 255 );
	Fl::set_color( FL_BLACK, 51, 51, 51 );
	Fl::scheme("shark");
	current_scheme = "dark_shark";
}

void deep_purple()
{
	Fl_Tooltip::color( fl_rgb_color( 0, 0, 1 ) );
	Fl::background2( 34, 52, 103 );
	Fl::background( 93, 93, 114 );
	Fl::foreground( 255, 255, 255 );
	Fl::set_color( FL_BLACK, 200, 200, 200 );
	Fl::scheme( "none" );
	current_scheme = "deep_purple";
}

void shiny_plastic()
{
	Fl_Tooltip::color( fl_rgb_color( 255, 255, 191 ) );
	Fl::background2( 255, 255, 255 );
	Fl::background( 192, 192, 192);
	Fl::foreground( 0, 0, 0 );
	Fl::set_color( FL_BLACK, 0, 0, 0 );
	Fl::scheme("plastic");
	current_scheme = "shiny_plastic";
}

