#include <cstdlib>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include "CurveEditor2.H"
CurveEditor2::CurveEditor2( int x, int y, int w, int h, const char* label )
	: Fl_Widget( x, y, w, h, label )
{
	m_points[0].x = 0;
	m_points[0].y = 255;
	m_points[1].x = 255;
	m_points[1].y = 0;
	m_current = 0;
	m_lineColor = FL_RED;
	for ( int i = 0; i < 256; i++ ) {
		m_values[i] = i;
	}
}

CurveEditor2::~CurveEditor2()
{
}
void CurveEditor2::draw()
{
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	
	fl_color( FL_BLACK );
	
/*	fl_begin_line();
	fl_curve( x() + m_points[0].x, y() + m_points[0].y, x() + m_points[1].x, y() + m_points[1].y,x() + m_points[2].x, y() + m_points[2].y,x() + m_points[3].x, y() + m_points[3].y );
	fl_end_line();*/

	fl_line_style( FL_SOLID, 1 );
	fl_color( FL_BLACK );
	fl_line( x(), y() + h() / 2, x() + w(), y() + h() / 2 );
	fl_line( x(), y() + h() / 4, x() + w(), y() + h() / 4 );
	fl_line( x(), y() + h() * 3 / 4, x() + w(), y() + h() * 3 / 4 );
	
	fl_line( x() + w() / 2, y(), x() + w() / 2, y() + h() );
	fl_line( x() + w() / 4, y(), x() + w() / 4, y() + h() );
	fl_line( x() + w() * 3 / 4 , y(), x() + w() * 3 / 4, y() + h() );
	
	
	fl_push_matrix();

	fl_translate( _x(), _y() );
	fl_scale( _w() / 256.0, _h() / 256.0 );

	fl_color( m_lineColor );
	fl_line_style( FL_SOLID, 2 );
	fl_begin_line();
	fl_vertex( -10.0, m_points[0].y );
	for ( int i = 0; i < 2; i++ ) {
		fl_vertex( m_points[i].x, m_points[i].y );
	}
	fl_vertex( 266.0, m_points[1].y );
	fl_end_line();
	
	//--8<---
/*	fl_color( FL_BLACK );
	fl_line_style( FL_SOLID, 1 );
	fl_begin_line();
	for ( int i = 0; i < 256; i++ ) {
		fl_vertex( i, 256 - m_values[i] );
	}
	fl_end_line();*/
	//--8<---


	
	fl_line_style( FL_SOLID, 1 );

	fl_pop_matrix();
	
	int x_display;
	int y_display;
	for ( int i = 0; i < 2; i++ ) {
		convert_to_display( m_points[i].x, m_points[i].y, x_display, y_display );
		fl_draw_box( FL_UP_BOX, _x() + x_display - 5, _y() + y_display - 5, 10, 10, m_lineColor );
	}
	
	fl_pop_clip();
}

int CurveEditor2::handle( int event )
{
	int x_rel = Fl::event_x() - _x();
	int y_rel = Fl::event_y() - _y();
	int x_point;
	int y_point;
	switch ( event ) {
		case FL_PUSH:
			{
				for ( int i = 0; i < 2; i++ ) {
					convert_to_display( m_points[i].x, m_points[i].y, x_point, y_point );
					if ( abs( x_point - x_rel ) < 10 && abs( y_point - y_rel ) < 10 ) {
						m_current = &m_points[i];
						convert_from_display( x_rel, y_rel, m_current->x, m_current->y );
						if ( m_current->x < 0 ) { m_current->x = 0; }
						if ( m_current->y < 0 ) { m_current->y = 0; }
						if ( m_current->x > 255 ) { m_current->x = 255; }
						if ( m_current->y > 255 ) { m_current->y = 255; }
						redraw();
						window()->cursor( FL_CURSOR_CROSS );
						return 1;
					}
				}
			}
			break;
		case FL_RELEASE:
			{
				if ( m_current ) {
					m_current = 0;
					calculate_values();
					do_callback();
					redraw();
					window()->cursor( FL_CURSOR_DEFAULT );
					return 1;
				}
			}
			break;
		case FL_DRAG:
			{
				if ( m_current ) {
					convert_from_display( x_rel, y_rel, m_current->x, m_current->y );
					if ( m_current->x < 0 ) { m_current->x = 0; }
					if ( m_current->y < 0 ) { m_current->y = 0; }
					if ( m_current->x > 255 ) { m_current->x = 255; }
					if ( m_current->y > 255 ) { m_current->y = 255; }
					if ( m_current == &m_points[0] && m_current->x > m_points[1].x ) {
						m_points[1].x = m_current->x;
					}
					if ( m_current == &m_points[1] && m_current->x < m_points[0].x ) {
						m_points[0].x = m_current->x;
					}
					if ( m_current == &m_points[0] && m_current->y < m_points[1].y ) {
						m_points[1].y = m_current->y;
					}
					if ( m_current == &m_points[1] && m_current->y > m_points[0].y ) {
						m_points[0].y = m_current->y;
					}
					calculate_values();
					do_callback();
					redraw();
					return 1;
				}
			}
			break;
		case FL_ENTER:
			return 1;
		case FL_LEAVE:
			window()->cursor( FL_CURSOR_DEFAULT );
			return 1;
		case FL_MOVE:
			{
				if ( !m_current ) {
					for ( int i = 0; i < 2; i++ ) {
						convert_to_display( m_points[i].x, m_points[i].y, x_point, y_point );
						if ( abs( x_point - x_rel ) < 10 && abs( y_point - y_rel ) < 10 ) {
							window()->cursor( FL_CURSOR_HAND );
							return 1;
						}
					}
					window()->cursor( FL_CURSOR_DEFAULT );
					return 1;
				}
			}
			break;
	}
	return Fl_Widget::handle( event );
}

void CurveEditor2::calculate_values()
{
	for ( int i = 0; i < m_points[0].x && i < 256; i++ ) {
		m_values[i] = 255 - m_points[0].y;
	}
	int A = m_points[1].x - m_points[0].x;
	int B = m_points[0].y - m_points[1].y;
	float C = (float)B / (float)A;
	for ( int i = m_points[0].x; i < m_points[1].x && i < 256; i++ ) {
		m_values[i] = 255 - m_points[0].y + (int)( (i-m_points[0].x) * C );
	}
	for ( int i = m_points[1].x; i < 256; i++ ) {
		m_values[i] = 255 - m_points[1].y;
	}
}


void CurveEditor2::convert_from_display( int x, int y, int& out_x, int& out_y )
{
	out_x = x * 256 / _w();
	out_y = y * 256 / _h();
}
void CurveEditor2::convert_to_display( int x, int y, int& out_x, int& out_y )
{
	out_x = x * _w() / 256;
	out_y = y * _h() / 256;
}
int CurveEditor2::_x()
{
	return x() + 7;
}
int CurveEditor2::_y()
{
	return y() + 7;
}
int CurveEditor2::_w()
{
	return w() - 14;
}
int CurveEditor2::_h()
{
	return h() - 14;
}
void CurveEditor2::set( int x1, int y1, int x2, int y2 )
{
	m_points[0].x = x1;
	m_points[0].y = y1;
	m_points[1].x = x2;
	m_points[1].y = y2;
}
void CurveEditor2::get( int &x1, int &y1, int &x2, int &y2 )
{
	x1 = m_points[0].x;
	y1 = m_points[0].y;
	x2 = m_points[1].x;
	y2 = m_points[1].y;
}

