/*  AudioFileSnd.H
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

#ifndef _AUDIO_FILE_SND_H_
#define _AUDIO_FILE_SND_H_

#include <sndfile.h>

#include "global_includes.H"
#include "IAudioFile.H"

namespace nle
{

class AudioFileSnd : public IAudioFile
{
	public:
		AudioFileSnd( string filename );
		~AudioFileSnd();
		void seek( int64_t position );
		int fillBuffer( float* output, unsigned long frames );
	private:
		SNDFILE *m_sndfile;
		bool m_mono;

};

} /* namespace nle */


#endif /* _AUDIO_FILE_SND_H_ */

