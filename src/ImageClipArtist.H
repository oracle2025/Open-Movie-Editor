/*  ImageClipArtist.H
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

#ifndef _IMAGE_CLIP_ARTIST_H_
#define _IMAGE_CLIP_ARTIST_H_

#include "IClipArtist.H"

class Fl_Image;

namespace nle
{

class ImageClip;

class ImageClipArtist : public IClipArtist
{
	public:
		ImageClipArtist( Fl_Image* image );
		~ImageClipArtist();
		void render( Rect& rect, int64_t start, int64_t stop ); 
		void image( Fl_Image* image );
	private:
		Fl_Image* m_image;
		Fl_Image* m_image_new;

};

} /* namespace nle */

#endif /* _IMAGE_CLIP_ARTIST_H_ */
