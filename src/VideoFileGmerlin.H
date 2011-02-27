/*  VideoFileGmerlin.H
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _VIDEO_FILE_GMERLIN_H_
#define _VIDEO_FILE_GMERLIN_H_

extern "C" {
#include <gmerlin/avdec.h>
}

#include "IVideoFile.H"
#include <string>

namespace nle
{

class LazyFrame;
class IProgressListener;

class VideoFileGmerlin : public IVideoFile
{
	public:
		VideoFileGmerlin( std::string filename, IProgressListener* progress = 0 );
		~VideoFileGmerlin();
		bool ok();
		int64_t length();
		LazyFrame* read();
		void seek( int64_t position );
		const char* decoder() { return "gmerlin"; }
		IProgressListener* m_progress;
	protected:
		int64_t ticksPerFrame();
		void seekToFrame( int64_t frame );
	private:
		bool m_ok;
		int64_t m_ticksPerFrame;
		LazyFrame* m_lazy_frame;
		gavl_video_frame_t *m_gavl_frame;
		const gavl_video_format_t *m_video_format;
		bgav_t* m_decoder;
};

} /* namespace nle */

#endif /* _VIDEO_FILE_GMERLIN_H_ */
