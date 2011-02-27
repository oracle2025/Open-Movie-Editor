/* JackPlaybackCore.H
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

#ifndef _JACK_PLAYBACK_CORE_H_
#define _JACK_PLAYBACK_CORE_H_

#include <stdint.h>
#include <jack/jack.h>
#include <jack/transport.h>
#include "IPlaybackCore.H"

namespace nle
{
class IAudioReader;
class IVideoReader;
class IVideoWriter;
class AudioThreadedRingbuffer;

class JackPlaybackCore : public IPlaybackCore
{
	public:
		JackPlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter );
		~JackPlaybackCore();
		bool ok();

		void play();
		void stop();
		void jackreadAudio( jack_nframes_t nframes );
		void flipFrame();
		void hardstop();
		int sync( jack_transport_state_t state, jack_position_t *pos );
		void play_ping( int fd );
		void stop_ping( int fd );
	private:
		IAudioReader* m_audioReader;
		IVideoReader* m_videoReader;
		IVideoWriter* m_videoWriter;
		int64_t m_currentFrame;
		AudioThreadedRingbuffer* m_threadedRingbuffer;
		int m_play_pipe[2];
		int m_stop_pipe[2];
};

} /* namespace nle */

#endif /* _JACK_PLAYBACK_CORE_H_ */

