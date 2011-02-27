/*  InkscapeClip.cxx
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

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl.H>

#include "InkscapeClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "ImageClip.H"
#include "ImageClipArtist.H"
#include "helper.H"
#include "globals.H"
#include "timeline/Track.H"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "VideoViewGL.H"
#include <cassert>
#define BUFFER_LEN 1024
namespace nle
{

static void detect_svg_update_callback( void* data );

InkscapeClip::InkscapeClip( Track* track, int64_t position, string filename, int64_t length, int id, ClipData* data )
	: FilterClip( track, position, id ), VideoEffectClip( this )
{
	"/usr/share/openmovieeditor/svg-titles/0000.svg";
	"720x576";
	"inkscape --export-png=123.png /usr/share/openmovieeditor/svg-titles/0002.svg";
	m_filename = filename;


	char buffer[BUFFER_LEN];
	findpath( filename.c_str(), buffer, BUFFER_LEN );
	string cachepath = string(g_homefolder) + ("/.openme/cache" PREF_FILE_ADD)+ buffer;
	string cachefile = string(g_homefolder) + ("/.openme/cache" PREF_FILE_ADD) + filename + ".png";



	/*

	Clip Create: copy svg-template into Projects Folder $HOME/Video\ Projects/random_file_name().svg
	Clip Render: generate png from svg, using inkscape $HOME/Video\ Projects/random_file_name().png
	Clip Edit: spawn inkscape, with copy from Projects Folder, and check
	whether it was saved, when new version is detetected, run Clip Render

	Extend the context Menü of the clip, for editing

	*/
	string svg_filename = filename;
	string inkscape_command = "inkscape --export-png=\"";
	inkscape_command += cachefile;
	inkscape_command += "\" \"";
	inkscape_command += svg_filename;
	inkscape_command += "\"";
	system( inkscape_command.c_str() );

	if ( length > 0 ) {
		m_length = length;
	} else {
		m_length = NLE_TIME_BASE * 10;
	}
	m_image_clip = new ImageClip( track, position, cachefile, length, id );

	setEffects( data );
	Fl::add_timeout(1.0, detect_svg_update_callback, this );
}


InkscapeClip::~InkscapeClip()
{
	Fl::remove_timeout( detect_svg_update_callback, this );
	delete m_image_clip;
}
int64_t InkscapeClip::length()
{
	return m_length;
}

void InkscapeClip::doAction( int index )
{
	string svg_filename = m_filename;
	string inkscape_command = "inkscape \"";
	inkscape_command += svg_filename;
	inkscape_command += "\"";
	//system( inkscape_command.c_str() );

	const char* args[] = { "/bin/sh", "-c", inkscape_command.c_str(), (char *)0 };
	if ( fork() == 0 ) {
		execvp( "/bin/sh", (char* const*)args );
	}
}
static void detect_svg_update_callback( void* data ) {
	InkscapeClip* ic = (InkscapeClip*)data;
	ic->detectSvgUpdate();
	Fl::repeat_timeout(1.0, detect_svg_update_callback, data);
}
void InkscapeClip::detectSvgUpdate()
{
	string svg_filename = m_filename;
	char buffer[BUFFER_LEN];
	findpath( m_filename.c_str(), buffer, BUFFER_LEN );
	string cachepath = string(g_homefolder) + ("/.openme/cache" PREF_FILE_ADD)+ buffer;
	string cachefile = string(g_homefolder) + ("/.openme/cache" PREF_FILE_ADD) + m_filename + ".png";
	string png_filename = cachefile;
	struct stat statbuf1;
	struct stat statbuf2;
	int r = stat( svg_filename.c_str(), &statbuf1 );
	assert( r != -1 );
	r = stat( png_filename.c_str(), &statbuf2 );
	assert( r != -1 );
	if ( statbuf1.st_mtime > statbuf2.st_mtime ) {
		string inkscape_command = "inkscape --export-png=\"";
		inkscape_command += png_filename;
		inkscape_command += "\" \"";
		inkscape_command += svg_filename;
		inkscape_command += "\"";
		system( inkscape_command.c_str() );
		delete m_image_clip;
		m_image_clip = new ImageClip( track(), position(), png_filename, length(), m_id );
		g_videoView->redraw();
	}
}

LazyFrame* InkscapeClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	position_in_file = position - m_position;
	if ( position >= m_position && position <= m_position + m_length ) {
		return m_image_clip->getRawFrame( m_image_clip->position(), position_in_file );
	} else {
		return 0;
	}
}
int InkscapeClip::w()
{
	return m_image_clip->w();
}
int InkscapeClip::h()
{
	return m_image_clip->h();
}
int64_t InkscapeClip::trimA( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	if ( m_position + trim < 0 ) {
		trim = (-1) * m_position;
	}
	m_length -= trim;
	m_position += trim;
	return trim;
}
int64_t InkscapeClip::trimB( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	m_length -= trim;
	return trim;
}
int64_t InkscapeClip::fileLength()
{
	return m_image_clip->fileLength();
}
bool InkscapeClip::ok()
{
	return m_image_clip->ok();
}
IClipArtist* InkscapeClip::getArtist()
{
	return m_image_clip->getArtist();
}

} /* namespace nle */
