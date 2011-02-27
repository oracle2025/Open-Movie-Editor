/*  AudioVolumeFilter.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include <FL/fl_draw.H>
#include <tinyxml.h>
#include "sl/sl.h"

#include "AudioVolumeFilter.H"
#include "TimelineView.H"
#include "AudioClip.H"
#include "timeline/Track.H"
#include "AutomationDragHandler.H"

namespace nle
{

AudioVolumeFilter::AudioVolumeFilter( AudioClip* clip )
{
	m_audioClip = clip;
	m_automationPoints = 0;
	m_autoCache = 0;
	
	auto_node* r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = m_audioClip->audioLength();
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	r = new auto_node;
	r->next = 0;
	r->y = 1.0;
	r->x = 0;
	m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
	m_expanded = true;
	m_bypass = false;
}
AudioVolumeFilter::~AudioVolumeFilter()
{
	auto_node* node;
	while ( ( node = (auto_node*)sl_pop( &m_automationPoints ) ) ) {
		delete node;
	}	
}
void AudioVolumeFilter::onDraw( Rect& rect )
{
	Track* track = m_audioClip->track();

	float stretchF;
	if ( track->type() == TRACK_TYPE_AUDIO ) {
		stretchF = track->stretchFactor();
	} else {
		stretchF = ( 48000 );
	} 
	//fl_push_clip( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );

	auto_node* nodes = m_automationPoints;

	fl_color( FL_RED );
	for ( ; nodes && nodes->next; nodes = nodes->next ) {
		int y = (int)( rect.y + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) + 5 );
		int y_next = (int)( rect.y + ( ( track->h() - 10 ) * ( 1.0 - nodes->next->y ) ) + 5 );
		fl_line( g_timelineView->get_screen_position( m_audioClip->audioPosition() + nodes->x, stretchF ),
				y,
				g_timelineView->get_screen_position( m_audioClip->audioPosition() + nodes->next->x, stretchF ),
				y_next );
	}
	nodes = m_automationPoints;
	for ( ; nodes; nodes = nodes->next ) {
		//consider Trimming
		int x;
		int y = (int)( rect.y + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) );
		if ( !nodes->next ) {
			x = g_timelineView->get_screen_position( m_audioClip->audioPosition() + nodes->x, stretchF ) - 10;
		} else if ( nodes == m_automationPoints ) {
			x = g_timelineView->get_screen_position( m_audioClip->audioPosition() + nodes->x, stretchF );
		} else {
			x = g_timelineView->get_screen_position( m_audioClip->audioPosition() + nodes->x, stretchF ) - 5;
		}
		fl_draw_box( FL_UP_BOX, x, y, 10, 10, FL_RED );
	}
	//fl_pop_clip();

	
}
DragHandler* AudioVolumeFilter::onMouseDown( Rect& rect, int x, int y, bool /*shift*/ )
{
	return new AutomationDragHandler( m_audioClip, rect, m_automationPoints, x, y );
}
int AudioVolumeFilter::fillBuffer( float* input_output, unsigned long frames, int64_t position )
//int AudioVolumeFilter::fillBuffer( float*, unsigned long frames, int64_t )
{
	if ( m_bypass ) {
		return frames;
	}
	int64_t currentPosition = m_audioClip->audioPosition();
	int64_t aLength = m_audioClip->audioLength();
	//Manipulate output

	int64_t start_output = currentPosition > position ? currentPosition - position : 0;
	int64_t start_clip = currentPosition > position ? currentPosition : position ;
	int64_t count = currentPosition + aLength - position - start_output;
	if ( count + start_output > frames ) {
		count = frames - start_output;
	}
	float envelope;
	for ( int64_t i = 0; i < count; i++ ) {
		envelope = getEnvelope( start_clip + i );
		input_output[(i*2)] = input_output[(i*2)] * envelope;
		input_output[(i*2) + 1] = input_output[(i*2) + 1] * envelope;
	}
	if ( count < 0 ) { count = 0; }
	count = count * 2;
	for ( int64_t i = (frames * 2) - 1; i >= count; i-- ) {
		input_output[i] = 0.0;
	}
	return frames;
}
void AudioVolumeFilter::reset()
{
	m_autoCache = m_automationPoints;
}
int64_t AudioVolumeFilter::trimA( int64_t trim )
{
	float last_y = m_automationPoints->y;
	int64_t last_x = 0;
	if ( trim > 0 ) {
		auto_node* n = m_automationPoints;
		while ( 1 ) {
			if ( n->x < trim ) {
				if ( n != m_automationPoints ) {
					cerr << "Fatal, this shouldn't happen (AudioClip::trimA)" << endl;
					return trim;
				}
				last_y = n->y;
				last_x = n->x;
				n = n->next;
				delete (auto_node*)sl_pop( &m_automationPoints );
			} else {
				auto_node* r = new auto_node;
				r->y = ( ( n->y - last_y ) * ( (float)(trim - last_x) / (float)(n->x - last_x) ) ) + last_y;
				r->x = 0;
				r->next = 0;
				m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
				auto_node* q = r->next;
				for ( ; q; q = q->next ) {
					q->x -= trim;
				}
				break;
			}
			if ( !n ) {
				break;
			}
		}
	} else {
		auto_node* n = m_automationPoints;
		if ( n->y == n->next->y ) {
			n->x = 0;
			n = n->next;
			for ( ; n; n = n->next ) {
				n->x -= trim;
			}
		} else {
			auto_node* r = new auto_node;
			r->y = n->y;
			r->x = 0;
			r->next = 0;
			m_automationPoints = (auto_node*)sl_push( m_automationPoints, r );
			for ( ; n; n = n->next ) {
				n->x -= trim;
			}
		}
	}
	return trim;
}
int64_t AudioVolumeFilter::trimB( int64_t trim )
{
	if ( trim > 0 ) {
		//etwas von den Automations entfernen
		auto_node* n = m_automationPoints;
		while ( n->next && m_audioClip->audioLength() - trim > n->next->x ) {
			n = n->next;
		}
		int64_t next_x = n->next->x;
		float next_y = n->next->y;
		auto_node* r = n->next;
		while ( r ) {
			auto_node* q = r;
			r = r->next;
			delete q;
		}
		r = new auto_node;
		r->y = ( ( next_y - n->y ) * ( (float)( ( m_audioClip->audioLength() - trim ) - n->x) / (float)(next_x - n->x) ) ) + n->y;
		r->x = m_audioClip->audioLength() - trim;
		r->next = 0;
		n->next = r;
	} else {
		// evtl. etwas hinzufügen
		auto_node* n = m_automationPoints;
		while ( n->next->next ) {
			n = n->next;
		}
		if( n->y == n->next->y ) {
			n->next->x = m_audioClip->audioLength() - trim;
		} else {
			n = n->next;
			auto_node* r = new auto_node;
			r->y = n->y;
			r->x = m_audioClip->audioLength() - trim;
			r->next = 0;
			n->next = r;
		}
	}
	return trim;
}
float AudioVolumeFilter::getEnvelope( int64_t position )
{
	int64_t pPos = position - m_audioClip->audioPosition();
	if ( m_autoCache && m_autoCache->x <= pPos &&  m_autoCache->next && m_autoCache->next->x > pPos ) {
		int64_t diff = m_autoCache->next->x - m_autoCache->x;
		float diff_y = m_autoCache->next->y - m_autoCache->y;
		float inc = diff_y / diff;
		return ( inc * ( pPos - m_autoCache->x ) ) + m_autoCache->y;
	} else {
		if ( m_autoCache ) {
			m_autoCache = m_autoCache->next;
			return getEnvelope( position );
		}
	}
	return 0.0;

}
void AudioVolumeFilter::writeXML( TiXmlElement* xml_node )
{
	TiXmlElement* automation;
	auto_node* q = m_automationPoints;
	for ( ; q; q = q->next ) {
		automation = new TiXmlElement( "automation" );
		xml_node->LinkEndChild( automation );
		automation->SetAttribute( "x", q->x );
		automation->SetDoubleAttribute( "y", q->y );
	}
}
void AudioVolumeFilter::readXML( TiXmlElement* xml_node )
{
	auto_node* autonode = m_automationPoints;
	TiXmlElement* automation = TiXmlHandle( xml_node ).FirstChildElement( "automation" ).Element();
	int x;
	double y;
	bool fff = false;
	for ( ; automation; automation = automation->NextSiblingElement( "automation" ) ) {
		if ( fff ) {
			if ( !autonode->next ) {
				autonode->next = new auto_node;
				autonode->next->next = 0;
				autonode->next->x = autonode->x + 1;
				autonode->next->y = 1.0;
			}
			autonode = autonode->next;
		} else {
			fff = true;
		}
		if ( ! automation->Attribute( "x", &x ) )
			continue;
		if ( ! automation->Attribute( "y", &y ) )
			continue;
		autonode->x = x;
		autonode->y = y;
	}

}

} /* namespace nle */



