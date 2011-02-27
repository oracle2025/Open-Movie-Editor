/*  SplitCommand.cxx
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

#include "SplitCommand.H"
#include "Timeline.H"
#include "VideoTrack.H"
#include "VideoClip.H"
#include "AudioClip.H"
#include "AudioFileFactory.H"
#include "TitleClip.H"

namespace nle
{

SplitCommand::SplitCommand( Clip* clip, int64_t position )
{
	m_automationsCount = 0;
	m_automationPoints = 0;
	m_track = clip->track()->num();
	m_clipNr1 = clip->id();
	m_clipNr2 = g_timeline->getClipId();
	m_position = position;
	m_length = clip->length();
	m_audioClip = 0;
	m_titleClip = 0;
	m_data = clip->getClipData();
	TitleClip* tc = dynamic_cast<TitleClip*>(clip);
	if ( tc ) {
		m_titleClip = true;
	}
}
SplitCommand::~SplitCommand()
{
	if ( m_data ) {
		delete m_data;
	}
}
void SplitCommand::doo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr1 );
	int mute = 0;
	if ( VideoClip* vc = dynamic_cast<VideoClip*>(c) ) {
		mute = vc->m_mute;
	}

	if ( m_audioClip ) {
		IAudioFile *af = AudioFileFactory::get( c->filename() );
		if ( !af ) { return; }
		int64_t trim = m_position - c->position();
		AudioClip* ac = new AudioClip( t, m_position, af, trim + c->trimA(), c->trimB(), m_clipNr2 );
		g_timeline->addClip( m_track, ac );
	} else if ( m_titleClip ) {
		TitleClip* tc = new TitleClip( t, m_position, m_length - ( m_position - c->position() ) + c->trimA() - c->trimB(), m_clipNr2, m_data );
		g_timeline->addClip( m_track, tc );
	} else {
		g_timeline->addFile( m_track, m_position, c->filename(), ( m_position - c->position() ) + c->trimA(), c->trimB(), mute, m_clipNr2, m_length, m_data );
	}
	c->trimB( ( c->position() + c->length() ) - m_position );
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}
void SplitCommand::undo()
{
	Track* t = g_timeline->getTrack( m_track );
	Clip* c = t->getClip( m_clipNr2 );
	int64_t l = c->length();
	t->removeClip( c );
	delete c;
	c = t->getClip( m_clipNr1 );
	c->trimB( (-1) * l );
	VideoTrack* vt = dynamic_cast<VideoTrack*>(t);
	if ( vt ) { vt->reconsiderFadeOver(); }
}

} /* namespace nle */


