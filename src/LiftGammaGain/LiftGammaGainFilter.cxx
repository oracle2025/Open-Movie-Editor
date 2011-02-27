/* LiftGammaGainFilter.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include "LiftGammaGainFilter.H"
#include "LiftGammaGainDialog.H"
#include "helper.H"
#include "LazyFrame.H"

#include <tinyxml.h>
#include <cmath>

namespace nle
{

LiftGammaGainFilter::LiftGammaGainFilter( int w, int h )
{
	m_w = w;
	m_h = h;
	m_lazy_frame = new LazyFrame( w, h );
	m_gavl_frame = gavl_video_frame_create( m_lazy_frame->format() );
	m_lazy_frame->put_data( m_gavl_frame );
	m_frame = m_gavl_frame->planes[0];
	for ( unsigned int i = 0; i < 256; i++ ) {
		m_red[i] = i;
		m_green[i] = i;
		m_blue[i] = i;
	}
	m_lift[0]  = m_lift[1]  = m_lift[2]  = 1.0;
	m_gamma[0] = m_gamma[1] = m_gamma[2] = 1.0;
	m_gain[0]  = m_gain[1]  = m_gain[2]  = 1.0;
	m_lift[3]  = m_gamma[3] = m_gain[3]  = 0.5;
	m_dialog = 0;
	m_bypass = false;
	m_expanded = true;
}
LiftGammaGainFilter::~LiftGammaGainFilter()
{
	delete m_lazy_frame;
	gavl_video_frame_destroy( m_gavl_frame );
	if ( m_dialog ) {
		delete m_dialog;
	}
}
LazyFrame* LiftGammaGainFilter::getFrame( LazyFrame* frame, int64_t position )
{
	if ( m_bypass ) {
		return frame;
	}
	unsigned int len = m_w * m_h;
	unsigned char* dst = m_frame;
	unsigned char* src = frame->RGBA()->planes[0];
	while (len--)
	{
		*dst++ = m_red[*src++];
		*dst++ = m_green[*src++];
		*dst++ = m_blue[*src++];
		*dst++ = *src++; // copy alpha
	}
	return m_lazy_frame;
}
const char* LiftGammaGainFilter::name()
{
	return "Lift Gamma Gain";
}
IEffectDialog* LiftGammaGainFilter::dialog()
{
	if ( !m_dialog ) {
		m_dialog = new LiftGammaGainDialog( this );
	}
	return m_dialog;

}
void LiftGammaGainFilter::writeXML( TiXmlElement* xml_node )
{
	int bypass = m_bypass;
	xml_node->SetAttribute( "bypass", bypass );
	xml_node->SetDoubleAttribute( "lift_r", m_lift[0] );
	xml_node->SetDoubleAttribute( "lift_g", m_lift[1] );
	xml_node->SetDoubleAttribute( "lift_b", m_lift[2] );
	xml_node->SetDoubleAttribute( "lift_v", m_lift[3] );
	xml_node->SetDoubleAttribute( "gamma_r", m_gamma[0] );
	xml_node->SetDoubleAttribute( "gamma_g", m_gamma[1] );
	xml_node->SetDoubleAttribute( "gamma_b", m_gamma[2] );
	xml_node->SetDoubleAttribute( "gamma_v", m_gamma[3] );
	xml_node->SetDoubleAttribute( "gain_r", m_gain[0] );
	xml_node->SetDoubleAttribute( "gain_g", m_gain[1] );
	xml_node->SetDoubleAttribute( "gain_b", m_gain[2] );
	xml_node->SetDoubleAttribute( "gain_v", m_gain[3] );
}
void LiftGammaGainFilter::readXML( TiXmlElement* xml_node )
{
	int bypass = m_bypass;
	xml_node->Attribute( "bypass", &bypass );
	m_bypass = bypass;
	double r, g, b, v;
	xml_node->Attribute( "lift_r", &r );
	xml_node->Attribute( "lift_g", &g );
	xml_node->Attribute( "lift_b", &b );
	xml_node->Attribute( "lift_v", &v );
	m_lift[0] = r;
	m_lift[1] = g;
	m_lift[2] = b;
	m_lift[3] = v;
	xml_node->Attribute( "gamma_r", &r );
	xml_node->Attribute( "gamma_g", &g );
	xml_node->Attribute( "gamma_b", &b );
	xml_node->Attribute( "gamma_v", &v );
	m_gamma[0] = r;
	m_gamma[1] = g;
	m_gamma[2] = b;
	m_gamma[3] = v;
	xml_node->Attribute( "gain_r", &r );
	xml_node->Attribute( "gain_g", &g );
	xml_node->Attribute( "gain_b", &b );
	xml_node->Attribute( "gain_v", &v );
	m_gain[0] = r;
	m_gain[1] = g;
	m_gain[2] = b;
	m_gain[3] = v;
	calculate_values();
}

void LiftGammaGainFilter::lift( float r, float g, float b, float v )
{
	m_lift[0] = r;
	m_lift[1] = g;
	m_lift[2] = b;
	m_lift[3] = v;
}
void LiftGammaGainFilter::gamma( float r, float g, float b, float v )
{
	m_gamma[0] = r;
	m_gamma[1] = g;
	m_gamma[2] = b;
	m_gamma[3] = v;
}
void LiftGammaGainFilter::gain( float r, float g, float b, float v )
{
	m_gain[0] = r;
	m_gain[1] = g;
	m_gain[2] = b;
	m_gain[3] = v;
}


static float red( float* a ) {
	return a[0];
}
static float green( float* a ) {
	return a[1];
}
static float blue( float* a ) {
	return a[2];
}
static float value( float* a ) {
	return a[3];
}
static int f_to_i( float in ) {
	return (int)( in * 255 );
}
static float i_to_f( int in ) {
	return (float)in / 255.0;
}
static int clamp_255( int in ) {
	int out;
	if ( in < 0 ) {
		return 0;
	}
	if ( in > 255 ) {
		return 255;
	}
	return in;
}

void LiftGammaGainFilter::calculate_values()
{
	// r goes from 0.0 to 1.0
	float gain, lift, gamma;
	float vgain, vlift, vgamma;
	vgain = value( m_gain ) * 2.0;
	vlift = value( m_lift ) * 2.0 - 1.0;
	vgamma = value( m_gamma ) * 2.0;
	bool m_constant_green = false;
	if ( m_constant_green ) {
		for ( unsigned int i = 0; i < 256; i++ ) {
			/* Red */
			/* Cyan => Rv */
			/* Magenta => R^ B^ */
			gamma = 1.0 / ( 1.0 - ( green( m_gamma ) - red( m_gamma ) ) );
			gain = 1.0 - (green( m_gain ) - red( m_gain ));
			lift = red( m_lift ) - green( m_lift );
			m_red[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );

			/* Blue */
			/* Magenta => R^ B^ */
			/* Yellow => Bv */
			gamma = 1.0 / ( 1.0 - ( green( m_gamma ) - blue( m_gamma ) ) );
			gain = 1.0 - (green( m_gain ) - blue( m_gain ));
			lift = blue( m_lift ) - green( m_lift );
			m_blue[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );
		}
	} else {
		for ( unsigned int i = 0; i < 256; i++ ) {
			/* Red */
			/* Cyan => Rv */
			/* Magenta => R^ B^ */
			gamma = ( 1.0 / red( m_gamma ) ) * vgamma;
			gain = ( red( m_gain ) ) * vgain;
			lift = ( red( m_lift ) - 1.0 ) + vlift;
			m_red[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );

			/* Blue */
			/* Magenta => R^ B^ */
			/* Yellow => Bv */
			gamma = ( 1.0 / blue( m_gamma ) ) * vgamma;
			gain = ( blue( m_gain ) ) * vgain;
			lift = ( blue( m_lift ) - 1.0 ) + vlift;
			m_blue[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );

			gamma = ( 1.0 / green( m_gamma ) ) * vgamma;
			gain = ( green( m_gain ) ) * vgain;
			lift = ( green( m_lift ) - 1.0 ) + vlift;
			m_green[i] = clamp_255(f_to_i(  pow( (( i_to_f(i) * gain ) + lift ), gamma ) ) );
		}
	}
}

} /* namespace nle */
