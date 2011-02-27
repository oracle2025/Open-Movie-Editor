/*  TimelineBase.cxx
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

#include <iostream>

#include <sl.h>

#include "TimelineBase.H"
#include "Track.H"

using namespace std;


namespace nle
{


TimelineBase::TimelineBase()
{
	m_allTracks = 0;
	m_clipId = 1;
}
TimelineBase::~TimelineBase()
{
	clear();
}
int TimelineBase::getClipId() { return m_clipId++; }
void TimelineBase::updateClipId( int id )
{
	if ( id >= m_clipId ) {
		m_clipId = id + 1;
	}	
}

void TimelineBase::clear()
{
	track_node* node;
	while ( ( node = (track_node*)sl_pop( &m_allTracks ) ) ) {
		delete node->track;
		delete node;
	}
}

static int sort_track_helper( void* p, void* )
{
	track_node* node = (track_node*) p;
	node->track->sort();
	return 0;
}
void TimelineBase::sort()
{
	sl_map( m_allTracks, sort_track_helper, 0 );
}

void TimelineBase::addClip( int track, Clip *clip )
{
	Track *t = getTrack( track );
	if ( t ) {
		t->addClip( clip );
	} else {
		cerr << "No such track" << endl;
	}
}
void TimelineBase::addFile( int track, int64_t position, std::string filename, int64_t trimA, int64_t trimB, int mute, int id, int64_t length, ClipData* data )
{
	Track *t = getTrack( track );
	if ( t ) {
		t->addFile( position, filename, trimA, trimB, mute, id, length, data );
	} else {
		cerr << "No such track" << endl;
	}
}

void TimelineBase::removeClip( int track, Clip *clip )
{
	Track *t = getTrack( track );
	if ( t ) {
		t->removeClip( clip );
	} else {
		cerr << "No such track" << endl;
	}
}

void TimelineBase::addTrack( Track* track, int position )
{
	track_node* node = new track_node;
	node->track = track;
	node->next = 0;
	if ( position < 0 ) {
		m_allTracks = (track_node*)sl_unshift( m_allTracks, node );
	} else if ( position == 0 ) {
		node->next = m_allTracks;
		m_allTracks = node;
	} else {
		track_node* p;
		int i = 1;
		for ( p = m_allTracks; p->next && i < position; p = p->next, i++ ) {
			// nothing;
		}
		node->next = p->next;
		p->next = node;
	}
}
static track_node* sl_swap( track_node* root )
{
	track_node* q = root;
	track_node* r;
	if ( !q || !q->next ) {
		return q;
	}
	r = q->next;
	q->next = r->next;
	r->next = q;
	return r;
}
void TimelineBase::trackUp( Track* track )
{
	if ( !m_allTracks || m_allTracks->track == track ) {
		return;
	}
	track_node* p = m_allTracks;
	if ( p && p->next && p->next->track == track ) {
		m_allTracks = sl_swap( p );
		return;
	}
	while (  p && p->next && p->next->next && p->next->next->track != track ) {
		p = p->next;
	}
	if ( !p || !p->next || !p->next->next || p->next->next->track != track ) {
		return;
	}
	p->next = sl_swap( p->next );
}
void TimelineBase::trackDown( Track* track )
{
	track_node* p = m_allTracks;
	if ( p && p->track == track ) {
		m_allTracks = sl_swap( p );
		return;
	}
	while ( p && p->next && p->next->track != track ) {
		p = p->next;
	}
	if ( !p || !p->next || p->next->track != track ) {
		return;
	}
	p->next = sl_swap( p->next );
}
static int remove_track_helper( void* p, void* data )
{
	int* track = (int*)data;
	track_node* node = (track_node*)p;
	if ( node->track->num() == *track ) {
		return 1;
	}
	return 0;
}
void TimelineBase::removeTrack( int track )
{
	track_node* node = (track_node*)sl_remove( &m_allTracks, remove_track_helper, &track );
	delete node->track;
	delete node;
}

Clip* TimelineBase::find( int track, int64_t position )
{
	Track* t = getTrack( track );
	if ( t ) {
		return t->find( position );
	} else {
		cerr << "No such track" << endl;
	}
	return 0;
}

static int track_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	track_node* node = (track_node*)p;
	l = (int64_t)(node->track->length() / node->track->stretchFactor());
	if ( l > *max ) {
		*max = l;
	}
	return 0;
}
int64_t TimelineBase::length()
{
	int64_t max = 0;
	sl_map( m_allTracks, track_length_helper, &max );
	return max;
}

Track* TimelineBase::getTrack( int track )
{
	track_node* node = (track_node*)m_allTracks;
	while ( node && node->track->num() != track ) {
		node = node->next;
	}
	if ( node ) {
		return node->track;
	}
	return 0;
}
Clip* TimelineBase::getClip( int track, int clip )
{
	Track* t = getTrack( track );
	if ( !t ) { return 0; }
	return t->getClip( clip );
}


} /* namespace nle */
