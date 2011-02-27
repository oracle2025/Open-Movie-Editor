/*  AddCommand.cxx
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


#include "AddCommand.H"
#include "timeline/Clip.H"
#include "Timeline.H"
#include "VideoTrack.H"

namespace nle
{

AddCommand::AddCommand( string filename, Track* target, int64_t position )
	: Command()
{
	m_track = target->num();
	m_clipNr = g_timeline->getClipId();
	m_position = position;
	m_filename = filename;
}

void AddCommand::doo()
{
	g_timeline->addFile( m_track, m_position, m_filename, 0, 0, false, m_clipNr );
	Track* t = g_timeline->getTrack( m_track );
	VideoTrack* vt = dynamic_cast<VideoTrack*>( t );
	if ( vt ) { vt->reconsiderFadeOver(); }
}

void AddCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr );
	t->removeClip( c );
	delete c;
}
const char* AddCommand::serialize()
{
	static char buffer[1024];
	snprintf( buffer, 1024, "addclip filename=\"%s\" track=%d position=%lld id=%d;", m_filename.c_str(), m_track, m_position, m_clipNr );
	return buffer;
}

} /* namespace nle */
