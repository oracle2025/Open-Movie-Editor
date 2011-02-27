/*  FilterClip.H
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

#ifndef _FILTER_CLIP_H_
#define _FILTER_CLIP_H_

#include "timeline/Clip.H"

namespace nle
{

class FilterBase;
class FilterFactory;

typedef struct _filter_stack {
	struct _filter_stack* next;
	FilterBase* filter;
	/* AudioFilter* afilter;
	 * VideoFilter* vfilter;
	 * to avoid a dynamic_cast in thight loops
	 */
} filter_stack;

class FilterClip : public Clip
{
	public:
		virtual ~FilterClip();
		FilterBase* pushFilter( FilterFactory* factory );
		FilterBase* appendFilter( FilterFactory* factory );
		FilterBase* insertFilter( FilterFactory* factory, int position );
		void moveFilterUp( int num );
		void moveFilterUp( FilterBase* filter );
		void moveFilterDown( int num );
		void moveFilterDown( FilterBase* filter );
		void removeFilter( int num );
		FilterBase* getFilter( int num );
		void popFilter();
		filter_stack* getFilters() { return m_filters; }
		virtual void reset();
	protected:
		FilterClip( Track* track, int64_t position, int id = -1 );
		filter_stack* m_filters;
};


} /* namespace nle */

#endif /* _FILTER_CLIP_H_ */
