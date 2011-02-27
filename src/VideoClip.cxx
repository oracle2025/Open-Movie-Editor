/*  VideoClip.cxx
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

#include <cassert>

#include "VideoClip.H"
#include "IVideoFile.H"
#include "FilmStrip.H"
#include "AudioFileFactory.H"
#include "ErrorDialog/IErrorHandler.H"
#include "VideoClipArtist.H"
#include "FilmStripFactory.H"
#include "IAudioFile.H"
#include "helper.H"
#include "WavArtist.H"

namespace nle
{
VideoClip::VideoClip( Track* track, int64_t position, IVideoFile* vf, int64_t A, int64_t B, int id, ClipData* data )
	: AudioClip( track, position, 0, id ), VideoEffectClip( this )
{
	m_trimA = A;
	m_trimB = B;
	m_audioFile = 0;
	m_videoFile = vf;
	m_lastFrame = -1;
	m_audioReader = 0;
	
	m_audioFile = vf->getAudioReader();
	if ( !m_audioFile ) {
		m_audioFile = AudioFileFactory::get( m_videoFile->filename() );
	}
	CLEAR_ERRORS();


	setEffects( data );

	if ( track->render_mode() ) { // Render mode
		m_artist = 0;
		m_audioReader = m_audioFile;
	} else { // Playback mode
		g_filmStripFactory->ref( vf );
		m_artist = new VideoClipArtist( this );
		if ( m_audioFile ) {
			g_wavArtist->add( m_audioFile );
			m_audioReader = m_audioFile;
		}
	}
}
int VideoClip::w()
{
	return m_videoFile->width();
}
int VideoClip::h()
{
	return m_videoFile->height();
}
bool VideoClip::hasAudio()
{
	return ( m_audioFile != 0 );
}
VideoClip::~VideoClip()
{
	if ( m_artist ) {
		delete m_artist;
		m_artist = 0;
		g_wavArtist->remove( m_videoFile->filename() );
		g_filmStripFactory->unref( m_videoFile );
	}
	delete m_videoFile;
}
string VideoClip::filename()
{
	return m_videoFile->filename();
}
int64_t VideoClip::maxAudioLength()
{
	return m_videoFile->length() * 48000 / NLE_TIME_BASE;
}
int64_t VideoClip::length()
{
	return m_videoFile->length() - ( m_trimA + m_trimB );
}
int64_t VideoClip::audioTrimA()
{
	return m_trimA * 48000 / NLE_TIME_BASE;
}

int64_t VideoClip::audioTrimB()
{
	int64_t r = m_trimB * 48000 / NLE_TIME_BASE;
	int64_t t = m_audioFile->length() - maxAudioLength() + r;
	return t < 0 ? 0 : t;
}
int64_t VideoClip::audioPosition()
{
	return m_position * 48000 / NLE_TIME_BASE;
}
void VideoClip::reset()
{
	AudioClip::reset();
	m_lastFrame = -1;
}
LazyFrame* VideoClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	if ( position < m_position || position > m_position + length() )
		return NULL;
	int64_t s_pos = position - m_position + m_trimA;
	position_in_file = s_pos;
	return m_videoFile->getFrame( s_pos );
}
int64_t VideoClip::fileLength()
{
	return m_videoFile->length();
}	
int64_t VideoClip::trimA( int64_t trim )
{
	if ( trim + m_trimA < 0 ) {
		trim = -m_trimA;
	}
	if ( length() - trim <= 0 || trim == 0 ) {
		return 0;
	}
	return Clip::trimA( trim );
}
int64_t VideoClip::trimB( int64_t trim )
{
	if ( trim + m_trimB < 0 ) {
		trim = -m_trimB;
	}
	if ( length() - trim <= 0 ) {
		return 0;
	}
	return Clip::trimB( trim );
}
int VideoClip::interlacing()
{
	return m_videoFile->interlacing();
}

} /* namespace nle */
