/* Frei0rFactoryPlugin.H
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

#ifndef _FREI0R_FACTORY_PLUGIN_H_
#define _FREI0R_FACTORY_PLUGIN_H_

#include "frei0r.h"
#include "global_includes.H"
#include "FilterFactory.H"

typedef void (*f0r_deinit_f)(void);
typedef int (*f0r_init_f)(void);
typedef void (*f0r_get_plugin_info_f)(f0r_plugin_info_t *info);

namespace nle
{

class Frei0rFactoryPlugin : public FilterFactory
{
	public:
		Frei0rFactoryPlugin( string filename );
		~Frei0rFactoryPlugin();
		FilterBase* get( Clip* clip );
		//IVideoEffect* get( int w, int h );
		bool ok() { return m_ok; }
		const char* name();
		const char* identifier();
	private:
		f0r_init_f            f0r_init;
		f0r_deinit_f          f0r_deinit;
		f0r_get_plugin_info_f f0r_get_plugin_info;
		void *m_handle;
		f0r_plugin_info_t m_info;
		bool m_ok;
};

} /* namespace nle */

#endif /* _FREI0R_FACTORY_PLUGIN_H_ */
