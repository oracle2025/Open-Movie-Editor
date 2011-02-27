/* ColorCurveFactory.cxx
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

#include "ColorCurveFactory.H"
#include "ColorCurveFilter.H"
#include "VideoClip.H"
#include "TitleClip.H"


namespace nle
{
ColorCurveFactory* g_colorCurveFactory;

ColorCurveFactory::ColorCurveFactory()
{
	g_colorCurveFactory = this;
}
ColorCurveFactory::~ColorCurveFactory()
{
}
FilterBase* ColorCurveFactory::get( Clip* clip )
{
	VideoEffectClip* effectClip = dynamic_cast<VideoEffectClip*>(clip);
	if ( !effectClip ) {
		return 0;
	}
	return new ColorCurveFilter( effectClip->w(), effectClip->h() );
}
	
const char* ColorCurveFactory::name()
{
	return "Color Curve";
}

} /* namespace nle */
