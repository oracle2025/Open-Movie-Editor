/*  IAudioFile.H
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

#ifndef _I_AUDIO_FILE_H_
#define _I_AUDIO_FILE_H_

#include <stdint.h>
#include "IAudioReader.H"
#include "global_includes.H"

namespace nle
{

class IAudioFile : public IAudioReader
{
	public:
		virtual ~IAudioFile() {}
		inline bool ok() { return m_ok; }
		inline int64_t length() { return m_length; }              // TODO: Change to TIME_BASE
			// <= number of floats: 2 * length (left, right)
		virtual void seek( int64_t sample ) = 0;                  // TODO: Change to TIME_BASE
		void sampleseek( int, int64_t position ) { seek( position ); }
		virtual int fillBuffer( float* output, unsigned long frames ) = 0;
			//sizof(output) = frames * 2
			//File Format: 48000 stereo interleaved
		inline string filename() { return m_filename; }
		inline int samplerate() { return m_samplerate; } // propably not useful
	protected:
		int64_t m_length;
		bool m_ok;
		string m_filename;
		int m_samplerate;
};

} /* namespace nle */

#endif /* _I_AUDIO_FILE_H_ */
