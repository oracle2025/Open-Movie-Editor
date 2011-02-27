/*  Resampler.H
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _RESAMPLER_H_
#define _RESAMPLER_H_

#include <stdint.h>

#include <samplerate.h>

#include "IAudioFile.H"

namespace nle
{

class IAudioFile;

class Resampler : public IAudioFile
{
	public:
		Resampler( IAudioFile* audiofile );
		~Resampler();
		void seek( int64_t position );
		int fillBuffer( float* output, unsigned long frames );
		float m_buffer[4096*2];
		IAudioFile* m_audiofile;
	private:
		int m_samplerate;
		int m_srcSamplerate;
		SRC_STATE* m_state;
		int m_error;
};

} /* namespace nle */

#endif /* _RESAMPLER_H_ */
