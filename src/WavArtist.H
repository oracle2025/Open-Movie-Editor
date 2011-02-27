/*  WavArtist.H
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

#ifndef _WAV_ARTIST_H_
#define _WAV_ARTIST_H_

#include "globals.H"
#include "Rect.H"
#include "global_includes.H"
#include "JobManager.H"

#define PEAK_RANGE 1000

namespace nle
{

class IAudioFile;
class WaveForm;

typedef struct _peakfile_node {
	struct _peakfile_node *next;
	WaveForm* wav;
	string filename;
	int refCount;
} peakfile_node;

class WavArtist : public JobDoneListener
{
	public:
		WavArtist();
		~WavArtist();
		void add( IAudioFile* file );
		void remove( string filename );
		void render( string filename, Rect& rect, int64_t start, int64_t stop );
		void jobDone( Job* job_description );
	private:
		peakfile_node* m_peaks;
};

} /* namespace nle */


#endif /* _WAV_ARTIST_H_ */
