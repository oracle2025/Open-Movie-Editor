/*  render_helper.H
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

#ifndef _RENDER_HELPER_H_
#define _RENDER_HELPER_H_

#include <gavl/gavl.h>

namespace nle
{

void rectangle_crop_aspect(const gavl_rectangle_i_t * dst_rect,
                               const gavl_video_format_t * src_format,
                               gavl_rectangle_f_t * src_rect,
                               const gavl_video_format_t * dst_format);

void crop_format( int src_w, int src_h, float src_aspect, int src_blank,
		int dst_w, int dst_h, float dst_aspect, int dst_blank,
		double& src_rect_x, double& src_rect_y, double& src_rect_w, double& src_rect_h,
       		int& dst_rect_x, int& dst_rect_y, int& dst_rect_w, int& dst_rect_h );
void fit_format(
		int src_w, int src_h, float src_aspect, int src_blank,
		int dst_w, int dst_h, float dst_aspect, int dst_blank,
		double& src_rect_x, double& src_rect_y, double& src_rect_w, double& src_rect_h,
       		int& dst_rect_x, int& dst_rect_y, int& dst_rect_w, int& dst_rect_h );
void stretch_format(
		int src_w, int src_h, float src_aspect, int src_blank,
		int dst_w, int dst_h, float dst_aspect, int dst_blank,
		double& src_rect_x, double& src_rect_y, double& src_rect_w, double& src_rect_h,
       		int& dst_rect_x, int& dst_rect_y, int& dst_rect_w, int& dst_rect_h );

void halve_image( unsigned char *optr, unsigned char *iptr, int iw, int ih );

void blend( unsigned char* dst, unsigned char* src1, unsigned char* src2, float alpha, int len );
void blend_alpha( unsigned char* dst, unsigned char* rgb, unsigned char* rgba, float alpha, int len );
void blend_alpha2( unsigned char* dst, unsigned char* rgba, unsigned char* rgba2, float alpha, int len );

unsigned int mixChannels( float *A, float *B, float* out, unsigned int frames );

//shamelessly extracted from libquicktime
void decode_int16_to_float(void * _in, float ** out, int num_channels, int num_samples);
void decode_uint8_to_float(void * _in, float ** out, int num_channels, int num_samples);

// Frame: is interlaced
// Fields: two images above each other in one array.
void frame_to_fields( int interlace_order /* 1=top ff, 2=bottom ff */, unsigned
	char* src, unsigned char* dst, int w, int h, bool src_alpha );
void fields_to_frames( int interlace_order /* 1=top ff, 2=bottom ff */, unsigned
	char* src, unsigned char* dst, int w, int h );

} /* namespace nle */

#endif /* _RENDER_HELPER_H_ */

