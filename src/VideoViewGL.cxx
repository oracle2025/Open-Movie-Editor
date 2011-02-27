/*  VideoViewGL.cxx
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

#include <FL/gl.h>
#include <FL/Fl_PNG_Image.H>

#include "VideoViewGL.H"
#include "globals.H"
#include "SwitchBoard.H"
#include "IPlaybackCore.H"
#include "Timeline.H"
#include "render_helper.H"
#include "int_float_scale.H"
#include "LazyFrame.H"



extern bool g_16_9;
extern bool g_black_borders;
extern bool g_black_borders_2_35;

using namespace std;

namespace nle
{


struct texture_frame_cache {
	void* p;
	int dirty;
};

static struct texture_frame_cache tcache[10];
	
VideoViewGL* g_videoView = 0;

void reset_cache()
{
	for ( int i = 0; i < 10; i++ ) {
		tcache[i].p = 0;
		tcache[i].dirty = 0;
	}
}

VideoViewGL::VideoViewGL( int x, int y, int w, int h, const char *l )
	: Fl_Gl_Window( x, y, w, h, l )
{
	g_videoView = this;
	m_zoom = 0.0;
	m_seekPosition = 0;
	reset_cache();
	m_once = true;
}

VideoViewGL::~VideoViewGL()
{
}

static GLuint video_canvas[10];
static int T_W = 2048;
static int T_H = 2048;
static float TEXTURE_WIDTH = 2048.0;
static float TEXTURE_HEIGHT = 2048.0;
//static unsigned char pulldown_frame[3 * 1024 * 1024];

void VideoViewGL::drawVideoBorder()
{
	//int w_ = 1024;
	int w_ = g_16_9 ? 1024 : 768;
	int h_ = 576;
	float gl_x, gl_y, gl_w, gl_h;
	{
		float f_v = ( (float)w_ / (float)h_ );
		float f_w = ( (float)w() / (float)h() );
		float f_g = f_v / f_w;
		if ( f_g > 1.0 ) {
			gl_h = 10.0 / f_g;
			gl_w = 10.0;
		} else {
			gl_h = 10.0;
			gl_w = f_g * 10.0;
		}
		gl_x = ( 10.0 - gl_w ) / 2;
		gl_y = ( 10.0 - gl_h ) / 2;

	}

	glDisable (GL_TEXTURE_2D);
	if ( (g_black_borders||g_black_borders_2_35) && !g_16_9 ) {
		int w_ = 768;
		//int w_ = 1024;
		int h_ = 576;
		float gl_x, gl_y, gl_w, gl_h;
		{
			float f_v = ( (float)w_ / (float)h_ );
			float f_w = ( (float)w() / (float)h() );
			float f_g = f_v / f_w;
			if ( f_g > 1.0 ) {
				gl_h = 10.0 / f_g;
				gl_w = 10.0;
			} else {
				gl_h = 10.0;
				gl_w = f_g * 10.0;
			}
			if ( g_black_borders_2_35 ) {
				gl_h = gl_h / ( ( 2.35 / 1.0 ) / ( 4.0 / 3.0 ) );
			} else {
				gl_h = gl_h / ( ( 16.0 / 9.0 ) / ( 4.0 / 3.0 ) );
			}
			gl_x = ( 10.0 - gl_w ) / 2;
			gl_y = ( 10.0 - gl_h ) / 2;

		}
		
		glBegin (GL_QUADS);

		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );

		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( gl_x + gl_w, 0.0f, 0.0f );
		glVertex3f( gl_x + gl_w, gl_y, 0.0f );
		glVertex3f( 0.0f, gl_y, 0.0f );
		
		glVertex3f( 0.0f, gl_y + gl_h, 0.0f );
		glVertex3f( gl_x + gl_w, gl_y + gl_h, 0.0f );
		glVertex3f( gl_x + gl_w, 10.0f, 0.0f );
		glVertex3f( 0.0f, 10.0f, 0.0f );


		glEnd();
	}

	glLineWidth(3);
	glBegin (GL_LINE_LOOP);
	glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
	
	glVertex3f   (  gl_x,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
	glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );

	glEnd();
	glLineWidth(1);
	glBegin (GL_LINE_LOOP);
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glVertex3f   (  gl_x,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
	glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
	glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );

	glEnd();

	
	glEnable (GL_TEXTURE_2D);
	
/*
	float _x, _y, _w, _h;
	_x = gl_x + (gl_w / 8.0);
	_y = gl_y;
	_w = gl_w / 1.33333333333333333;
	_h = gl_h;
	glDisable (GL_TEXTURE_2D);
	glLineWidth(3);
	glBegin (GL_LINE_LOOP);
	glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
	
	glVertex3f   (  _x,      _y, 0.0 );
	glVertex3f   ( _x + _w,      _y, 0.0 );
	glVertex3f   ( _x + _w,     _y + _h, 0.0 );
	glVertex3f   (  _x,     _y + _h, 0.0 );

	glEnd();
	glLineWidth(1);
	glBegin (GL_LINE_LOOP);
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glVertex3f   (  _x,      _y, 0.0 );
	glVertex3f   ( _x + _w,      _y, 0.0 );
	glVertex3f   ( _x + _w,     _y + _h, 0.0 );
	glVertex3f   (  _x,     _y + _h, 0.0 );

	glEnd();
	glEnable (GL_TEXTURE_2D);
	*/
}

