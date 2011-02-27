/*  Track.H
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

#ifndef _TRACK_BASE_H_
#define _TRACK_BASE_H_

#include "timeline/Track.H"

namespace nle
{

class TrackBase : public Track
{
	public:
		virtual int fillBuffer( float* output, unsigned long frames, int64_t position );
	protected:
		TrackBase( int num, string name );
		clip_node* m_current;
	private:
		int64_t m_prev_position;
};

} /* namespace nle */


#endif /* _TRACK_BASE_H_ */
