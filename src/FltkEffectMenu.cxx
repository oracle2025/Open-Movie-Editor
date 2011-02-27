/*  FltkEffectMenu.cxx
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

#include "FltkEffectMenu.H"
#include "global_includes.H"
#include "TimelineView.H"
#include "FilterFactory.H"

namespace nle
{

void effect_callback( void*, void* v ) {
	FilterFactory* effectFactory = (FilterFactory*)v;
	g_timelineView->addEffect( effectFactory );
}

FltkEffectMenu::FltkEffectMenu(  int x, int y, int w, int h, const char *l )
	: Fl_Menu_Button( x, y, w, h, l )
{
}	
FltkEffectMenu::~FltkEffectMenu()
{
}
void FltkEffectMenu::addEffect( FilterFactory* effectFactory )
{
	add( effectFactory->name(), 0, (Fl_Callback*)effect_callback, effectFactory, 0 );
}

} /* namespace nle */
