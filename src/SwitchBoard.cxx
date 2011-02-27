/*  SwitchBoard.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#include <cassert>

#include "SwitchBoard.H"
#include "Timeline.H"
#include "TimelineView.H"
#include "globals.H"

namespace nle
{

  /* private variables of this file */
  float f_zoom = 1.0; //0.2 -> 2.0


  /**
   * Perform any initialisation that we need to do to the switchboard
   */
  void InitSwitchboard()
  {
    // nothing required just now
  }

  float GetZoom() { return f_zoom; }

  void SetZoom(float zoom){ f_zoom = zoom; }

  void MoveCursor() 
  {
    g_timelineView->move_cursor(g_timeline->m_playPosition );
  }


} /* namespace nle */
