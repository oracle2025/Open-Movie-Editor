/* GmerlinWidget.H
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

#ifndef _GMERLIN_WIDGET_H_
#define _GMERLIN_WIDGET_H_

#include "IEffectWidget.H"

#include <gavl/gavl.h>
extern "C" {
	#include <gmerlin/plugin.h>
}

namespace nle
{

class GmerlinEffect;
class GmerlinWidget;

typedef struct _gmerlin_widget_callback_info {
	GmerlinWidget* widget;
	const char* name;
	const bg_parameter_info_t* info;
} gmerlin_widget_callback_info;

class GmerlinWidget : public IEffectWidget
{
	public:
		GmerlinWidget( GmerlinEffect* effect );
		~GmerlinWidget();
		void setValue( const char* name, const bg_parameter_info_t* info, bg_parameter_value_t *v );
	private:
		GmerlinEffect* m_effect;
		gmerlin_widget_callback_info* m_infostack;
		const bg_fv_plugin_t*      m_filter;
		void*                      m_filter_instance;
};

} /* namespace nle */

#endif /* _GMERLIN_WIDGET_H_ */