void VideoViewGL::pushFrameStack( LazyFrame** fs, bool move_cursor )
{
 if ( move_cursor ) {MoveCursor(); }
 // TODO: Cursor not Moving when playback
 // When this redraws the TimelineView,
 // it causes redraws of the opengl window, which interferes with the playback,
 // because then swap_buffers is called after draw, and then pushFrameStack
 // _always_ draws into the second buffer which cases _severe_ screenflicker,
 // BAH 
	
	make_current();
	if ( !valid() ) {
		not_valid();
	}

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glBindTexture( GL_TEXTURE_2D, video_canvas[0] );

	if ( !fs[0] ) {
		static unsigned char p[3 * 2048 * 2048] = { 0 };
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, T_W, T_H, GL_RGB, GL_UNSIGNED_BYTE, p );
		swap_buffers();
		return;
	}

	drawFrameStack(fs);

	swap_buffers();

}

void VideoViewGL::pushFrame( LazyFrame* fs, bool move_cursor )
{
	LazyFrame* fstack[2] = {0};
	fstack[0] = fs;
	pushFrameStack( fstack, move_cursor );
}
void VideoViewGL::not_valid()
{
	float glo1, glo2;
	if ( m_zoom < 0.0 ) {
		glo1 = 10.0 * m_zoom;
		glo2 = 10.0 - 10.0 * m_zoom;
	} else {
		glo1 = 0.0 + m_zoom * 5.0; // -> 5.0
		glo2 = 10.0 - m_zoom * 5.0; // -> 5.0
	}
	glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
	glOrtho( glo1, glo2, glo2, glo1, -20000, 10000 ); glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable (GL_TEXTURE_2D);
}
/*
 *  GL_NVX_ycrcb
 */
void VideoViewGL::draw()
{
	if (g_backseek) { reset_cache(); g_backseek=false; }
	static unsigned char p[3 * 2048 * 2048] = { 0 };
	if (m_once) {
		GLint max[2];
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, max);
		glGenTextures( 10, video_canvas );
		if ( max[0] < T_W ) {
			T_W = max[0];
			T_H = max[0];
			TEXTURE_WIDTH = max[0];
			TEXTURE_HEIGHT = max[0];
		}
		for ( int i = 0; i < 10; i++ ) {
			glBindTexture (GL_TEXTURE_2D, video_canvas[i] );
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, T_W, T_H, 0, GL_RGB, GL_UNSIGNED_BYTE, p);
		}
		opengl_system_information.str( "" );
		opengl_system_information << "OpenGL vendor string: " << ((const char*)glGetString(GL_VENDOR)) << endl;
		opengl_system_information << "OpenGL renderer string: " << ((const char*)glGetString(GL_RENDERER)) << endl;
		opengl_system_information << "OpenGL version string: " << ((const char*)glGetString(GL_VERSION)) << endl;
		opengl_system_information << "GL_MAX_TEXTURE_SIZE = " << max[0] << endl;
		m_once = false;
	}
	if ( g_playbackCore->active() ) { return; }
	if ( !valid() ) {
		not_valid();
	}
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindTexture( GL_TEXTURE_2D, video_canvas[0] );
	if ( m_seekPosition >= 0 ) {
		g_SEEKING = true;
		LazyFrame** fs = g_timeline->getFrameStack( m_seekPosition );
		g_SEEKING = false;
		if ( !fs[0] ) {
			g_PREVENT_OFFSCREEN_CRASH = false;
			return;
		}


		drawFrameStack( fs );
	}
	g_PREVENT_OFFSCREEN_CRASH = false;
	drawVideoBorder();
}

