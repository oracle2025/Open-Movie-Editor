/*  DummyClip.cxx
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

#include "DummyClip.H"
#include "DummyClipArtist.H"

namespace nle
{
	
DummyClip::DummyClip( Track* track, string filename, int64_t position, int64_t length, int64_t A, int64_t B, int id )
	: Clip( track, position, id ), m_filename( filename ), m_fileLength( length )
{
	m_trimA = A;
	m_trimB = B;
	m_artist = new DummyClipArtist( this );
}

} /* namespace nle */
