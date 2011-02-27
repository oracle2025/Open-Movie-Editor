/* NodeFilter.H
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

#ifndef _NODE_FILTER_H_
#define _NODE_FILTER_H_

#include "IVideoEffect.H"
#include <string>
#include <stdint.h>
#include <gavl/gavl.h>

const int MAX_FILTER_OUT = 20;
const int MAX_FILTER_IN = 20;

class INode;
class Fl_Widget;

typedef struct _filters {
	struct _filters* next;
	int id;
	int x;
	int y;
	int w;
	int h;
	int input_count;
	int output_count;
	struct _filters* inputs[MAX_FILTER_IN];
	struct _filters* outputs[MAX_FILTER_OUT];
	int target_slots[MAX_FILTER_OUT];
	std::string name;
	INode* node;
	Fl_Widget* widgets[20];
} filters;

namespace nle
{

class NodeFilter : public IVideoEffect
{
	public:
		NodeFilter( int w, int h );
		~NodeFilter();
		void clear();
		void init();
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		const char* identifier() { return "effect:builtin:NodeFilter"; }
		const char* name();
		LazyFrame* getFrame( LazyFrame* frame, int64_t position = -1 );
		int numParams() { return 1; }
		bool hasDialog() { return true; }
		IEffectDialog* dialog();
		IEffectDialog* m_dialog;
		filters* m_filters;
		INode* m_sink_node;
		INode* m_src_node;
		int m_w;
		int m_h;
		uint32_t* m_frame_cache;
	private:
		LazyFrame* m_lazy_frame;
		gavl_video_frame_t* m_gavl_frame;
};
	
} /* namespace nle */

#endif /* _NODE_FILTER_H_ */
