/*  IAudioFilter.H
 *
 *  Copyright (C) 2007-2008 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _I_AUDIO_FILTER_H_
#define _I_AUDIO_FILTER_H_

#include <stdint.h>
#include "FilterBase.H"
#include "Rect.H"

class TiXmlElement;

namespace nle
{

class DragHandler;

class IAudioFilter : public FilterBase
{
	public:
		virtual ~IAudioFilter() {}
		//virtual void onDraw( Rect& rect ) = 0;
		virtual DragHandler* onMouseDown( Rect& rect, int x, int y, bool shift ) = 0;
		virtual int fillBuffer( float* input_output, unsigned long frames, int64_t position ) = 0;
		virtual void writeXML( TiXmlElement* xml_node ) = 0;
		virtual void readXML( TiXmlElement* xml_node ) = 0;
		//void setParams();
		//void getParams();
		//trimA
		//trimB
	protected:
		IAudioFilter() {}
};

} /* namespace nle */


#endif /* _I_AUDIO_FILTER_H_ */

