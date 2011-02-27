/* LiftGammaGainFilter.H
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

#ifndef _LIFT_GAMMA_GAIN_FILTER_H_
#define _LIFT_GAMMA_GAIN_FILTER_H_

#include "IVideoEffect.H"
#include <gavl/gavl.h>

namespace nle
{

class LiftGammaGainFilter : public IVideoEffect
{
	public:
		LiftGammaGainFilter( int w, int h );
		~LiftGammaGainFilter();
		LazyFrame* getFrame( LazyFrame* frame, int64_t position = -1 );
		const char* name();
		int numParams() { return 1; }
		bool hasDialog() { return true; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;

		unsigned char m_red[256];
		unsigned char m_green[256];
		unsigned char m_blue[256];

		const char* identifier() { return "effect:builtin:LiftGammaGain"; }

		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}

		void calculate_values();

		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );


		void lift( float r, float g, float b, float v );
		void gamma( float r, float g, float b, float v );
		void gain( float r, float g, float b, float v );
		const float* lift() { return m_lift; }
		const float* gamma() { return m_gamma; }
		const float* gain() { return m_gain; }

	private:
		int m_w;
		int m_h;
		unsigned char *m_frame;
		gavl_video_frame_t* m_gavl_frame;
		LazyFrame* m_lazy_frame;
		float m_lift[4];
		float m_gamma[4];
		float m_gain[4];
};

	
} /* namespace nle */

#endif /* _LIFT_GAMMA_GAIN_FILTER_H_ */
