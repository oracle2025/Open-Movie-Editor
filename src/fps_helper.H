/*  fps_helper.H
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

#ifndef _FPS_HELPER_H_
#define _FPS_HELPER_H_

#include "timebase.H"
#include "Timeline.H"

namespace nle
{

inline int64_t single_frame_length( playback_fps fps )
{
	switch ( fps ) {
		case PB_FPS_NONE:
			return -1;
		case PB_FPS_25_PAL:
			return ( 1200 * (int64_t)NLE_TIME_BASE / 30000 );
		case PB_FPS_2997_NTSC:
			return ( 1001 * (int64_t)NLE_TIME_BASE / 30000 );
		case PB_FPS_24:
			return ( 1250 * (int64_t)NLE_TIME_BASE / 30000 );
		case PB_FPS_15:
			return ( 2000 * (int64_t)NLE_TIME_BASE / 30000 );
		case PB_FPS_50:
			return ( 600 * (int64_t)NLE_TIME_BASE / 30000 );
		case PB_FPS_60:
			return ( 500 * (int64_t)NLE_TIME_BASE / 30000 );
	}
	return -1;
}
inline double seconds_frame_length( playback_fps fps ) {
	switch ( fps ) {
		case PB_FPS_NONE:
			return 0.04;
		case PB_FPS_25_PAL:
			return ( 0.04 );
		case PB_FPS_2997_NTSC:
			return ( 1.0 / 29.97 );
		case PB_FPS_24:
			return ( 1.0 / 24.0 );
		case PB_FPS_15:
			return ( 1.0 / 15.0 );
		case PB_FPS_50:
			return ( 1.0 / 50.0 );
		case PB_FPS_60:
			return ( 1.0 / 60.0 );
	}
	return 0.04;
}

} /* namespace nle */

#endif /* _FPS_HELPER_H_ */
