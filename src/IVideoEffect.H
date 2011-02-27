/* IVideoEffect.H
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
#ifndef _I_VIDEO_EFFECT_H_
#define _I_VIDEO_EFFECT_H_


#include "FilterBase.H"
namespace nle
{

class IEffectDialog;
class LazyFrame;

class IVideoEffect : public FilterBase
{
	public:
		virtual ~IVideoEffect() {}
		virtual LazyFrame* getFrame( LazyFrame* frame, int64_t position = -1 ) = 0;
		virtual const char* name() = 0;
		virtual int numParams() = 0;
		virtual IEffectDialog* dialog() = 0;
		/*
		 * This Class is empty today, but will be populated soon
		 */
};

} /* namespace nle */

#endif /* _I_VIDEO_EFFECT_H_ */
