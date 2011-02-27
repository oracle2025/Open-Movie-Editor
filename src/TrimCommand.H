/*  TrimCommand.H
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

#ifndef _TRIM_COMMAND_H_
#define _TRIM_COMMAND_H_

#include <stdint.h>

#include "Command.H"
#include "global_includes.H"

namespace nle
{

class Clip;

class TrimCommand : public Command
{
	public:
		TrimCommand( Clip* clip, int64_t trim, bool right );
		~TrimCommand() {}
		void doo();
		void undo();
	private:
		int m_track;
		int m_clipNr;
		bool m_right;
		int64_t m_trim;

};

} /* namespace nle */

#endif /* _TRIM_COMMAND_H_ */
