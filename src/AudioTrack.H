/*  AudioTrack.H
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

#ifndef _AUDIO_TRACK_H_
#define _AUDIO_TRACK_H_

#include "TrackBase.H"
#include "global_includes.H"

namespace nle
{

class AudioClip;

class AudioTrack : public TrackBase
{
	public:
		// constructors and destructors
		AudioTrack( ClipIdProvider* idProvider, int num, string name = "Audio" );
		~AudioTrack();

		// method declerations
		void addFile( int64_t position, string filename, int64_t trimA = 0, int64_t trimB = 0, int mute = 0, int id = -1, int64_t length = -1, ClipData* data = 0 );
		int type() { return TRACK_TYPE_AUDIO; }
		void sort();
		float stretchFactor() { return ( 48000.0 ); }
		int64_t soundLength();
		int getClipId() { return m_idProvider->getClipId(); }
		void updateClipId( int id ) { m_idProvider->updateClipId( id ); }
		bool render_mode();
	private:
		ClipIdProvider* m_idProvider;
};
	
} /* namespace nle */

#endif /* _AUDIO_TRACK_H_ */
