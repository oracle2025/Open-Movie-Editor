/* ColorCurveFilter.cxx
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

#include <tinyxml.h>

#include "ColorCurveFilter.H"
#include "ColorCurveDialog.H"
#include "helper.H"
#include "LazyFrame.H"

namespace nle
{

ColorCurveFilter::ColorCurveFilter( int w, int h )
{
	m_w = w;
	m_h = h;
	m_lazy_frame = new LazyFrame( w, h );
	m_gavl_frame = gavl_video_frame_create( m_lazy_frame->format() );
	m_frame = m_gavl_frame->planes[0];
	m_lazy_frame->put_data( m_gavl_frame );

	for ( unsigned int i = 0; i < 256; i++ ) {
		m_values[i] = i;
		m_values_r[i] = i;
		m_values_g[i] = i;
		m_values_b[i] = i;
	}
	m_dialog = 0;

	m_parameters.r.p1.x = 0;
	m_parameters.r.p1.y = 255;
	m_parameters.r.p2.x = 255;
	m_parameters.r.p2.y = 0;

	m_parameters.g = m_parameters.b = m_parameters.m = m_parameters.r;
	m_expanded = true;
	m_bypass = false;
}

ColorCurveFilter::~ColorCurveFilter()
{
	if ( m_dialog ) {
		delete m_dialog;
	}
	if ( m_lazy_frame ) {
		delete m_lazy_frame;
	}
	if ( m_gavl_frame ) {
		gavl_video_frame_destroy( m_gavl_frame );
	}
}
LazyFrame* ColorCurveFilter::getFrame( LazyFrame* frame, int64_t )
{
	if ( m_bypass ) {
		return frame;
	}
	unsigned int len = m_w * m_h;
	unsigned char* dst = m_frame;
	unsigned char* src = frame->RGBA()->planes[0];
	while ( len-- ) {
		*dst++ = m_values_r[*src++];
		*dst++ = m_values_g[*src++];
		*dst++ = m_values_b[*src++];
		*dst++ = *src++; // copy alpha
	}
	return m_lazy_frame;
}
const char* ColorCurveFilter::name()
{
	return "Color Curve";
}

IEffectDialog* ColorCurveFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new ColorCurveDialog( this );
	}
	return m_dialog;
}
void ColorCurveFilter::calculate_values( unsigned char* values, struct color_curve_desc* desc )
{
	for ( int i = 0; i < desc->p1.x && i < 256; i++ ) {
		values[i] = 255 - desc->p1.y;
	}
	int A = desc->p2.x - desc->p1.x;
	int B = desc->p1.y - desc->p2.y;
	float C = (float)B / (float)A;
	for ( int i = desc->p1.x; i < desc->p2.x && i < 256; i++ ) {
		values[i] = 255 - desc->p1.y + (int)( (i-desc->p1.x) * C );
	}
	for ( int i = desc->p2.x; i < 256; i++ ) {
		values[i] = 255 - desc->p2.y;
	}
}
void ColorCurveFilter::calculate_values()
{
	unsigned char val_r[256];
	unsigned char val_g[256];
	unsigned char val_b[256];
	unsigned char val_m[256];
	
	calculate_values( val_r, &(m_parameters.r) );
	calculate_values( val_g, &(m_parameters.g) );
	calculate_values( val_b, &(m_parameters.b) );
	calculate_values( val_m, &(m_parameters.m) );
	
	unsigned char* red = val_r;
	unsigned char* green = val_g;
	unsigned char* blue = val_b;
	unsigned char* master = val_m;

	unsigned char* out_r = m_values_r;
	unsigned char* out_g = m_values_g;
	unsigned char* out_b = m_values_b;


	for ( int i = 0; i < 256; i++ ) {
		out_r[i] = master[red[i]];
		out_g[i] = master[green[i]];
		out_b[i] = master[blue[i]];
	}


}
void ColorCurveFilter::writeXML( TiXmlElement* xml_node )
{
	int bypass = m_bypass;
	xml_node->SetAttribute( "bypass", bypass );

	xml_node->SetAttribute( "r_p1_x", m_parameters.r.p1.x );
	xml_node->SetAttribute( "r_p1_y", m_parameters.r.p1.y );
	xml_node->SetAttribute( "r_p2_x", m_parameters.r.p2.x );
	xml_node->SetAttribute( "r_p2_y", m_parameters.r.p2.y );

	xml_node->SetAttribute( "g_p1_x", m_parameters.g.p1.x );
	xml_node->SetAttribute( "g_p1_y", m_parameters.g.p1.y );
	xml_node->SetAttribute( "g_p2_x", m_parameters.g.p2.x );
	xml_node->SetAttribute( "g_p2_y", m_parameters.g.p2.y );

	xml_node->SetAttribute( "b_p1_x", m_parameters.b.p1.x );
	xml_node->SetAttribute( "b_p1_y", m_parameters.b.p1.y );
	xml_node->SetAttribute( "b_p2_x", m_parameters.b.p2.x );
	xml_node->SetAttribute( "b_p2_y", m_parameters.b.p2.y );

	xml_node->SetAttribute( "m_p1_x", m_parameters.m.p1.x );
	xml_node->SetAttribute( "m_p1_y", m_parameters.m.p1.y );
	xml_node->SetAttribute( "m_p2_x", m_parameters.m.p2.x );
	xml_node->SetAttribute( "m_p2_y", m_parameters.m.p2.y );
	
}
void ColorCurveFilter::readXML( TiXmlElement* xml_node )
{
	int bypass = m_bypass;
	xml_node->Attribute( "bypass", &bypass );
	m_bypass = bypass;
	xml_node->Attribute( "r_p1_x", &m_parameters.r.p1.x );
	xml_node->Attribute( "r_p1_y", &m_parameters.r.p1.y );
	xml_node->Attribute( "r_p2_x", &m_parameters.r.p2.x );
	xml_node->Attribute( "r_p2_y", &m_parameters.r.p2.y );

	xml_node->Attribute( "g_p1_x", &m_parameters.g.p1.x );
	xml_node->Attribute( "g_p1_y", &m_parameters.g.p1.y );
	xml_node->Attribute( "g_p2_x", &m_parameters.g.p2.x );
	xml_node->Attribute( "g_p2_y", &m_parameters.g.p2.y );

	xml_node->Attribute( "b_p1_x", &m_parameters.b.p1.x );
	xml_node->Attribute( "b_p1_y", &m_parameters.b.p1.y );
	xml_node->Attribute( "b_p2_x", &m_parameters.b.p2.x );
	xml_node->Attribute( "b_p2_y", &m_parameters.b.p2.y );

	xml_node->Attribute( "m_p1_x", &m_parameters.m.p1.x );
	xml_node->Attribute( "m_p1_y", &m_parameters.m.p1.y );
	xml_node->Attribute( "m_p2_x", &m_parameters.m.p2.x );
	xml_node->Attribute( "m_p2_y", &m_parameters.m.p2.y );
	calculate_values();
}

} /* namespace nle */
