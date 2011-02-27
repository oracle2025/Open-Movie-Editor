/* ColorCurveFilter.H
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

#ifndef _COLOR_CURVE_FILTER_H_
#define _COLOR_CURVE_FILTER_H_

#include "IVideoEffect.H"
#include <gavl/gavl.h>

namespace nle
{

struct color_curve_p {
	int x;
	int y;
};
struct color_curve_desc {
	struct color_curve_p p1;
	struct color_curve_p p2;
};
struct color_curve_data {
	struct color_curve_desc r;
	struct color_curve_desc g;
	struct color_curve_desc b;
	struct color_curve_desc m;
};

class ColorCurveFilter : public IVideoEffect
{
	public:
		ColorCurveFilter( int w, int h );
		~ColorCurveFilter();
		LazyFrame* getFrame( LazyFrame* frame, int64_t position = -1 );
		const char* name();
		int numParams() { return 1; }
		bool hasDialog() { return true; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;
		unsigned char m_values[256];
		unsigned char m_values_r[256];
		unsigned char m_values_g[256];
		unsigned char m_values_b[256];
		const char* identifier() { return "effect:builtin:ColorCurves"; }
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
		struct color_curve_data m_parameters;
		void calculate_values();
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
	private:
		void calculate_values( unsigned char* value, struct color_curve_desc* desc );
		unsigned char *m_frame;
		gavl_video_frame_t* m_gavl_frame;
		LazyFrame* m_lazy_frame;
		int m_w;
		int m_h;
};

	
} /* namespace nle */

#endif /* _COLOR_CURVE_FILTER_H_ */
