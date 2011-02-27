#ifndef _CURVE_EDITOR_2_H_
#define _CURVE_EDITOR_2_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

struct point {
	int x;
	int y;
};

class CurveEditor2 : public Fl_Widget
{
	public:
		CurveEditor2( int x, int y, int w, int h, const char* label = 0 );
		~CurveEditor2();
		void draw();
		int handle( int event );
		void convert_from_display( int x, int y, int& out_x, int& out_y );
		void convert_to_display( int x, int y, int& out_x, int& out_y );
		void lineColor( Fl_Color c ) { m_lineColor = c; }
		unsigned char m_values[256];
		void set( int x1, int y1, int x2, int y2 );
		void get( int &x1, int &y1, int &x2, int &y2 );
		void calculate_values();
	private:
		int _x();
		int _y();
		int _w();
		int _h();
		point m_points[2];
		point* m_current;
		Fl_Color m_lineColor;
};

#endif /* _CURVE_EDITOR_2_H_ */

