/*  MoveCommand.cxx
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

#include "globals.H"
#include "MoveCommand.H"
#include "timeline/Clip.H"
#include "timeline/Track.H"
#include "Timeline.H"
#include "VideoTrack.H"



namespace nle
{

MoveCommand::MoveCommand( Clip* clip, Track* target, int64_t position )
	: Command()
{
	m_srcTrack = clip->track()->num();
	m_dstTrack = target->num();
	m_clipNr = clip->id();
	m_srcPosition = clip->position();
	m_dstPosition = position;
}

static void performCommand( int tn1, int tn2, int64_t pos, int clip )
{
	Track* t1 = g_timeline->getTrack( tn1 );
	Track* t2 = g_timeline->getTrack( tn2 );
	Clip* c = t1->getClip( clip );
	c->position( pos );
	if ( t1 != t2 ) {
		t1->removeClip( c );
		c->track( t2 );
		t2->addClip( c );
	}
	VideoTrack* t;
	t = dynamic_cast<VideoTrack*>( t2 ); //removeClip calls reconsiderFadeOver on t1
	if ( t ) { t->reconsiderFadeOver(); }
}

void MoveCommand::doo()
{
	performCommand( m_srcTrack, m_dstTrack, m_dstPosition, m_clipNr );
}

void MoveCommand::undo()
{
	performCommand( m_dstTrack, m_srcTrack, m_srcPosition, m_clipNr );
}

} /* namespace nle */