void VideoViewGL::seek( int64_t position )
{
	if ( g_playbackCore->active() ) {
		return;
	}
	m_seekPosition = position;
	redraw();
}

void VideoViewGL::play()
{
	if ( g_playbackCore->active() ) {
		return;
	}
	reset_cache();
	m_seekPosition = -1;
	g_timeline->sort();
	g_playbackCore->play();
}

void VideoViewGL::stop()
{
	g_playbackCore->stop();
}

void VideoViewGL::drawFrameStack( LazyFrame** fs )
{
	int count = 0;	
	for ( int i = 1; fs[i]; i++ ) {
		count++;
	}
	//TODO: i < 10
	for ( int i = count; i>=0; i-- ) {
		glBindTexture( GL_TEXTURE_2D, video_canvas[i] );
		if ( fs[i]->cacheable() ) {
			if ( tcache[i].p == fs[i] && !tcache[i].dirty && !fs[i]->dirty() ) {
				continue;
			} else {
				tcache[i].p = fs[i];
				tcache[i].dirty = false;
				fs[i]->dirty( false );
			}
		} else {
			tcache[i].dirty = true;
		}
		if ( fs[i]->format()->pixelformat == GAVL_RGB_24 ) {
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->format()->frame_width, fs[i]->format()->frame_height, GL_RGB, GL_UNSIGNED_BYTE, fs[i]->native()->planes[0] );
		} else {
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, fs[i]->format()->frame_width, fs[i]->format()->frame_height, GL_RGBA, GL_UNSIGNED_BYTE, fs[i]->RGBA()->planes[0] );
		}
	}
	for ( int i = count; i>=0; i-- ) {
		glBindTexture( GL_TEXTURE_2D, video_canvas[i] );
		float gl_x, gl_y, gl_w, gl_h;
		{
			float f_v;// = ( (float)fs[i]->w / (float)fs[i]->h );
			if ( g_16_9 ) {
				f_v = ( 16.0 / 9.0 );
			} else {
				f_v = ( 4.0 / 3.0 );
			}
			float f_w = ( (float)w() / (float)h() );
			float f_g = f_v / f_w;
			if ( f_g > 1.0 ) {
				gl_h = 10.0 / f_g;
				gl_w = 10.0;
			} else {
				gl_h = 10.0;
				gl_w = f_g * 10.0;
			}
			gl_x = ( 10.0 - gl_w ) / 2;
			gl_y = ( 10.0 - gl_h ) / 2;

		}
	
		float ww = ( fs[i]->format()->frame_width ) / TEXTURE_WIDTH;
		float xx = 0;
		float hh = fs[i]->format()->frame_height / TEXTURE_HEIGHT;
		glColor4f( 1.0f, 1.0f, 1.0f, fs[i]->alpha() ); //Control Transparency
		glBegin (GL_QUADS);
			glTexCoord2f (  xx,      0.0 );
			glVertex3f   (  gl_x,      gl_y, 0.0 );
			glTexCoord2f (  xx + ww,  0.0 );  // (fs->w / 512.0)
			glVertex3f   ( gl_x + gl_w,      gl_y, 0.0 );
			glTexCoord2f (  xx + ww,  hh ); // (368.0 / 512.0) (240.0 / 512.0)
			glVertex3f   ( gl_x + gl_w,     gl_y + gl_h, 0.0 );
			glTexCoord2f (  xx,      hh ); // (fs->h / 512.0)
			glVertex3f   (  gl_x,     gl_y + gl_h, 0.0 );
		glEnd ();
	}
	drawVideoBorder();
}


} /* namespace nle */
