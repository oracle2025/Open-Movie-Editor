/*  GmerlinEffect.H
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

#ifndef _GMERLIN_EFFECT_H_
#define _GMERLIN_EFFECT_H_

#include "IVideoEffect.H"

#include <string>

#include <gavl/gavl.h>
extern "C" {
#include <gmerlin/pluginregistry.h>
}

namespace nle
{

class VideoEffectClip;

class GmerlinEffect : public IVideoEffect
{
	public:
		GmerlinEffect( const char* identifier, bg_plugin_handle_t* plugin_handle, VideoEffectClip* clip );
		~GmerlinEffect();
		LazyFrame* getFrame( LazyFrame* frame, int64_t position = -1 );
		const char* name();
		int numParams() { return 1; }
		IEffectDialog* dialog();
		IEffectWidget* widget();
		const char* identifier();
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		LazyFrame* m_input_lazy_frame;
		gavl_video_format_t input_format;
		const bg_fv_plugin_t* filter() { return m_filter; }
		bg_cfg_section_t* config() { return m_cfg_section; }
		void* instance() { return m_filter_instance; }
		int numberOfParams();
	private:
		bg_plugin_handle_t*        m_plugin_handle;
		const bg_fv_plugin_t*      m_filter;
		void*                      m_filter_instance;
		const bg_parameter_info_t* m_parameters;
		LazyFrame*                 m_lazy_frame;
		gavl_video_frame_t*        m_gavl_frame;
		std::string                m_identifier;
		bg_cfg_section_t*          m_cfg_section;
		
};

} /* namespace nle */

#endif /* _GMERLIN_EFFECT_H_ */
