/*  TrimCommand.cxx
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

#include "TrimCommand.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "timeline/Clip.H"

namespace nle
{
	
TrimCommand::TrimCommand( Clip* clip, int64_t trim, bool right )
	: Command()
{
	m_track = clip->track()->num();
	m_clipNr = clip->id();
	m_right = right;
	if ( m_right ) {
		m_trim = ( clip->position() + clip->length() ) - trim;
	} else {
		m_trim = trim - clip->position();
	}
}
void TrimCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	if ( m_right ) {
		c->trimB( m_trim );
	} else {
		c->trimA( m_trim );
	}
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}
void TrimCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	if ( m_right ) {
		c->trimB( (-1) * m_trim );
	} else {
		c->trimA( (-1) * m_trim );
	}
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}


} /* namespace nle */
