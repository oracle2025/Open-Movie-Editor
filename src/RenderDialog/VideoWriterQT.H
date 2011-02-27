/* VideoWriterQT.H
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _VIDEO_WRITER_QT_H_
#define _VIDEO_WRITER_QT_H_

#include "IVideoFileWriter.H"
#include "video_format.H"
#include <lqt.h>
#include <gavl/gavl.h>

namespace nle
{

class EncodingPreset;

class VideoWriterQT : public IVideoFileWriter
{
	public:
		VideoWriterQT( EncodingPreset* preset, const char* filename );
		~VideoWriterQT();
		void encodeVideoFrame( LazyFrame* frame );
		void encodeAudioFrame( float* buffer, int frames );
		video_format* format() { return &m_format; }
		int samplerate() { return m_samplerate; }
	private:
		quicktime_t *m_qt;
		float left_buffer[32000];
		float right_buffer[32000];
		video_format m_format;
		int m_samplerate;
		unsigned char** m_rows;
		gavl_video_format_t m_gavl_format;
		int64_t m_timestamp;
		//gavl_audio_format_t m_gavl_audio_format;
};

	
} /* namespace nle */

#endif /* _VIDEO_WRITER_QT_H_ */
