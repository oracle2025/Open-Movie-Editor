/*  Timeline.cxx
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

#include <cmath>
#include <cassert>
#include <tinyxml.h>
#include <fstream>

#include <FL/filename.H>

#include "Timeline.H"
#include "VideoTrack.H"
#include "AudioTrack.H"
#include "VideoClip.H"
#include "timeline/Track.H"
#include "render_helper.H"
#include "helper.H"
#include "globals.H"
#include "config.h"

#include "DummyClip.H"
#include "VideoFileFactory.H"
#include "TitleClip.H"
#include "ImageClip.H"
#include "MainFilterFactory.H"
#include "FilterBase.H"
#include "AudioFileFactory.H"

#include <cstring>
#include "strlcpy.h"
#include "TimelineView.H"
#include "InkscapeClip.H"
#include "LazyFrame.H"

namespace nle
{

Timeline* g_timeline = 0;


Timeline::Timeline()
	: TimelineBase()
{
	m_render_mode = false;
	m_trackId = 0;
	VideoTrack *vt;
	AudioTrack *at;
	vt = new VideoTrack( this, getTrackId() );
	addTrack( vt );
	vt = new VideoTrack( this, getTrackId() );
	addTrack( vt );
	at = new AudioTrack( this, getTrackId() );
	addTrack( at );
	at = new AudioTrack( this, getTrackId() );
	addTrack( at );
	m_playPosition = 0;
	m_samplePosition = 0;

	g_timeline = this; //Singleton sucks, this is better :)

	m_seekPosition = 0;
	m_soundLength = 0;
	m_changed = false;
	m_playback_fps = PB_FPS_NONE;

	m_blended_gavl_frame = 0;
	m_blended_lazy_frame = 0;
}

Timeline::~Timeline()
{
	g_timeline = NULL;
	if ( m_blended_gavl_frame ) {
		gavl_video_frame_destroy( m_blended_gavl_frame );
		m_blended_gavl_frame = 0;
	}
	if ( m_blended_lazy_frame ) {
		delete m_blended_lazy_frame;
		m_blended_lazy_frame = 0;
	}
}
int Timeline::getTrackId() { return m_trackId++; }
void reset_helper( Track* track ) { track->sort(); }
static int video_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	track_node* node = (track_node*)p;
	if ( node->track->type() == TRACK_TYPE_VIDEO ) {
		l = node->track->length();
		if ( l > *max ) {
			*max = l;
		}
	}
	return 0;
}
static int audio_length_helper( void* p, void* data )
{
	int64_t l;
	int64_t* max = (int64_t*)data;
	track_node* node = (track_node*)p;
	if ( AudioTrack* t = dynamic_cast<AudioTrack*>(node->track) ) {
		l = t->soundLength();
		if ( l > *max ) {
			*max = l;
		}
	}
	return 0;
}

void Timeline::sort()
{
	TimelineBase::sort();
	m_playPosition = m_seekPosition;
	m_samplePosition = int64_t( m_seekPosition * 48000 / NLE_TIME_BASE );
	{
		int64_t audio_max = 0;
		int64_t video_max = 0;
		sl_map( m_allTracks, audio_length_helper, &audio_max );
		sl_map( m_allTracks, video_length_helper, &video_max );
		video_max = (int64_t)( video_max * 48000 / NLE_TIME_BASE );
		m_soundLength = video_max > audio_max ? video_max : audio_max;
	}
}
LazyFrame* Timeline::getFrame( int64_t position )
{
	return nextFrame( position );
}
LazyFrame* Timeline::nextFrame( int64_t position ) //only used from VideoViewGL
{
	static int64_t last_frame = -1;
	LazyFrame* res = NULL;
	if ( position < 0 || last_frame < 0 || last_frame + 1200 == position ) {
		m_playPosition += 1200;
	} else {
		m_playPosition = position;
	}
	last_frame = m_playPosition;
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		res = current->getFrame( m_playPosition - 1200 );
		if ( res )
			return res;
	}
	return res;
}
LazyFrame** Timeline::getFrameStack( int64_t position )
{
	static LazyFrame* frameStack[8]; //At most 8 Frames, ought to be enough for everyone ;)
	int cnt = 0;

	if ( position < 0 ) {
		position = m_seekPosition;
		m_seekPosition += 35280000 / (int)g_fps;
	}
	
	m_playPosition = position;
	
	for ( track_node *p = m_allTracks; p; p = p->next ) {
		VideoTrack* current = dynamic_cast<VideoTrack*>(p->track);
		if ( !current ) {
			continue;
		}
		LazyFrame** fs = current->getFrameStack( position );
		
		for ( int i = 0; fs[i] && cnt <=7 ; i++ ) {
			frameStack[cnt] = fs[i];
			cnt++;
		}
		if ( cnt == 7 ) {
			break;
		}
	}
	frameStack[cnt] = 0;
	return frameStack;
}
int Timeline::fillBuffer( float* output, unsigned long frames )
{
	memset( buffer1, 0, sizeof(buffer1) );
	memset( buffer2, 0, sizeof(buffer2) );

	unsigned int rv;
	unsigned int max_frames = 0;
	track_node* p = m_allTracks;
	assert( frames <= 32000);
	if ( !p )
		return 0;
	rv = (dynamic_cast<TrackBase*>(p->track))->fillBuffer( buffer1, frames, m_samplePosition );
	max_frames = rv;
	p = p->next;
	if ( !p ) { //Only one Track
		for ( unsigned long i = 0; i < frames * 2; i += 2 ) {
			output[i] = buffer1[i];
			output[i+1] = buffer1[i+1];
		}
		while ( max_frames < frames && m_samplePosition + max_frames < m_soundLength ) {
			output[max_frames] = 0.0;
			max_frames++;
		}
// 		this is done via sampleseek() from the audio thread
//		m_samplePosition += max_frames;
		return max_frames;
	}
	assert( p );
	assert( p->track );
	TrackBase* tb = dynamic_cast<TrackBase*>(p->track);
	rv = tb->fillBuffer( buffer2, frames, m_samplePosition );
	max_frames = rv > max_frames ? rv : max_frames;
	mixChannels( buffer1, buffer2, output, frames );
	p = p->next;
	while ( p ) {
		rv = (dynamic_cast<TrackBase*>(p->track))->fillBuffer( buffer1, frames, m_samplePosition );
		max_frames = rv > max_frames ? rv : max_frames;
		mixChannels( output, buffer1, output, frames );
		p = p->next;
	}
	while ( max_frames < frames && m_samplePosition + max_frames < m_soundLength ) {
		output[max_frames] = 0.0;
		max_frames++;
	}
//	this is done via sampleseek() from the audio thread
//	m_samplePosition += max_frames;

	return max_frames;
}
LazyFrame* Timeline::getBlendedFrame()
{
	return getBlendedFrame( -1 );
}
LazyFrame* Timeline::getBlendedFrame( int64_t position )
{
	LazyFrame** fs = getFrameStack( position );
	int len = m_blended_lazy_frame->format()->frame_width * m_blended_lazy_frame->format()->frame_height;
	unsigned char* dst_buffer = m_blended_gavl_frame->planes[0];
	gavl_video_frame_clear( m_blended_gavl_frame, m_blended_lazy_frame->format() );

	int start = 0;
	int stop = 0;

	for ( int i = 0; fs[i]; i++ ) {
		start = i + 1;
	}
	if ( start == 1 ) {
		return fs[0];
	}
	if ( !fs[start] ) {
		start--;
		if ( start >= 0 ) {
			unsigned char *src, *dest, *end;
			fs[start]->set_target( m_blended_lazy_frame->format() );
			src = fs[start]->target()->planes[0];
			dest = dst_buffer;
			end = fs[start]->target()->planes[0] + ( len * 4 );
			while ( src < end ) {
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
				dest[4] = src[4];
				dest += 4;
				src += 4;
			}
		}
	}
	start--;
	gavl_video_frame_t* gavl_frame = 0;
	for ( int i = start; i >= stop; i-- ) {
		fs[i]->set_target( m_blended_lazy_frame->format() );
		gavl_frame = fs[i]->target();
		blend_alpha2( dst_buffer, dst_buffer, gavl_frame->planes[0], fs[i]->alpha(), len );
	}

	return m_blended_lazy_frame;
	
	// 0: ganz oben
	// 1: 
	// 2: ganz unten, zuerst blitten

}
void Timeline::prepareFormat( const gavl_video_format_t* format )
{
	m_blended_lazy_frame = new LazyFrame( format );
	m_blended_gavl_frame = gavl_video_frame_create( format );
	m_blended_lazy_frame->put_data( m_blended_gavl_frame );
}
void Timeline::unPrepareFormat()
{
	if ( m_blended_gavl_frame ) {
		gavl_video_frame_destroy( m_blended_gavl_frame );
		m_blended_gavl_frame = 0;
	}
	if ( m_blended_lazy_frame ) {
		delete m_blended_lazy_frame;
		m_blended_lazy_frame = 0;
	}
}
void Timeline::clear()
{
	TimelineBase::clear();
	m_trackId = 0;
}






int Timeline::write( string filename, string name )
{
	char buffer[512];
	TiXmlDocument doc( filename.c_str() );
	TiXmlDeclaration* dec = new TiXmlDeclaration( "1.0", "", "no" );
	doc.LinkEndChild( dec );

	TiXmlElement* project = new TiXmlElement( "open_movie_editor_project" );
	doc.LinkEndChild( project );

	TiXmlElement* item = new TiXmlElement( "version" );
	project->LinkEndChild( item );
	TiXmlText* text = new TiXmlText( VERSION );
	item->LinkEndChild( text );

	item = new TiXmlElement( "name" );
	project->LinkEndChild( item );
	text = new TiXmlText( name.c_str() );
	item->LinkEndChild( text );

	item = new TiXmlElement( "zoom" );
	item->SetDoubleAttribute( "value", 1.0 );
	project->LinkEndChild( item );

	item = new TiXmlElement( "scroll" );
	item->SetAttribute( "value", 0 );
	project->LinkEndChild( item );

	item = new TiXmlElement( "stylus" );
	item->SetAttribute( "value", 0 );
	project->LinkEndChild( item );

	item = new TiXmlElement( "playback_fps" );
	item->SetAttribute( "value", m_playback_fps );
	project->LinkEndChild( item );

	TiXmlElement* video_tracks = new TiXmlElement( "video_tracks" );
	project->LinkEndChild( video_tracks );
	
	TiXmlElement* audio_tracks = new TiXmlElement( "audio_tracks" );
	project->LinkEndChild( audio_tracks );

	track_node* node = this->getTracks();

	TiXmlElement* track;
	TiXmlElement* clip;
	//TiXmlElement* automation;
	while ( node ) {
		track = new TiXmlElement( "track" );
		track->SetAttribute( "height", node->track->h() );
		track->SetAttribute( "name", node->track->name().c_str() );
		if ( node->track->type() == TRACK_TYPE_VIDEO ) {
			video_tracks->LinkEndChild( track );
		} else if ( node->track->type() == TRACK_TYPE_AUDIO ) {
			audio_tracks->LinkEndChild( track );
		}
		clip_node* cn = node->track->getClips();
		while ( cn ) {
			clip = new TiXmlElement( "clip" );
			track->LinkEndChild( clip );
			clip->SetAttribute( "filename", cn->clip->filename().c_str() );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->position() );
			clip->SetAttribute( "position", buffer );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->length() );
			clip->SetAttribute( "length", buffer );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->trimA() );
			clip->SetAttribute( "trimA", buffer );
			snprintf( buffer, sizeof(buffer), "%lld", cn->clip->trimB() );
			clip->SetAttribute( "trimB", buffer );
			if ( FilterClip* fc = dynamic_cast<FilterClip*>(cn->clip) ) {
				filter_stack* filters;
				for ( filters = fc->getFilters(); filters; filters = filters->next ) {
					TiXmlElement* filter_xml = new TiXmlElement( "filter" );
					clip->LinkEndChild( filter_xml );
					filter_xml->SetAttribute( "name", filters->filter->name() );
					filter_xml->SetAttribute( "identifier", filters->filter->identifier() );
					filters->filter->writeXML( filter_xml );
				}
			}
			if ( VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>(cn->clip) ) {
				/*if ( vc->def() ) {
					clip->SetAttribute( "render", "default" );
				} else if ( vc->crop() ) {
					clip->SetAttribute( "render", "crop" );
				} else if ( vc->fit() ) {
					clip->SetAttribute( "render", "fit" );
				} else if ( vc->stretch() ) {
					clip->SetAttribute( "render", "stretch" );
				}*/
				if ( VideoClip* vc = dynamic_cast<VideoClip*>(cn->clip) ) {
					clip->SetAttribute( "mute", (int)vc->m_mute );
				}
				if ( TitleClip* tc = dynamic_cast<TitleClip*>(cn->clip) ) {
					clip->SetAttribute( "text", tc->text() );
					clip->SetDoubleAttribute( "x", tc->x() );
					clip->SetDoubleAttribute( "y", tc->y() );
					clip->SetAttribute( "size", tc->size() );
					clip->SetAttribute( "font", tc->font() );
					clip->SetAttribute( "color", tc->color() );
				}
			}
			cn = cn->next;
		}
		node = node->next;
	}

	doc.SaveFile();
	return 1;
}
int Timeline::read( string filename )
{
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) {
		return 0;
	}
	this->clear();
	TiXmlHandle docH( &doc );

	const char* versions[] = { "0.0.20061221","0.0.20061219", "0.0.20061128", "0.0.20061121", "0.0.20060901", "0.0.20060630", 0 };
	TiXmlText *name = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "version" ).FirstChild().Text();
	if ( name ) {
		const char* cname = name->Value();
		for ( int i = 0; versions[i]; i++ ) {
			if ( strcmp( versions[i], cname ) == 0 ) {
				cerr << "This Project File Format Version is not supported anymore" << endl;
				return 0;
			}
		}
	}

	TiXmlElement* item = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "playback_fps" ).Element();
	int i = PB_FPS_NONE;
	if ( item ) {
		item->Attribute( "value", &i );
	}
	m_playback_fps = (playback_fps)i;


	TiXmlElement* track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "video_tracks" ).FirstChild( "track" ).Element();
	
	int trackId = 0;
	
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		trackId = this->getTrackId();
		VideoTrack *tr = new VideoTrack( this, trackId );
		this->addTrack( tr );
		const char* name = track->Attribute( "name" );
		if ( name ) {
			tr->name( name );
		}
		int height = 30;
		if ( track->Attribute( "height", &height ) ) {
			tr->h( height );
		}
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int64_t position; //TODO: int64_t problem
			int64_t trimA;
			int64_t trimB;
			int mute = 0;
			int64_t length;
			char filename[1024];
			const char* position_str;
			if ( ! ( position_str = j->Attribute( "position" ) ) )
				continue;
			position = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimA" ) ) )
				continue;
			trimA = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimB" ) ) )
				continue;
			trimB = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "length" ) ) )
				continue;
			length = atoll( position_str );
			
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename ) //TODO is this correct?
				continue;
			j->Attribute( "mute", &mute );
			VideoEffectClip* vec = 0;
			const char* ext = fl_filename_ext( filename );
			if ( strcmp( filename, "TitleClip" ) == 0 ) {
				TitleClip* c = new TitleClip( tr, position, length - trimA - trimB, -1 );
				vec = c;
				const char* textp;
				double x;
				double y;
				int size;
				int font;
				int color;
				if ( ( textp = j->Attribute( "text" ) ) ) {
					c->text( textp );
				}
				if ( j->Attribute( "x", &x ) ) {
					c->x( x );
				}
				if ( j->Attribute( "y", &y ) ) {
					c->y( y );
				}
				if ( j->Attribute( "size", &size ) ) {
					c->size( size );
				}
				if ( j->Attribute( "font", &font ) ) {
					c->font( font );
				}
				if ( j->Attribute( "color", &color ) ) {
					c->color( (Fl_Color)color );
				}
				TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
				for( ; effectXml; effectXml = effectXml->NextSiblingElement( "filter" ) ) {
					FilterFactory* ef = g_mainFilterFactory->get( effectXml->Attribute( "identifier" ) );
					if ( ef ) {
						FilterBase* effectObj = c->appendFilter( ef );
						effectObj->readXML( effectXml );
						
					}
				}
				this->addClip( trackId, c );
			} else if ( strcmp( ext, ".svg" ) == 0 ||strcmp( ext, ".SVG" ) == 0 ) {
				InkscapeClip* c = new InkscapeClip( tr, position, filename, length - trimA - trimB, -1 );
				TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
				for( ; effectXml; effectXml = effectXml->NextSiblingElement( "filter" ) ) {
					FilterFactory* ef = g_mainFilterFactory->get( effectXml->Attribute( "identifier" ) );
					if ( ef ) {
						FilterBase* effectObj = c->appendFilter( ef );
						effectObj->readXML( effectXml );
						
					}
				}
				this->addClip( trackId, c );
			} else {
				IVideoFile* vf = VideoFileFactory::get( filename );
				if ( vf ) {
					VideoClip* c = new VideoClip( tr, position, vf, trimA, trimB, -1 );
					vec = c;
					c->m_mute = mute;
					TiXmlElement* filterXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
					for ( ; filterXml; filterXml = filterXml->NextSiblingElement( "filter" ) ) {
						FilterFactory* ff = g_mainFilterFactory->get( filterXml->Attribute( "identifier" ) );//g_audioVolumeFilterFactory;
						if ( ff ) {
							FilterBase* filter = c->appendFilter( ff );
							filter->readXML( filterXml );
						}
					}
					this->addClip( trackId, c );
				} else {
					ImageClip* ic = new ImageClip( tr, position, filename, length - trimA - trimB, -1 );
					if ( !ic->ok() ) {
						delete ic;
						if ( length > 0 ) {
							Clip* c = new DummyClip( tr, filename, position, length+trimA+trimB, trimA, trimB );
							this->addClip( trackId, c );
						}
					} else {
						vec = ic;
						//TODO: This is copy and paste
						TiXmlElement* effectXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
						for( ; effectXml; effectXml = effectXml->NextSiblingElement( "filter" ) ) {
							FilterFactory* ef = g_mainFilterFactory->get( effectXml->Attribute( "identifier" ) );
							if ( ef ) {
								FilterBase* effectObj = ic->appendFilter( ef );
								effectObj->readXML( effectXml );
							}
						}

						this->addClip( trackId, ic );
					}
				}
			}
			/*if ( vec ) {
				const char* render;
				render = j->Attribute( "render" );
				if ( !render ) {
				} else if ( strcmp( render, "default" ) == 0 ) {
					vec->def( true );
				} else if ( strcmp( render, "crop" ) == 0 ) {
					vec->crop( true );
				} else if ( strcmp( render, "fit" ) == 0 ) {
					vec->fit( true );
				} else if ( strcmp( render, "stretch" ) == 0 ) {
					vec->stretch( true );
				}
			}*/

		}
		tr->reconsiderFadeOver();
	}
	track = docH.FirstChild( "open_movie_editor_project" ).FirstChild( "audio_tracks" ).FirstChild( "track" ).Element();
	for ( ; track; track = track->NextSiblingElement( "track" ) ) {
		trackId = this->getTrackId();
		Track *tr = new AudioTrack( this, trackId );
		this->addTrack( tr );
		const char* name = track->Attribute( "name" );
		if ( name ) {
			tr->name( name );
		}
		int height = 30;
		if ( track->Attribute( "height", &height ) ) {
			tr->h( height );
		}
		
		TiXmlElement* j = TiXmlHandle( track ).FirstChildElement( "clip" ).Element();
		for ( ; j; j = j->NextSiblingElement( "clip" ) ) {
			int64_t position;
			int64_t trimA;
			int64_t trimB;
			int64_t length;
			char filename[1024];
			const char* position_str;
			if ( ! ( position_str = j->Attribute( "position" ) ) )
				continue;
			position = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimA" ) ) )
				continue;
			trimA = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "trimB" ) ) )
				continue;
			trimB = atoll( position_str );
			if ( ! ( position_str = j->Attribute( "length" ) ) )
				continue;
			length = atoll( position_str );

			
			
			strlcpy( filename, j->Attribute( "filename" ), sizeof(filename) );
			if ( ! filename )
				continue;
			IAudioFile *af = AudioFileFactory::get( filename );
			if ( !af ) {
				Track *t = this->getTrack( trackId );
				Clip* clip = new DummyClip( t, filename, position, length+trimA+trimB, trimA, trimB );
				this->addClip( trackId, clip );
				continue;
			}
			AudioClip* ac = new AudioClip( tr, position, af, trimA, trimB );
			Clip* clip = ac;

			TiXmlElement* filterXml = TiXmlHandle( j ).FirstChildElement( "filter" ).Element();
			for ( ; filterXml; filterXml = filterXml->NextSiblingElement( "filter" ) ) {
				FilterFactory* ff = g_mainFilterFactory->get( filterXml->Attribute( "identifier" ) );
				if ( ff ) {
					FilterBase* filter = ac->appendFilter( ff );
					filter->readXML( filterXml );
				}
			}
			this->addClip( trackId, clip );
		}
	}

	// TODO: vv--- Remove this, doesn't belong here
	g_timelineView->redraw();
	g_timelineView->adjustScrollbar();
	return 1;
}
int Timeline::write_smil( std::string filename, int track )
{
	sort();
	TiXmlDocument doc( filename.c_str() );
	TiXmlDeclaration* dec = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( dec );
	TiXmlElement* smil = new TiXmlElement( "smil" );
	smil->SetAttribute( "xmlns", "http://www.w3.org/2001/SMIL20/Language" );
	doc.LinkEndChild( smil );

	TiXmlElement* body = new TiXmlElement( "body" );
	smil->LinkEndChild( body );

	track_node* node = this->getTracks();
	int i = 0;

	for ( ; i < track; node = node->next ) {
		i++;
	}
	clip_node* cn = node->track->getClips();
	TiXmlElement* seq;
	TiXmlElement* video;
	while ( cn ) {
		if ( cn->clip->type() == CLIP_TYPE_VIDEO ) {
			seq = new TiXmlElement( "seq" );
			body->LinkEndChild( seq );
			video = new TiXmlElement( "video" );
			seq->LinkEndChild( video );
			video->SetAttribute( "src", cn->clip->filename().c_str() );
			int64_t clipBegin = cn->clip->trimA();
			int64_t clipEnd = cn->clip->fileLength() - cn->clip->trimB();
			video->SetAttribute( "clipBegin", timestamp_to_smil_string( clipBegin, 40 ) ); // FIXME: 25fps Hardcoded
			video->SetAttribute( "clipEnd", timestamp_to_smil_string( clipEnd, 40 ) ); // FIXME: 25fps Hardcoded
		}
		cn = cn->next;
	}
	doc.SaveFile();
	
	return 1;
}

int Timeline::write_srt( std::string filename, int track )
{
	sort();

	std::ofstream srt_file;
	srt_file.open( filename.c_str() );

	track_node* node = this->getTracks();
	int i = 0;
	for ( ; i < track; node = node->next ) {
		i++;
	}
	clip_node* cn = node->track->getClips();
	TitleClip* title_clip;
	i = 1;
	while ( cn ) {
		title_clip = dynamic_cast<TitleClip*>(cn->clip);
		if ( title_clip ) {
			int64_t textBegin = title_clip->A();
			int64_t textEnd = title_clip->B();
			string text = title_clip->text();
			srt_file << i << std::endl;
			srt_file << timestamp_to_smil_string( textBegin );
			srt_file << " --> ";
			srt_file << timestamp_to_smil_string( textBegin ) << std::endl;
			srt_file << text << std::endl << std::endl;
		}
		cn = cn->next;
		i++;
	}
	srt_file.close();
	return 1;
}



} /* namespace nle */
