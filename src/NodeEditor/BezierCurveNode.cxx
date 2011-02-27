/*  BezierCurveNode.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */



#include "BezierCurveNode.H"
#include "CurveEditorBezier.H"
#include <FL/Fl_Group.H>
#include <cassert>
#include "sl/sl.h"
#include <tinyxml.h>
#include <math.h>
#include <iostream>

BezierCurveNode::BezierCurveNode( int w, int h )
{
	m_frame = new uint32_t[w*h];
	m_length = w * h;
	m_points = 0;
}
BezierCurveNode::~BezierCurveNode()
{
	delete_widgets();
	delete [] m_frame;
	point_list* node;
	while ( ( node = (point_list*)sl_pop( &m_points ) ) ) {
		delete node;
	}

}
uint32_t* BezierCurveNode::getFrame( int output, double position )
{
	uint32_t* input_frame;
	if ( node->inputs[0] == 0 || !node->inputs[0]->node ) {
		return 0;
	}
	input_frame = node->inputs[0]->node->getFrame( 0, position );
	if ( !input_frame ) {
		return 0;
	}
	int len = m_length;
	assert( len > 0 );
	unsigned char* dst = (unsigned char*)m_frame;
	unsigned char* src = (unsigned char*)input_frame;
	while (len--) {
		*dst++ = m_values[*src++];
		*dst++ = m_values[*src++];
		*dst++ = m_values[*src++];
		*dst++ = *src++; // copy alpha
	}
	return m_frame;
}
void BezierCurveNode::init_widgets()
{
	CurveEditorBezier* curve = new CurveEditorBezier( 0, 0, 100, 100 );
	node->widgets[0] = curve;
	node->widgets[1] = 0;
	if ( m_points ) {
		point_list* node;
		while ( ( node = (point_list*)sl_pop( &curve->m_points ) ) ) {
			delete node;
		}
		curve->m_points = m_points;
		m_points = 0;
	}
	curve->setValues( m_values );
}

void BezierCurveNode::delete_widgets()
{
	CurveEditorBezier* curve = dynamic_cast<CurveEditorBezier*>( node->widgets[0] );
	if ( curve ) {
		m_points = curve->m_points;
		curve->m_points = 0;
	}
	for ( int i = 0; node->widgets[i]; i++ ) {
		node->widgets[i]->parent()->remove( node->widgets[i] );
		delete node->widgets[i];
		node->widgets[i] = 0;
	}
}
void BezierCurveNode::readXML( TiXmlElement* xml_node )
{
	TiXmlElement* point_xml = TiXmlHandle( xml_node ).FirstChildElement( "point" ).Element();
	for ( ; point_xml; point_xml = point_xml->NextSiblingElement( "point" ) ) {
		point_list* n = new point_list;
		double dval = 0.0;
		point_xml->Attribute( "x", &dval );
		n->x = dval;
		point_xml->Attribute( "y", &dval );
		n->y = dval;
		point_xml->Attribute( "pre_x", &dval );
		n->pre_x = dval;
		point_xml->Attribute( "pre_y", &dval );
		n->pre_y = dval;
		point_xml->Attribute( "post_x", &dval );
		n->post_x = dval;
		point_xml->Attribute( "post_y", &dval );
		n->post_y = dval;
		n->next = 0;
		m_points = (point_list*)sl_unshift( m_points, n );
	}
	calc_lut();
}

void BezierCurveNode::writeXML( TiXmlElement* xml_node )
{
	CurveEditorBezier* curve = dynamic_cast<CurveEditorBezier*>( node->widgets[0] );
	if ( curve ) {
		m_points = curve->m_points;
	}
	TiXmlElement* point_xml;
	for ( point_list* n = m_points; n; n = n->next ) {
		point_xml = new TiXmlElement( "point" );
		xml_node->LinkEndChild( point_xml );
		point_xml->SetDoubleAttribute( "x", (double)n->x );
		point_xml->SetDoubleAttribute( "y", (double)n->y );
		point_xml->SetDoubleAttribute( "pre_x", (double)n->pre_x );
		point_xml->SetDoubleAttribute( "pre_y", (double)n->pre_y );
		point_xml->SetDoubleAttribute( "post_x", (double)n->post_x );
		point_xml->SetDoubleAttribute( "post_y", (double)n->post_y );
	}
}




/****COPY+PASTE FROM Curve Editor Bezier*****/
extern float find_value_array( point_list* p, int len, float index );
extern unsigned char scale_to_8bit( float value );
extern float find_value( point_list* p, float index );
void BezierCurveNode::calc_lut()
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
void BezierCurveNode::calc_curve_lut()
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
	for ( point_list* p = m_points; p->next; p = p->next ) {
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
	for ( point_list* p = m_points; p->next; p = p->next ) {
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
