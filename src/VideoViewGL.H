/*  VideoViewGL.H
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

#ifndef _VIDEO_VIEW_GL_H_
#define _VIDEO_VIEW_GL_H_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include "IVideoWriter.H"
#include <sstream>

namespace nle
{

class Timeline;
class LazyFrame;

class VideoViewGL : public Fl_Gl_Window, public IVideoWriter
{
	public:
		VideoViewGL( int x, int y, int w, int h, const char *l = 0 );
		~VideoViewGL();
		void draw();
		void pushFrameStack( LazyFrame** fs, bool move_cursor = true );
		void pushFrame( LazyFrame* fs, bool move_cursor = true );
		void nextFrame( int64_t frame );
		void seek( int64_t position );
		void play();
		void stop();
		void zoom( float zoom ) { m_zoom = zoom; }
		std::stringstream opengl_system_information;
	private:
		void not_valid();
		void drawFrameStack( LazyFrame** fs );
		int64_t m_seekPosition;
		void drawVideoBorder();
		float m_zoom;
		bool m_once;
};

extern VideoViewGL* g_videoView;

} /* namespace nle */


#endif /* _VIDEO_VIEW_GL_H_ */
