/*  Track.cxx
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

#include <algorithm>
#include <iostream>

using namespace std;

#include "Track.H"
#include "Clip.H"
#include "../SwitchBoard.H"




namespace nle
{

Track::Track( int num, string name )
{
	m_name = name;
	m_num = num;
	m_clips = 0;
	m_height = 30;
}
Track::~Track()
{
	clip_node* node;
	while ( ( node = (clip_node*)sl_pop( &m_clips ) ) ) {
		delete node->clip;
		delete node;
	}
}
void Track::addClip( Clip* clip )
{
	clip_node* node = new clip_node;
	node->clip = clip;
	node->next = 0;
	m_clips = (clip_node*)sl_push( m_clips, node );
}
static int remove_clip_helper( void* p, void* data )
{
	Clip* clip = (Clip*)data;
	clip_node* node = (clip_node*)p;
	if ( node->clip == clip ) {
		return 1;
	} else {
		return 0;
	}
}
void Track::removeClip( Clip* clip )
{
	clip_node* n = (clip_node*)sl_remove( &m_clips, remove_clip_helper, clip );
	if ( n ) {
		delete n;
	}
}
static int find_clip_helper( void* p, void* data )
{
	clip_node* node = (clip_node*)p;
	Clip* clip = node->clip;
	int64_t position = *((int64_t*)data);
	if ( clip->A() <= position && clip->B() >= position ) {
		return 1;
	} else {
		return 0;
	}
}
Clip *Track::find( int64_t position )
{
	clip_node* node = (clip_node*)sl_map( m_clips, find_clip_helper, &position );
	if ( node ) {
		return node->clip;
	}
	return 0;
}
Clip* Track::getClip( int id )
{
	clip_node* node = m_clips;
	while ( node && node->clip->id() != id ) {
		node = node->next;
	}
	if ( node ) { return node->clip; }
	return 0;
}
static int reset_clip( void* p, void* )
{
	clip_node* node = (clip_node*)p;
	node->clip->reset();
	return 0;
}
static int cmp_clip(void *p1, void *p2)
{
        clip_node *q1 = (clip_node*)p1, *q2 = (clip_node*)p2;
        return q1->clip->position() > q2->clip->position();
}       
void Track::sort()
{
	m_clips = (clip_node*)sl_mergesort( m_clips, cmp_clip );
	sl_map( m_clips, reset_clip, 0 );
}
#define SNAP_TOLERANCE 20
static bool is_in_tolerance( int64_t A, int64_t B, unsigned int tolerance )
{
  unsigned int t = (unsigned int)( tolerance / GetZoom() );
	return ( B - t <= A && B + t >= A );
}
int64_t Track::getSnapA( Clip* clip, int64_t A )
{
	for ( clip_node *p = m_clips; p; p = p->next ) {
		Clip* current = p->clip;
		int64_t B = current->B();
		if ( current != clip && is_in_tolerance( A, B, (unsigned int)(SNAP_TOLERANCE * stretchFactor()) ) ) {
			return B + 1;
		}
	}
	return A;
}
int64_t Track::getSnapB( Clip* clip, int64_t B )
{
	for ( clip_node *p = m_clips; p; p = p->next ) {
		Clip* current = p->clip;
		int64_t A = current->position();
		if ( current != clip && is_in_tolerance( B + clip->length(), A, (unsigned int)(SNAP_TOLERANCE * stretchFactor()) ) ) {
			return A - 1 - clip->length();
		}
	}
	return B;
}
static int clip_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	clip_node* node = (clip_node*)p;
	l = node->clip->length() + node->clip->position();
	if ( l > *max ) {
		*max = l;
	}
	return 0;
}
int64_t Track::length()
{
	int64_t max = 0;
	sl_map( m_clips, clip_length_helper, &max );
	return (int64_t)( max );
}

	
} /* namespace nle */
