/*  VideoFileProject.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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


#include "VideoFileProject.H"
#include "AudioFileProject.H"
#include "fps_definitions.H"

namespace nle
{

VideoFileProject::VideoFileProject( std::string filename )
{
	Timeline* x = g_timeline;
	m_timeline = new Timeline;
	g_timeline = x;
	m_filename = filename;
	m_ok = false;
	m_timeline->read( filename );
	m_timeline->render_mode( true );

	gavl_video_format_t format;
	format.frame_width  = 720;
	format.frame_height = 576;
	format.image_width  = 720;
	format.image_height = 576;
	format.pixel_width = 1;
	format.pixel_height = 1;
	format.pixelformat = GAVL_RGBA_32;
	format.interlace_mode = GAVL_INTERLACE_NONE;

	m_timeline->prepareFormat( &format );
	
	m_ok = true;
}
VideoFileProject::~VideoFileProject()
{
	Timeline* x = g_timeline;
	delete m_timeline;
	g_timeline = x;
}
bool VideoFileProject::ok()
{
	return m_ok;
}
int64_t VideoFileProject::length()
{
	return m_timeline->length() * NLE_TIME_BASE;
}
LazyFrame* VideoFileProject::read()
{
	return m_timeline->getBlendedFrame();
}
void VideoFileProject::seek( int64_t position )
{
	m_timeline->seek( position );
}
int64_t VideoFileProject::ticksPerFrame()
{
	return ( 1200 * (int64_t)NLE_TIME_BASE ) / 30000;
}
void VideoFileProject::seekToFrame( int64_t frame )
{
	seek( frame * ticksPerFrame() );
}
IAudioFile* VideoFileProject::getAudioReader()
{
	return new AudioFileProject( m_timeline, m_filename );
}

} /* namespace nle */
