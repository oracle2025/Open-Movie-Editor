/*  video_format.H
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

#ifndef _VIDEO_FORMAT_H_
#define _VIDEO_FORMAT_H_

namespace nle
{
	
typedef struct
{
	int frame_duration;
	int timescale;
	int audio_frames_per_chunk;
	int video_frames_per_chunk;
} video_framerate;

typedef struct
{
	char name[255];
	video_framerate framerate;
	int w;
	int h;
	int pixel_w;
	int pixel_h;
	float pixel_aspect_ratio;
	int interlacing;
	char video_codec[255];
	char audio_codec[255];
	int samplerate;
} video_format;

} /* namespace nle */

#endif /* _VIDEO_FORMAT_H_ */
