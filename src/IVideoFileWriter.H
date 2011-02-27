/* IVideoFileWriter.H
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

#ifndef _I_VIDEO_FILE_WRITER_H_
#define _I_VIDEO_FILE_WRITER_H_

#include "RenderDialog/video_format.H"

namespace nle
{

class LazyFrame;

class IVideoFileWriter
{
	public:
		virtual ~IVideoFileWriter() {}
		virtual void encodeVideoFrame( LazyFrame* frame ) = 0;
		virtual void encodeAudioFrame( float* buffer, int frames ) = 0;
		virtual video_format* format() = 0;
		virtual int samplerate() = 0;
};

	
} /* namespace nle */

#endif /* _I_VIDEO_FILE_WRITER_H_ */
