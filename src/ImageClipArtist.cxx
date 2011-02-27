/*  ImageClipArtist.cxx
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

#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>

#include "ImageClipArtist.H"

namespace nle 
{

ImageClipArtist::ImageClipArtist( Fl_Image* image )
{
	m_image = image->copy( 40, 30 );
	m_image_new = 0;
}
ImageClipArtist::~ImageClipArtist()
{
	if ( m_image ) {
		delete m_image;
	}
}
void ImageClipArtist::render( Rect& rect, int64_t, int64_t )
{
	if ( m_image_new ) {
		if ( m_image ) {
			delete m_image;
		}
		m_image = m_image_new->copy( 40, 30 );
		m_image_new = 0;
	}
	fl_push_clip( rect.x, rect.y, rect.w, rect.h );		
	m_image->draw( rect.x, rect.y );
	m_image->draw( rect.x + rect.w - 40, rect.y );
	fl_draw_box( FL_BORDER_FRAME, rect.x, rect.y, 40, 30, FL_WHITE );
	fl_draw_box( FL_BORDER_FRAME, rect.x + rect.w - 40, rect.y, 40, 30, FL_WHITE );
	fl_pop_clip();
}
void ImageClipArtist::image( Fl_Image* image )
{
	m_image_new = image;
	// This is evil, because it keeps the reference for later
}


} /* namespace nle */
