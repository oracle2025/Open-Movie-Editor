/*  VideoTrack.H
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

#ifndef _VIDEO_TRACK_H_
#define _VIDEO_TRACK_H_

#include "TrackBase.H"
#include "global_includes.H"
#include "timebase.H"

namespace nle
{


typedef struct _fade_over_struct {
	struct _fade_over_struct* next;
	Clip* clipA;
	Clip* clipB;
	float inc;
} fade_over;

int64_t fade_over_start( fade_over* o );
int64_t fade_over_end( fade_over* o );
fade_over* create_fade_over( Clip* a, Clip* b );
void get_alpha_values( fade_over* o, float& a, float& b, int64_t position );

class VideoClip;
class LazyFrame;

class VideoTrack : public TrackBase
{
	public:
		VideoTrack( ClipIdProvider* idProvider, int num, string name = "Video" );
		virtual ~VideoTrack();

		void addFile( int64_t position, string filename, int64_t trimA = 0, int64_t trimB = 0, int mute = 0, int id = -1, int64_t length = -1, ClipData* data = 0 );
		int type() { return TRACK_TYPE_VIDEO; }
		float stretchFactor() { return (float)NLE_TIME_BASE; }
		LazyFrame* getFrame( int64_t position );
		LazyFrame** getFrameStack( int64_t position );
		void sort();
		void reconsiderFadeOver();
		void removeClip( Clip* clip );
		int fillBuffer( float* output, unsigned long frames, int64_t position ); //Needed for Crossfade handling
		int getClipId() { return m_idProvider->getClipId(); }
		void updateClipId( int id ) { m_idProvider->updateClipId( id ); }
		bool render_mode();
	private:
		int64_t m_totalAudioLength;
		int64_t m_playPosition;
		clip_node* m_vidCurrent;
		fade_over* m_fade_overs;
		fade_over* m_currentAudioFadeOver;
		ClipIdProvider* m_idProvider;
};
	
} /* namespace nle */

#endif /* _VIDEO_TRACK_H_ */
