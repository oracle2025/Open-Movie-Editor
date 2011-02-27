/* FilterBase.H
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

#ifndef _FILTER_BASE_
#define _FILTER_BASE_

#include <stdint.h>
#include "Rect.H"

class TiXmlElement;

namespace nle
{

class DragHandler;
class IEffectDialog;
class IEffectWidget;

class FilterData
{
	public:
		virtual ~FilterData() {}
};

class FilterBase
{
	public:
		virtual ~FilterBase() {}
		virtual void reset() {}
		virtual int64_t trimA( int64_t trim ) { return trim; }
		virtual int64_t trimB( int64_t trim ) { return trim; }
		virtual void onDraw( Rect& ) {}
		virtual DragHandler* onMouseDown() { return 0; }
		virtual const char* name() = 0;
		virtual void writeXML( TiXmlElement* ) {}
		virtual void readXML( TiXmlElement* ) {}
		virtual bool hasDialog() { return false; }
		virtual IEffectDialog* dialog() { return 0; }
		virtual IEffectWidget* widget();
		virtual const char* identifier() = 0;
		virtual FilterData* getFilterData() = 0;
		virtual void setFilterData( FilterData* ) = 0;
		void bypass( bool b ) { m_bypass = b; }
		bool bypass() { return m_bypass; }
		void expanded( bool b ) { m_expanded = b; }
		bool expanded() { return m_expanded; }
	protected:
		bool m_bypass;
		bool m_expanded;
};
	
} /* namespace nle */

#endif /* _FILTER_BASE_ */
