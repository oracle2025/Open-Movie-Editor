/*  TitleClip.cxx
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

#include <cassert>
#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>

#include "TitleClip.H"
#include "ErrorDialog/IErrorHandler.H"
#include "ImageClipArtist.H"
#include "globals.H"
#include "nle.h"
#include "timeline/Track.H"
#include "LazyFrame.H"

namespace nle
{
class TitleClipData : public ClipData
{
	public:
		~TitleClipData() { 
			if ( effect_data ) {
				delete effect_data;
			}
		}
		string text;
		int size;
		int font;
		float x;
		float y;
		Fl_Color color;
		ClipData* effect_data;
};



TitleClip::TitleClip( Track* track, int64_t position, int64_t length, int id, ClipData* data )
	: FilterClip( track, position, id ), VideoEffectClip( this )
{
	m_ok = false;
	m_artist = 0;
	m_image = 0;
	m_render_mode = track->render_mode();
	
	m_dirty = false;

	
	if ( length > 0 ) {
		m_length = length;
	} else {
		m_length = NLE_TIME_BASE * 10;
	}
	m_lazy_frame = new LazyFrame( 768, 576 );
	m_gavl_frame = gavl_video_frame_create( 0 );
	m_lazy_frame->put_data( m_gavl_frame );
	m_gavl_frame->strides[0] = 768 * 4;
	m_pixels = new unsigned char[ 768*576*4 ];
	m_alpha = new unsigned char[ 768*576*3 ];
	m_ok = true;
	m_offscreen = 0;
	TitleClipData* title_data = dynamic_cast<TitleClipData*>(data);	
	if ( title_data ) {
		m_font = title_data->font;
		m_size = title_data->size;
		m_x = title_data->x;
		m_y = title_data->y;
		m_color = title_data->color;
		m_text = title_data->text;
		setEffects( title_data->effect_data );
	} else {
		m_font = FL_HELVETICA_BOLD;
		m_size = 50;
		m_x = 0.5;
		m_y = 0.5;
		m_color = FL_WHITE;
		m_text = "A Movie by";
	}
	m_lazy_frame->this_frame_wont_change();
}
void TitleClip::init()
{
	if ( ( m_image && !m_dirty ) || g_PREVENT_OFFSCREEN_CRASH ) {
		return;
	}
	g_ui->mainWindow->make_current();
	if ( !m_offscreen ) {
		m_offscreen = fl_create_offscreen(768, 576);
	}
	m_dirty = false;
	char* text = new char[m_text.length() + 1];
	strcpy( text, m_text.c_str() );
	
	
	fl_begin_offscreen(m_offscreen);
	fl_draw_box(FL_FLAT_BOX, 0, 0, 768, 576, FL_DARK3);
	fl_font(m_font, m_size);
	int w = 0;
	int h = 0;
	fl_measure( m_text.c_str(), w, h );

	int x_range = ( 768 - w );
	int y_range = ( 576 - h );
	int x = lrint(x_range * m_x);
	int y = h + lrint(y_range * m_y) - fl_descent();

	char* p = strtok( text, "\n" );
	int o_w = 0;
	int o_h = 0;
	fl_measure( p, o_w, o_h );
	y = o_h + lrint(y_range * m_y) - fl_descent();
	for ( ; p; p = strtok( 0, "\n" ) ) {
		o_h = o_w = 0;
		fl_measure( p, o_w, o_h );
		x = lrint( ( 768 - o_w )* m_x);
		fl_color(fl_contrast(FL_DARK3,m_color));
		fl_draw( p, x, y );
		fl_color(m_color);
		fl_draw( p, x - 2, y - 2 );
		y = y + o_h;
	}

	fl_read_image(m_pixels, 0, 0, 768, 576, 255);
	x = lrint(x_range * m_x);
	y = h + lrint(y_range * m_y) - fl_descent();
	
	fl_draw_box(FL_FLAT_BOX, 0, 0, 768, 576, FL_DARK3);	
	
	//fl_draw_box(FL_BORDER_FRAME, x, y - h + fl_descent(), w, h, FL_WHITE);

	strcpy( text, m_text.c_str() );
	p = strtok( text, "\n" );
	fl_color(FL_WHITE);
	y = o_h + lrint(y_range * m_y) - fl_descent();
	for ( ; p; p = strtok( 0, "\n" ) ) {
		o_h = o_w = 0;
		fl_measure( p, o_w, o_h );
		x = lrint( ( 768 - o_w )* m_x);
		fl_draw( p, x, y );
		fl_draw( p, x - 2, y - 2 );
		y = y + o_h;
	}

	
	
	fl_read_image(m_alpha, 0, 0, 768, 576);
	uchar* src = m_alpha;
	uchar* dst = m_pixels-1;
	for ( int i = 576*768; i > 0; i-- ) {
		dst += 4;
		*dst = *src;
		src += 3;
		
	}
	//free(alpha);
	fl_end_offscreen();
	if ( !m_image ) {
		m_image = new Fl_RGB_Image( m_pixels, 768, 576, 4 );
	} else {
		m_image->uncache();
	}
	//free(pixels);
	char** d = (char**)m_image->data();
	m_gavl_frame->planes[0] = (unsigned char *)d[0];
	m_lazy_frame->dirty( true );
	if ( !m_render_mode ) {
		if ( m_artist ) {
			ImageClipArtist* ica = dynamic_cast<ImageClipArtist*>(m_artist);
			assert( ica );
			ica->image( m_image );
		} else {
			m_artist = new ImageClipArtist( m_image );
		}
	}
	delete [] text;
}

TitleClip::~TitleClip()
{
	if ( m_artist ) {
		delete m_artist;
	}
	if ( m_image ) {
		delete m_image;
	}
	if ( m_alpha ) {
		delete m_alpha;
	}
	if ( m_offscreen ) {
		fl_delete_offscreen(m_offscreen);
	}
}
int64_t TitleClip::length()
{
	return m_length;
}

LazyFrame* TitleClip::getRawFrame( int64_t position, int64_t &position_in_file )
{
	if ( !m_image || m_dirty ) {
		init();
	}
	if ( !m_image ) {
		return 0;
	}
	position_in_file = position - m_position;
	if ( position >= m_position && position <= m_position + m_length ) {
		return m_lazy_frame;
	} else {
		return 0;
	}
}
int TitleClip::w()
{
	return 768;//m_image->w();
}
int TitleClip::h()
{
	return 576;//m_image->h();
}
int64_t TitleClip::trimA( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	if ( m_position + trim < 0 ) {
		trim = (-1) * m_position;
	}
	m_length -= trim;
	m_position += trim;
	return trim;
}
int64_t TitleClip::trimB( int64_t trim )
{
	if ( m_length - trim < 0 ) {
		return 0;
	}
	m_length -= trim;
	return trim;
}
int64_t TitleClip::fileLength()
{
	return -1;
}
ClipData* TitleClip::getClipData()
{
	TitleClipData* data = new TitleClipData;
	data->text = m_text;
	data->size = m_size;
	data->font = m_font;
	data->x = m_x;
	data->y = m_y;
	data->color = m_color;
	data->effect_data = vec_getClipData();
	return data;
}

} /* namespace nle */
