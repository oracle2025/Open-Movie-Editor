/*  Frei0rFactory.H
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

#ifndef _FREI0R_FACTORY_H_
#define _FREI0R_FACTORY_H_

#include "global_includes.H"

namespace nle
{
	
class IVideoEffect;
class IEffectMenu;
class Frei0rFactoryPlugin;
class FilterFactory;

typedef struct _effect_list_node {
	struct _effect_list_node* next;
	Frei0rFactoryPlugin* effect;
} effect_node;

class Frei0rFactory
{
	public:
		Frei0rFactory( IEffectMenu* menu );
		~Frei0rFactory();
		FilterFactory* get( string name );
	private:
		void enumerate( string folder, IEffectMenu* menu );
		effect_node* m_effects;
};

} /* namespace nle */

#endif /* _FREI0R_FACTORY_H_ */
