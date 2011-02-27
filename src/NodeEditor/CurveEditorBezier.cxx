
#include "CurveEditorBezier.H"
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include "sl/sl.h"
#include <iostream>
#include <math.h>
#include "VideoViewGL.H"
using namespace std;

unsigned char scale_to_8bit( float value );

CurveEditorBezier::CurveEditorBezier( int x, int y, int w, int h, const char* label )
	: Fl_Widget( x, y, w, h, label )	
{
	m_points = 0;
	point_list* n = new point_list;
	n->x = 1.0;
	n->y = 1.0;
	n->pre_x = n->pre_y = 0.0;
	n->post_x = n->post_y = 0.0;
	n->next = 0;
	m_points = new point_list;
	m_points->x = 0.0;
	m_points->y = 0.0;
	m_points->pre_x = m_points->pre_y = 0.0;
	m_points->post_x = m_points->post_y = 0.0;
	m_points->next = n;
	m_grabbed_point = 0;
	m_values = 0;
	m_grabbed_pre = 0;
	m_grabbed_post = 0;
	m_deleted_point = false;
}
CurveEditorBezier::~CurveEditorBezier()
{
	point_list* node;
	while ( ( node = (point_list*)sl_pop( &m_points ) ) ) {
		delete node;
	}
}
void CurveEditorBezier::draw()
{
	fl_draw_box( FL_DOWN_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );

	fl_line_style( FL_SOLID, 1 );
	fl_color( FL_BLACK );
	fl_line( x(), y() + h() / 2, x() + w(), y() + h() / 2 );
	fl_line( x(), y() + h() / 4, x() + w(), y() + h() / 4 );
	fl_line( x(), y() + h() * 3 / 4, x() + w(), y() + h() * 3 / 4 );
	
	fl_line( x() + w() / 2, y(), x() + w() / 2, y() + h() );
	fl_line( x() + w() / 4, y(), x() + w() / 4, y() + h() );
	fl_line( x() + w() * 3 / 4 , y(), x() + w() * 3 / 4, y() + h() );

	fl_push_matrix();
	fl_translate( x() + 7, y() + 7 ); // 2px border 5 px point radius
	fl_scale( w() - 14, h() - 14 );

/*	fl_color( FL_RED );
	fl_line_style( FL_SOLID, 2 );
	fl_begin_line();
	for ( point_list* n = m_points; n; n = n->next ) {
		fl_vertex( n->x, 1.0 - n->y );
	}
	fl_end_line();*/
	
	fl_color( FL_BLACK );
	fl_line_style( FL_SOLID, 1 );
	fl_begin_line();
	point_list* n_minus_1 = m_points;
	if ( !n_minus_1 ) {
		return;
	}
	for ( point_list* n = m_points->next; n; n = n->next ) {
		fl_curve( n_minus_1->x, 1.0 - n_minus_1->y, n_minus_1->x + n_minus_1->post_x, 1.0 - n_minus_1->y - n_minus_1->post_y, n->x + n->pre_x, 1.0 - n->y - n->pre_y, n->x, 1.0 - n->y );
		n_minus_1 = n_minus_1->next;
	}
	fl_end_line();

	
	fl_pop_matrix();

	int x_display;
	int y_display;
	int x_display2;
	int y_display2;
	int x_display3;
	int y_display3;
	for ( point_list* n = m_points; n; n = n->next ) {
		point_to_display( n->x, n->y, x_display, y_display );
		
		point_to_display( n->x + n->post_x, n->y + n->post_y, x_display2, y_display2 );
		point_to_display( n->x + n->pre_x, n->y + n->pre_y, x_display3, y_display3 );
		fl_color( FL_BLACK );
		fl_line_style( FL_SOLID, 1 );
		fl_begin_line();
		fl_vertex(x_display3, y_display3);
		fl_vertex(x_display,y_display);
		fl_vertex(x_display2, y_display2);
		fl_end_line();
		fl_draw_box( FL_UP_BOX, x_display - 5, y_display - 5, 10, 10, FL_RED );
		fl_draw_box( FL_UP_BOX, x_display2 - 5, y_display2 - 5, 10, 10, FL_GREEN );
		fl_draw_box( FL_UP_BOX, x_display3 - 5, y_display3 - 5, 10, 10, FL_GREEN );
	}

	
	//Debugging: Show LUT Points
/*	fl_color( FL_GREEN );
	fl_line_style( FL_SOLID, 1 );
	for ( int i = 0; i < 256; i++ ) {
		point_to_display( (float)(i) / 255.0, (float)m_values[i] / 255.0, x_display, y_display );
		fl_draw_box( FL_BORDER_FRAME, x_display - 3, y_display - 3, 6, 6, FL_BLACK );
	}*/
	

	fl_pop_clip();
}
int CurveEditorBezier::handle( int event )
{
	switch ( event ) {
		case FL_FOCUS:
		case FL_UNFOCUS:
			return 1;
		case FL_SHORTCUT:
			if ( Fl::event_key(FL_Delete) && m_grabbed_point ) {
				for ( point_list* n = m_points; n; n = n->next ) {
					if ( n->next == m_grabbed_point && m_grabbed_point->next ) {
						n->next = m_grabbed_point->next;
						delete m_grabbed_point;
						m_grabbed_point = 0;
						calc_lut();
						redraw();
						m_deleted_point = true;
						nle::g_videoView->redraw();
						return 1;
					}
				}
				return 1;
			}
		case FL_PUSH:
			{
				m_drag_lim_left = 0.0;
				for ( point_list* n = m_points; n; n = n->next ) {
					int x_scr;
					int y_scr;
					int x = Fl::event_x();
					int y = Fl::event_y();
					point_to_display( n->x + n->post_x, n->y + n->post_y, x_scr, y_scr );
					if ( x_scr - 5 < x && x_scr + 5 > x && y_scr - 5 < y && y_scr + 5 > y ) {
							if ( n->next ) {
							m_drag_lim_right = n->next->x;
						} else {
							m_drag_lim_right = 1.0;
						}

						m_grabbed_post = n;
						return 1;
					}
					point_to_display( n->x + n->pre_x, n->y + n->pre_y, x_scr, y_scr );
					if ( x_scr - 5 < x && x_scr + 5 > x && y_scr - 5 < y && y_scr + 5 > y ) {
							if ( n->next ) {
							m_drag_lim_right = n->next->x;
						} else {
							m_drag_lim_right = 1.0;
						}

						m_grabbed_pre = n;
						return 1;
					}
					m_drag_lim_left = n->x;
				}
				for ( point_list* n = m_points; n; n = n->next ) {
					int x_scr;
					int y_scr;
					int x = Fl::event_x();
					int y = Fl::event_y();
					point_to_display( n->x, n->y, x_scr, y_scr );
					if ( x_scr - 5 < x && x_scr + 5 > x && y_scr - 5 < y && y_scr + 5 > y ) {
						m_grabbed_point = n;
						if ( n->next ) {
							m_drag_lim_right = n->next->x;
						} else {
							m_drag_lim_right = 1.0;
						}
						break;
					}
					m_drag_lim_left = n->x;
				}
				return 1;
			}
			break; // never reached!
		case FL_RELEASE:
			{
				if ( m_grabbed_point ) {
					point_list* n = m_grabbed_point;
					m_grabbed_point = 0;
					display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
					if ( n->x > m_drag_lim_right ) {
						n->x = m_drag_lim_right;
					}
					if ( n->x < m_drag_lim_left ) {
						n->x = m_drag_lim_left;
					}
					if ( n->y < 0.0 ) { n->y = 0.0; }
					if ( n->y > 1.0 ) { n->y = 1.0; }
					calc_lut();
					redraw();
					nle::g_videoView->redraw();
					return 1;
				} else if ( m_grabbed_post ) {
					point_list* n = m_grabbed_post;
					m_grabbed_post = 0;
					if ( Fl::event_ctrl() ) {
						display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
						n->x -= n->post_x;
						n->y -= n->post_y;
						if ( n->x > m_drag_lim_right ) {
							n->x = m_drag_lim_right;
						}
						if ( n->x < m_drag_lim_left ) {
							n->x = m_drag_lim_left;
						}
						if ( n->y < 0.0 ) { n->y = 0.0; }
						if ( n->y > 1.0 ) { n->y = 1.0; }
						calc_lut();
						redraw();
						nle::g_videoView->redraw();
						return 1;
					}
					display_to_point( Fl::event_x(), Fl::event_y(), n->post_x, n->post_y );
					n->post_x -= n->x;
					n->post_y -= n->y;
					if ( !Fl::event_shift() ) {
						n->pre_x = - n->post_x;
						n->pre_y = - n->post_y;
					}
					calc_lut();
					redraw();
					nle::g_videoView->redraw();
					return 1;
				} else if ( m_grabbed_pre ) {
					point_list* n = m_grabbed_pre;
					m_grabbed_pre = 0;
					if ( Fl::event_ctrl() ) {
						display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
						n->x -= n->pre_x;
						n->y -= n->pre_y;
						if ( n->x > m_drag_lim_right ) {
							n->x = m_drag_lim_right;
						}
						if ( n->x < m_drag_lim_left ) {
							n->x = m_drag_lim_left;
						}
						if ( n->y < 0.0 ) { n->y = 0.0; }
						if ( n->y > 1.0 ) { n->y = 1.0; }
						calc_lut();
						redraw();
						nle::g_videoView->redraw();
						return 1;
					}
					display_to_point( Fl::event_x(), Fl::event_y(), n->pre_x, n->pre_y );
					n->pre_x -= n->x;
					n->pre_y -= n->y;
					if ( !Fl::event_shift() ) {
						n->post_x = - n->pre_x;
						n->post_y = - n->pre_y;
					}
					calc_lut();
					redraw();
					nle::g_videoView->redraw();
					return 1;
				}
				if ( m_deleted_point ) {
					m_deleted_point = false;
					return 1;
				}
				point_list* n = new point_list;
				n->next = 0;
				n->pre_x = n->pre_y = 0.0;
				n->post_x = n->post_y = 0.0;
				display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
				if ( n->x < 0.0 ) { n->x = 0.0; }
				if ( n->x > 1.0 ) { n->x = 1.0; }
				if ( n->y < 0.0 ) { n->y = 0.0; }
				if ( n->y > 1.0 ) { n->y = 1.0; }

				point_list* p = m_points;
				if ( p->x > n->x ) {
					n->next = p;
					m_points = n;
					calc_lut();
					redraw();
					nle::g_videoView->redraw();
					return 1;
				}
				for ( p = m_points; p ; p = p->next ) {
				      if ( p->x < n->x && p->next && p->next->x > n->x ) {
					      break;
				      }
				}
				if ( !p ) {
					return 1;
				}
				n->next = p->next;
				p->next = 0;
				m_points = (point_list*)sl_unshift( m_points, n );
				
				calc_lut();
				redraw();
				nle::g_videoView->redraw();
				return 1;
			}
			break; // never reached!
		case FL_DRAG:
			{
				if ( m_grabbed_point ) {
					point_list* n = m_grabbed_point;
					display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
					if ( n->x > m_drag_lim_right ) {
						n->x = m_drag_lim_right;
					}
					if ( n->x < m_drag_lim_left ) {
						n->x = m_drag_lim_left;
					}
					if ( n->y < 0.0 ) { n->y = 0.0; }
					if ( n->y > 1.0 ) { n->y = 1.0; }
					redraw();
					calc_lut();
					nle::g_videoView->redraw();
					return 1;
				} else if ( m_grabbed_post ) {
					point_list* n = m_grabbed_post;
					if ( Fl::event_ctrl() ) {
						display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
						n->x -= n->post_x;
						n->y -= n->post_y;
						if ( n->x > m_drag_lim_right ) {
							n->x = m_drag_lim_right;
						}
						if ( n->x < m_drag_lim_left ) {
							n->x = m_drag_lim_left;
						}
						if ( n->y < 0.0 ) { n->y = 0.0; }
						if ( n->y > 1.0 ) { n->y = 1.0; }
						redraw();
						calc_lut();
						nle::g_videoView->redraw();
						return 1;
					}
					display_to_point( Fl::event_x(), Fl::event_y(), n->post_x, n->post_y );
					n->post_x -= n->x;
					n->post_y -= n->y;
					if ( !Fl::event_shift() ) {
						n->pre_x = - n->post_x;
						n->pre_y = - n->post_y;
					}
					redraw();
					calc_lut();
					nle::g_videoView->redraw();
					return 1;
				} else if ( m_grabbed_pre ) {
					point_list* n = m_grabbed_pre;
					if ( Fl::event_ctrl() ) {
						display_to_point( Fl::event_x(), Fl::event_y(), n->x, n->y );
						n->x -= n->pre_x;
						n->y -= n->pre_y;
						if ( n->x > m_drag_lim_right ) {
							n->x = m_drag_lim_right;
						}
						if ( n->x < m_drag_lim_left ) {
							n->x = m_drag_lim_left;
						}
						if ( n->y < 0.0 ) { n->y = 0.0; }
						if ( n->y > 1.0 ) { n->y = 1.0; }
						redraw();
						calc_lut();
						nle::g_videoView->redraw();
						return 1;
					}
					display_to_point( Fl::event_x(), Fl::event_y(), n->pre_x, n->pre_y );
					n->pre_x -= n->x;
					n->pre_y -= n->y;
					if ( !Fl::event_shift() ) {
						n->post_x = - n->pre_x;
						n->post_y = - n->pre_y;
					}
					redraw();
					calc_lut();
					nle::g_videoView->redraw();
					return 1;
				}

			}
	}
	return Fl_Widget::handle( event );
}
void CurveEditorBezier::setValues( unsigned char* val )
{
	m_values = val;
	calc_lut();
}
void CurveEditorBezier::display_to_point( int xv, int yv, float& out_x, float& out_y )
{
	out_x = (float)(xv - x() - 7) / ( w() - 14 );
	out_y = 1.0 - (float)(yv - y() - 7) / ( h() - 14 );
}
void CurveEditorBezier::point_to_display( float xv, float yv, int& out_x, int& out_y )
{
	out_x = (int)( xv * ( w() - 14 ) ) + x() + 7;
	out_y = (int)( (1.0 - yv) * ( h() - 14 ) ) + y() + 7;
}

