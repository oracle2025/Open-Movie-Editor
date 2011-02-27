/*  Clip.cxx
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

#include "Clip.H"
#include "ClipIdProvider.H"
#include "Track.H"

namespace nle
{


Clip::Clip( Track *track, int64_t position, int id )
{
	m_position = position;
	m_track = track;
	m_trimA = 0;
	m_trimB = 0;
	if ( id < 0 ) {
		if (track) m_id = track->getClipId();
	} else {
		m_id = id;
		if (track) track->updateClipId( id );
	}
	m_selected = false;
}
int64_t Clip::trimA( int64_t trim )
{
	if ( length() - trim <= 0 ) {
		return 0;
	}
	m_position = m_position - m_trimA;
	if ( ( m_trimA + trim ) < 0 ) {
		trim = m_trimA;
	}
	m_trimA += trim;
	m_position = m_position + m_trimA;
	return trim;
}
int64_t Clip::trimB( int64_t trim )
{
	if ( length() - trim <= 0 ) {
		return 0;
	}
	if ( ( m_trimB + trim) < 0 ) {
		trim = m_trimB;
	}
	m_trimB += trim;
	return trim;
}

} /* namespace nle */
