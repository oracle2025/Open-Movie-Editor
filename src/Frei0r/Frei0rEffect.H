/* Frei0rEffect.H
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _FREI0R_EFFECT_H_
#define _FREI0R_EFFECT_H_

#include "IVideoEffect.H"
#include "frei0r.h"
#include <gavl/gavl.h>

typedef f0r_instance_t (*f0r_construct_f)(unsigned int, unsigned int);
typedef void (*f0r_destruct_f)(f0r_instance_t);
typedef void (*f0r_update_f)(f0r_instance_t, double, const uint32_t*, uint32_t*);
typedef void (*f0r_get_param_info_f)(f0r_param_info_t *info, int param_index);
typedef void (*f0r_set_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);
typedef void (*f0r_get_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);  

namespace nle
{

class IEffectDialog;

class Frei0rEffect : public IVideoEffect
{
	public:
		Frei0rEffect( f0r_plugin_info_t* info, void* handle, int w, int h);
		~Frei0rEffect();
		LazyFrame* getFrame( LazyFrame* frame, int64_t position );
		f0r_plugin_info_t *getPluginInfo() { return m_info; }
		void getParamInfo( f0r_param_info_t *info, int param_index );
		void getValue( f0r_param_t param, int param_index );
		void setValue( f0r_param_t param, int param_index );
		const char* name();
		int numParams();
		IEffectDialog* m_dialog;
		IEffectDialog* dialog();
		IEffectWidget* widget();
		void writeXML( TiXmlElement* );
		void readXML( TiXmlElement* );
		FilterData* getFilterData();
		void setFilterData( FilterData* );
		const char* identifier();
	private:
		f0r_instance_t m_instance;
		unsigned char *m_frame;
		gavl_video_frame_t* m_gavl_frame;
		LazyFrame* m_lazy_frame;
		// Function Pointers:
		f0r_construct_f f0r_construct;
		f0r_destruct_f f0r_destruct;
		f0r_update_f f0r_update;
		f0r_get_param_info_f  f0r_get_param_info;
		f0r_set_param_value_f f0r_set_param_value;
		f0r_get_param_value_f f0r_get_param_value;
		f0r_plugin_info_t* m_info;
		int m_w;
		int m_h;
};

} /* namespace nle */

#endif /* _FREI0R_EFFECT_H_ */
