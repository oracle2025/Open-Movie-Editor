/*  AudioFileGmerlin.H
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

#ifndef _AUDIO_FILE_Gmerlin_H_
#define _AUDIO_FILE_Gmerlin_H_

#include <stdint.h>
extern "C" {
#include <gmerlin/avdec.h>
}

#include "global_includes.H"
#include "IAudioFile.H"
#include "globals.H"

struct bgav_s;
typedef struct bgav_s bgav_t;

namespace nle
{

class AudioFileGmerlin : public IAudioFile
{
	public:
		AudioFileGmerlin( string filename );
		~AudioFileGmerlin();
		void seek( int64_t sample );
		int fillBuffer( float* output, unsigned long frames );
			// sizof(output) == frames * 2
			// File Format: 44100 stereo interleaved
	private:
		bgav_t* m_file;
		gavl_audio_frame_t* m_frame1;
		gavl_audio_frame_t* m_frame2;
		gavl_audio_converter_t* m_converter;
		int m_samples_per_frame;
		void reinit_frame( unsigned long frames );
		gavl_audio_format_t m_format;
		gavl_audio_format_t m_float_format;
};

}

#endif /* _AUDIO_FILE_GMERLIN_H_ */

