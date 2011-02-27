/* JackPlaybackCore.cxx
 *
 *  Copyright (C) 2005-2008 Richard Spindler <richard.spindler AT gmail.com>
 * 
 *  patch to support jackit.sf.net audio out and transport sync
 *   05/2006 Robin Gareus <robin AT gareus.org>
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

#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include <jack/jack.h>
#include <jack/transport.h>
#include <math.h>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include "globals.H"
#include "JackPlaybackCore.H"
#include "IAudioReader.H"
#include "IVideoReader.H"
#include "IVideoWriter.H"
#include "Timeline.H"
#include "ErrorDialog/IErrorHandler.H"
#include "AudioThreadedRingbuffer.H"
#include <iostream>
#include "fps_helper.H"

#include <cstdlib>
#include <cstring>

#define VIDEO_DRIFT_LIMIT 2 //Calculate this based on frame size
#define FRAMES 4096

namespace nle
{


/*
 * static functions
 */

static void video_idle_callback( void* data )
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->flipFrame();
}


/*
 * private jack 
 */

static jack_client_t *jack_client = 0;
static jack_port_t *output_port[2]; // stereo
static jack_nframes_t jack_bufsiz = 64;
static jack_nframes_t jack_latency_comp = 0;

/*
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 */
int jack_callback (jack_nframes_t nframes, void *data)
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->jackreadAudio( nframes );
	return(0);
}

static int jack_sync_callback( jack_transport_state_t state, jack_position_t *pos, void *data )
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	return pc->sync( state, pos );
}


/* when jack shuts down... */
void jack_shutdown( void *data )
{
	jack_client = 0;
	std::cerr << "jack server shutdown." << std::endl;
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->hardstop(); 
}

void close_jack(void)
{
	if ( jack_client ) {
		jack_client_close( jack_client );
	}
	jack_client = 0;
}

int64_t jack_poll_frame( void )
{
	jack_position_t	jack_position;
	double		jack_time;
	int64_t		frame;

	if ( !jack_client ) return ( -1 );
	/* Calculate frame. */
	jack_transport_query( jack_client, &jack_position );
	jack_time = jack_position.frame / (double) jack_position.frame_rate;
	frame = (int64_t)llrint( NLE_TIME_BASE * jack_time );
	return ( frame );
}

void jack_reposition( int64_t vframe )
{
	jack_nframes_t frame = (jack_nframes_t)( vframe * 48000 / NLE_TIME_BASE );
	if ( jack_client ) {
		jack_transport_locate( jack_client, frame );
	}
}

void play_pipe_callback( int fd, void* data )
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->play_ping( fd );
}

void stop_pipe_callback( int fd, void* data )
{
	JackPlaybackCore* pc = (JackPlaybackCore*)data;
	pc->stop_ping( fd );
}
void JackPlaybackCore::play_ping( int fd )
{
	unsigned char x;
	read( fd, &x, 1 );
	if ( m_active ) {
		return;
	}
	m_active = true;
	Fl::add_timeout( seconds_frame_length( g_timeline->m_playback_fps ) /*0.04*/, video_idle_callback, this );
	g_playButton->label( "@square" );
	g_firstButton->deactivate();
	g_lastButton->deactivate();
	g_forwardButton->deactivate();
	g_backButton->deactivate();
}
void JackPlaybackCore::stop_ping( int fd )
{
	unsigned char x;
	read( fd, &x, 1 );
	if ( !m_active ) {
		return;
	}
	m_active = false;
	g_playButton->label( "@>" );
	g_firstButton->activate();
	g_lastButton->activate();
	g_forwardButton->activate();
	g_backButton->activate();
}

