/* PortAudio19PlaybackCore.H
 *
 *  Copyright (C) 2005, 2006, 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _PORT_AUDIO_19_PLAYBACK_CORE_H_
#define _PORT_AUDIO_19_PLAYBACK_CORE_H_

#include <stdint.h>

#include "IPlaybackCore.H"

namespace nle
{
class IAudioReader;
class IVideoReader;
class IVideoWriter;


class PortAudio19PlaybackCore : public IPlaybackCore
{
	public:
		PortAudio19PlaybackCore( IAudioReader* audioReader, IVideoReader* videoReader, IVideoWriter* videoWriter );
		~PortAudio19PlaybackCore();
		void play();
		void stop();
		bool ok() { return true; }
		int readAudio( float* output, unsigned long frames );
		void flipFrame();
		void checkPlayButton();
	private:
		IAudioReader* m_audioReader;
		IVideoReader* m_videoReader;
		IVideoWriter* m_videoWriter;
		int64_t m_audioPosition;
		int64_t m_currentFrame;
		int64_t m_lastFrame;
};

} /* namespace nle */

#endif /* _PORT_AUDIO_PLAYBACK_CORE_H_ */

