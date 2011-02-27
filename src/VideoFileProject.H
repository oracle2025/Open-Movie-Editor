/*  VideoFileProject.H
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

#ifndef _VIDEO_FILE_PROJECT_H_
#define _VIDEO_FILE_PROJECT_H_

#include "IVideoFile.H"
#include "Timeline.H"
#include <string>

namespace nle
{

class LazyFrame;

class VideoFileProject : public IVideoFile
{
	public:
		VideoFileProject( std::string filename );
		~VideoFileProject();
		bool ok();
		int64_t length();
		LazyFrame* read();
		void seek( int64_t position );
		const char* decoder() { return "gmerlin"; }
		IAudioFile* getAudioReader();
	private:
		int64_t ticksPerFrame();
		void seekToFrame( int64_t frame );
	private:
		Timeline *m_timeline;
		bool m_ok;
};

} /* namespace nle */

#endif /* _VIDEO_FILE_PROJECT_H_ */