JackPlaybackCore::JackPlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter )
	: m_audioReader(audioReader), m_videoReader(videoReader), m_videoWriter(videoWriter)
{
	g_playbackCore = this;
	m_active = false;
	m_threadedRingbuffer = 0;

	if ( pipe(m_play_pipe) == -1 || pipe(m_stop_pipe) == -1 ) {
		cerr << "could not create pipes." << endl;
		return;
	}
	Fl::add_fd( m_play_pipe[0], play_pipe_callback, this );
	Fl::add_fd( m_stop_pipe[0], stop_pipe_callback, this );
	fcntl( m_play_pipe[1], F_SETFL, O_NONBLOCK );
	fcntl( m_stop_pipe[1], F_SETFL, O_NONBLOCK );

	jack_client = jack_client_open( "openmovieeditor", JackNullOption, 0 );

	if (!jack_client) {
		cerr << "could not connect to jack." << endl;
		return;
	}	

	jack_on_shutdown (jack_client, jack_shutdown, this);
	jack_set_process_callback(jack_client,jack_callback,this);

	output_port[0] = jack_port_register (jack_client, "output-L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	output_port[1] = jack_port_register (jack_client, "output-R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (!output_port[0] || !output_port[1]) {
		cerr << "no more jack ports availabe." << endl;
		close_jack();
		return;
	}
	jack_bufsiz= jack_get_buffer_size(jack_client);
	cout << "Jack Samplerate: " << jack_get_sample_rate(jack_client) << endl;
	if ( jack_get_sample_rate(jack_client) != 48000 ) {
		CLEAR_ERRORS();
		ERROR_DETAIL( "Jack is running with a samplerate other than 48000" );
		SHOW_ERROR( "Jack Soundoutput failed" );
		close_jack();
		return;
	}

	if (jack_bufsiz > FRAMES) { 
		CLEAR_ERRORS();
		ERROR_DETAIL( "please decrease jackd buffer size to 4096" );
		SHOW_ERROR( "Jack Soundoutput failed" );
		close_jack();
		return;
	}

	

	m_threadedRingbuffer = new AudioThreadedRingbuffer( m_audioReader );
	jack_set_sync_callback( jack_client, jack_sync_callback, this );
	jack_activate( jack_client );

	// TODO: skip auto-connect if user wishes so.
	char * port_name = 0; // TODO: get from preferences 
	int port_flags = JackPortIsInput;
	if (!port_name) port_flags |= JackPortIsPhysical;

	const char **found_ports = jack_get_ports( jack_client, port_name, 0, port_flags );
	for ( int i = 0; found_ports && found_ports[i] && i < 2; i++ ) {
		if ( jack_connect( jack_client, jack_port_name( output_port[i]), found_ports[i] ) ) {
			cerr << "can not connect to jack output" << endl;
		}
	}
}

bool JackPlaybackCore::ok() { return jack_client; }

/* hardstop() use only for error abort - else use stop */
void JackPlaybackCore::hardstop()
{
	m_active = false;
} 

JackPlaybackCore::~JackPlaybackCore()
{
	if ( jack_client ) {
		jack_client_close( jack_client );
		jack_client = 0;
	}
	if ( m_threadedRingbuffer ) {
		delete m_threadedRingbuffer;
		m_threadedRingbuffer = 0;
	}
	Fl::remove_fd( m_play_pipe[0] );
	Fl::remove_fd( m_stop_pipe[0] );
	close( m_play_pipe[0] );
	close( m_play_pipe[1] );
	close( m_stop_pipe[0] );
	close( m_stop_pipe[1] );
}


void JackPlaybackCore::play()
{
	if ( m_active ) return; 

	if ( !jack_client ) return;

	m_currentFrame = g_timeline->m_seekPosition;

	jack_reposition( m_currentFrame );

	jack_transport_start( jack_client );

	m_active = true;
	Fl::add_timeout( seconds_frame_length( g_timeline->m_playback_fps ) /*0.04*/, video_idle_callback, this ); // looks like hardcoded 25fps 
}

void JackPlaybackCore::stop()
{
	if ( !m_active ) {
		return;
	}
	m_active = false;
	if ( jack_client ) {
		jack_transport_stop( jack_client );
	}
}
int JackPlaybackCore::sync( jack_transport_state_t state, jack_position_t *pos )
{
	switch( state ) {
		case JackTransportStarting:
			if ( m_threadedRingbuffer->ready() ) {
				m_threadedRingbuffer->seek( pos->frame );
			}
		case JackTransportRolling: // FALLTHROUGH
		case JackTransportStopped:
			return m_threadedRingbuffer->ready();
	}
	return 0;
}

void JackPlaybackCore::jackreadAudio( jack_nframes_t nframes )
{
	static jack_transport_state_t last_jack_state = JackTransportStopped;
	void *outL, *outR;
	jack_position_t	jack_position;
	jack_transport_state_t ts;
	jack_nframes_t latencyL , latencyR;

	latencyL = jack_port_get_total_latency(jack_client,output_port[0]);
	latencyR = jack_port_get_total_latency(jack_client,output_port[1]);
	jack_latency_comp = max(latencyL,latencyR);

	outL = jack_port_get_buffer (output_port[0], nframes);
	outR = jack_port_get_buffer (output_port[1], nframes);
	ts = jack_transport_query( jack_client, &jack_position );
	
	if ( last_jack_state != ts ) {
		unsigned char x = 0x0;
		switch( ts ) {
			case JackTransportRolling:
				write( m_play_pipe[1], &x, 1 );
				/* check for EAGAIN ? */
				break;
			case JackTransportStopped:
				write( m_stop_pipe[1], &x, 1 );
				/* check for EAGAIN ? */
				break;
		}
		last_jack_state = ts;
	}
	switch( ts ) {
		case JackTransportStarting:
		case JackTransportStopped:
			memset(outR,0, sizeof (jack_default_audio_sample_t) * nframes);
			memset(outL,0, sizeof (jack_default_audio_sample_t) * nframes);
			return;
		case JackTransportRolling:
			break;
	}

	if ( !m_active )  {
		memset(outR,0, sizeof (jack_default_audio_sample_t) * nframes);
		memset(outL,0, sizeof (jack_default_audio_sample_t) * nframes);
		return;
	}
	
	float stereobuf[(2*FRAMES)];  // combined stereo signals in one buffer from OME
	unsigned long rv = m_threadedRingbuffer->fillBuffer(stereobuf, nframes);

	// demux stereo buffer into the jack mono buffers.
	jack_default_audio_sample_t *jackbufL, *jackbufR;
	jackbufL= (jack_default_audio_sample_t*)outL;
	jackbufR= (jack_default_audio_sample_t*)outR;
	
	for (unsigned int i=0;i<nframes && i< rv;i++) {
		jackbufL[i]= stereobuf[2*i];
		jackbufR[i]= stereobuf[2*i+1];
	}
}

void JackPlaybackCore::flipFrame()
{
	if ( !m_active ) {
		return;
	}
	if ( jack_client ) {
		m_currentFrame = jack_poll_frame();
	}
	static LazyFrame** fs = 0;
	if ( fs ) {
		m_videoWriter->pushFrameStack( fs );
	}
	fs = m_videoReader->getFrameStack( m_currentFrame );
	Fl::repeat_timeout( seconds_frame_length( g_timeline->m_playback_fps )/*0.04*/, video_idle_callback, this ); // more hardcoded framerates.
}


} /* namespace nle */
