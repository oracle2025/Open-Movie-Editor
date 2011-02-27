#include "DragBox.H"

namespace nle
{

DragBox::DragBox( int x, int y, int w, int h, const char* l )
	: Fl_Box( x, y, w, h, l )
{
}
DragBox::~DragBox()
{
}
int DragBox::handle( int event )
{
	switch ( event ) {
		case FL_DRAG:
			do_callback();
			// v  Fallthrough!
		case FL_PUSH:
		case FL_RELEASE:
			return 1;
	}
	return Fl_Box::handle( event );
}


} /* namespace nle */
