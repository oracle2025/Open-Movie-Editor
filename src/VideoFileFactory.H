/*  VideoFileFactory.H
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

#ifndef _VIDEO_FILE_FACTORY_H_
#define _VIDEO_FILE_FACTORY_H_

#include "global_includes.H"

namespace nle
{

class IVideoFile;

extern void* g_video_file_factory_progress;

class VideoFileFactory
{
	public:
		static IVideoFile* get( string filename );
};

} /* namespace nle */

#endif /* _VIDEO_FILE_FACTORY_H_ */

