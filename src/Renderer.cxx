/*  Renderer.cxx
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

#include <cstring>
#include <cassert>

#include <colormodels.h>

#include "strlcpy.h"
#include "sl/sl.h"

#include "Renderer.H"
#include "globals.H"
#include "Timeline.H"
#include "ProgressDialog/IProgressListener.H"
#include "render_helper.H"
#include "LazyFrame.H"

namespace nle
{

//static quicktime_t *qt;


Renderer::Renderer( IVideoFileWriter* writer )
{
	char buffer[1024];
	m_writer = writer;
	p_timeline = 0;
	Timeline* x = g_timeline;
	p_timeline = new Timeline();
	g_timeline = x;

	p_timeline->render_mode( true );
	string temp_filename;
	strcpy( buffer,"OME_RENDER_XXXXXX" );
	temp_filename = string(g_homefolder);
	
	temp_filename += ("/.openme/temp" PREF_FILE_ADD);
	temp_filename += buffer;
	g_timeline->write( temp_filename, "" );

	p_timeline->read( temp_filename );

	m_w = m_writer->format()->w;
	m_h = m_writer->format()->h;
	m_samplerate = m_writer->samplerate();
	m_fps.frame_duration = m_writer->format()->framerate.frame_duration;
	m_fps.timescale = m_writer->format()->framerate.timescale;
	m_fps.audio_frames_per_chunk = m_writer->format()->framerate.audio_frames_per_chunk;
	m_fps.video_frames_per_chunk = m_writer->format()->framerate.video_frames_per_chunk;

	gavl_video_format_t format;
	format.frame_width  = m_w;
	format.frame_height = m_h;
	format.image_width  = m_w;
	format.image_height = m_h;
	format.pixel_width = 1;
	format.pixel_height = 1;
	format.pixelformat = GAVL_RGBA_32;
	format.interlace_mode = GAVL_INTERLACE_NONE;

		
	p_timeline->prepareFormat( &format ); 
	
	return;
}
bool Renderer::ok() {
	return true;
//	return qt;
}
Renderer::~Renderer()
{
	/*if (qt)
		quicktime_close( qt );*/
	p_timeline->unPrepareFormat();
	if ( p_timeline ) {
		Timeline* x = g_timeline;
		delete p_timeline;
		g_timeline = x;
	}
	delete m_writer;
}

//#define AUDIO_BUFFER_SIZE 480
//#define AUDIO_BUFFER_SIZE 23040
#define AUDIO_BUFFER_SIZE 32000
void Renderer::go( IProgressListener* l )
{
	
/*	if ( !qt ) {
		return;
	}*/
	if ( l ) {
		l->start();
	}
	//TODO:  Make a copy of the timeline?
	//Use Write project to tmp file
	

	
	p_timeline->seek( 0 );
	p_timeline->sort();
	
	int res;
	float abuffer[AUDIO_BUFFER_SIZE*2];

	int64_t length = (int64_t)( p_timeline->length() * ( (float)m_fps.timescale / (float)m_fps.frame_duration  ) );
	int64_t current_frame = 0;


/*
 * 12 * 1920 = 23040
 * This is the chunk of audio that will be written between 12 Frames
 */
	LazyFrame* enc_frame;
	int64_t position = 0;
	int64_t frame_length = 35280000 / m_fps.timescale * m_fps.frame_duration;
	bool run = true;
	int frames_to_write;
	assert( m_fps.audio_frames_per_chunk <= AUDIO_BUFFER_SIZE );
	do {
		res = p_timeline->fillBuffer( abuffer, m_fps.audio_frames_per_chunk );
		p_timeline->sampleseek( 0, m_fps.audio_frames_per_chunk );
		m_writer->encodeAudioFrame( abuffer, res );
		for ( int i = 0; i < m_fps.video_frames_per_chunk; i++ ) {
			enc_frame = p_timeline->getBlendedFrame( position );
			position += frame_length;
			m_writer->encodeVideoFrame( enc_frame );
			current_frame++;
			if ( l ) {
				if ( l->progress( (double)current_frame / length ) ) {
					run = false;
					break;
				}
			}
		}
	} while ( res == m_fps.audio_frames_per_chunk && run );
	if ( l ) {
		l->end();
	}
}


} /* namespace nle */