float find_value( point_list* p, float index )
{
	if ( index <= p->x ) {
		return p->y;
	}
	point_list* n2;
	for ( point_list* n = p; n; n = n->next ) {
		if ( n->x < index && n->next && n->next->x > index ) {
			n2 = n->next;
			;
			float A = n2->x - n->x;
			float B = n->y - n2->y;
			float C = B / A;
			return n->y - (index - n->x) * C;
		}
		if ( !n->next ) {
			return n->y;
		}
	}
	return 0.0;
}
void CurveEditorBezier::calc_lut()
{
	calc_curve_lut();
	return;
	if ( !m_values ) {
		return;
	}
	for ( int i = 0; i < 256; i++ ) {
		m_values[i] = scale_to_8bit( find_value( m_points, ((float)i) /	255.0 ) );
	}
}
unsigned char scale_to_8bit( float value ) {
	if ( value < 0.0 ) {
		value = 0.0;
	} else if ( value > 1.0 ) {
		value = 1.0;
	}
	int guard = (int)( value * 255 );
	if ( guard < 0 ) {
		guard = 0;
	} else if ( guard > 255 ) {
		guard = 255;
	}
	return (unsigned char)( guard );
}

float find_value_array( point_list* p, int len, float index ) {
	if ( index <= p->x ) {
		return p->y;
	}
	point_list* n = &p[0];
	point_list* n2 = &p[1];
	int i;
	for ( i = 0; i < len - 1; i++ ) {
		n = &p[i];
		n2 = &p[i+1];
		if ( n->x < index && n2->x > index ) {
			float A = n2->x - n->x;
			float B = n->y - n2->y;
			float C = B / A;
			return n->y - (index - n->x) * C;
		}
	}
	return n2->y;

}
void CurveEditorBezier::calc_curve_lut()
{
	int n = 0;
	int n2 = 0;
	double x;
	double y;
	double x1;
	double yy1;
	double x2;
	double y2;
	double x3;
	double y3;
	double a;
	double b;
	int c;

	point_list* p2;
	for ( point_list* p = m_points; p && p->next; p = p->next ) {
		p2 = p->next;

		x = p->x;
		y = p->y;
		x1 = p->x+p->post_x;
		yy1 = p->y+p->post_y;
		x2 = p2->x+p2->pre_x;
		y2 = p2->y+p2->pre_y;
		x3 = p2->x;
		y3 = p2->y;

		a = fabs((x-x2)*(y3-yy1)-(y-y2)*(x3-x1));
		b = fabs((x-x3)*(y2-yy1)-(y-y3)*(x2-x1));
		if (b > a) a = b;

		c  = int(a*200);
		if ( c > 1 ) {
			if ( c > 100 ) { c = 100; }
			n += c;
		} else {
			n+= 2;
		}
		n++;
	}
	n += 2;
	point_list* all_points = new point_list[n+1];

	int i = 0;
	for ( point_list* p = m_points; p && p->next; p = p->next ) {
		p2 = p->next;
		all_points[i].x = p->x;
		all_points[i].y = p->y;
		i++;
		if ( i >= n ) {
			return;
		}

		x = p->x;
		y = p->y;
		x1 = p->x+p->post_x;
		yy1 = p->y+p->post_y;
		x2 = p2->x+p2->pre_x;
		y2 = p2->y+p2->pre_y;
		x3 = p2->x;
		y3 = p2->y;

		a = fabs((x-x2)*(y3-yy1)-(y-y2)*(x3-x1));
		b = fabs((x-x3)*(y2-yy1)-(y-y3)*(x2-x1));
		if (b > a) a = b;

		n2  = int(a*200);
		if ( n2 > 1 ) {
			if ( n2 > 100 ) { n2 = 100; }
			double e = 1.0/n2;

			// calculate the coefficients of 3rd order equation:
			double xa = (x3-3*x2+3*x1-x);
			double xb = 3*(x2-2*x1+x);
			double xc = 3*(x1-x);
			// calculate the forward differences:
			double dx1 = ((xa*e+xb)*e+xc)*e;
			double dx3 = 6*xa*e*e*e;
			double dx2 = dx3 + 2*xb*e*e;

			// calculate the coefficients of 3rd order equation:
			double ya = (y3-3*y2+3*yy1-y);
			double yb = 3*(y2-2*yy1+y);
			double yc = 3*(yy1-y);
			// calculate the forward differences:
			double dy1 = ((ya*e+yb)*e+yc)*e;
			double dy3 = 6*ya*e*e*e;
			double dy2 = dy3 + 2*yb*e*e;

			// draw points 1 .. n-2:
			for (int m=2; m<n2; m++) {
				x += dx1;
				dx1 += dx2;
				dx2 += dx3;
				y += dy1;
				dy1 += dy2;
				dy2 += dy3;
				//  fl_transformed_vertex(x,y);
				all_points[i].x = x;
				all_points[i].y = y;
				i++;
				if ( i >= n ) {
					return;
				}
			}

			// draw point n-1:
			all_points[i].x = x+dx1;
			all_points[i].y = y+dy1;
			i++;
			if ( i >= n ) {
				return;
			}
				//fl_transformed_vertex(x+dx1, y+dy1);
		}

		// draw point n:
		//fl_transformed_vertex(x3,y3);
		all_points[i].x = x3;
		all_points[i].y = y3;
		i++;
		if ( i >= n ) {
			return;
		}
	}
	int k = i;
	for ( int i = 0; i < 256; i++ ) {
		m_values[i] = scale_to_8bit( find_value_array( all_points, k,((float)i) / 255.0 ) );
	}
}
