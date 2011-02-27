#include <FL/Fl.H>
#include <FL/Fl_Box.H>

namespace nle
{

class DragBox : public Fl_Box
{
	public:
		DragBox( int x, int y, int w, int h, const char* l = 0 );
		~DragBox();
		int handle( int event );
};

} /* namespace nle */
