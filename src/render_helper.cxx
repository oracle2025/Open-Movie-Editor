/*  render_helper.cxx
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

#include <gavl/gavl.h>
#include <math.h>

#include "global_includes.H"
#include "render_helper.H"

namespace nle
{
	
/*
  optr is a pointer to a place to store the output image.
  iptr is a pointer to the input image.
  iw and ih are the width and height of the input image respectively.
*/

void halve_image ( unsigned char *optr,
                   unsigned char *iptr,
                   int iw, int ih )
{
/*	for ( int i = 0; i < ih >> 1; i++ ) {
		for ( int j = 0; j < iw; j++ ) {
			*optr++ = *iptr++;
		}
	}
	return;*/
  int ow   = iw >> 1 ;
  int oh   = ih >> 1 ;
  int iw3  = iw * 3 ;     /* Offset to get to the pixel below */
  int iw33 = iw * 3 + 3 ; /* Offset to get to the pixel below/right */

  for ( int i = 0 ; i < oh ; i++ )
  {
    for ( int j = 0 ; j < ow ; j++ )
    {
      /* Average red/green/blue for each pixel */
      *optr++ = ( *iptr + *(iptr + 3) + *(iptr + iw3) + *(iptr + iw33) ) >> 2; iptr++;
      *optr++ = ( *iptr + *(iptr + 3) + *(iptr + iw3) + *(iptr + iw33) ) >> 2; iptr++;
      *optr++ = ( *iptr + *(iptr + 3) + *(iptr + iw3) + *(iptr + iw33) ) >> 2; iptr++;
      iptr += 3 ;  /* Skip to the next pixel */
    }

    iptr += iw3 ;  /* Skip to the next row of pixels */
  }
}


// inspired by rasterman, but poorly done ;)
void blend( unsigned char* dst, unsigned char* src1, unsigned char* src2, float alpha, int len )
{
	unsigned char *ps1, *ps2, *pd, *pd_end;
	unsigned int a = (unsigned char)( alpha * 255 );
	ps1 = src1;
	ps2 = src2;
	pd = dst;
	pd_end = dst + ( len * 3 );
	while ( pd < pd_end ) {
		pd[0] = ( ( ( ps1[0] - ps2[0] ) * a ) >> 8 ) + ps2[0];
		ps1++;
		ps2++;
		pd++;
	}
}
void blend_alpha( unsigned char* dst, unsigned char* rgb, unsigned char* rgba, float alpha, int len )
{
	unsigned char *ps1, *ps2, *pd, *pd_end;
	unsigned int a = (unsigned char)( alpha * 255 );
	ps1 = rgba;
	ps2 = rgb;
	pd = dst;
	pd_end = dst + ( len * 3 );
	while ( pd < pd_end ) {
		pd[0] = ( ( ( ps1[0] - ps2[0] ) * a * ps1[3] ) >> 16 ) + ps2[0];
		pd[1] = ( ( ( ps1[1] - ps2[1] ) * a * ps1[3] ) >> 16 ) + ps2[1];
		pd[2] = ( ( ( ps1[2] - ps2[2] ) * a * ps1[3] ) >> 16 ) + ps2[2];
		ps1 += 4;
		ps2 += 3;
		pd += 3;
	}
}
static unsigned char MAX( unsigned char a, unsigned char b )
{
	if ( a > b ) {
		return a;
	} else {
		return b;
	}
}
void blend_alpha2( unsigned char* dst, unsigned char* rgb, unsigned char* rgba, float alpha, int len )
{
	unsigned char *ps1, *ps2, *pd, *pd_end;
	unsigned int a = (unsigned char)( alpha * 255 );
	ps1 = rgba;
	ps2 = rgb;
	pd = dst;
	pd_end = dst + ( len * 4 );
	while ( pd < pd_end ) {
		pd[0] = ( ( ( ps1[0] - ps2[0] ) * a * ps1[3] ) >> 16 ) + ps2[0];
		pd[1] = ( ( ( ps1[1] - ps2[1] ) * a * ps1[3] ) >> 16 ) + ps2[1];
		pd[2] = ( ( ( ps1[2] - ps2[2] ) * a * ps1[3] ) >> 16 ) + ps2[2];
		pd[3] = MAX( ps1[3], ps2[3] );
		ps1 += 4;
		ps2 += 4;
		pd += 4;
	}
}

unsigned int mixChannels( float *A, float *B, float* out, unsigned int frames )
{
	float *p_output = out;
	float *p_A = A;
	float *p_B = B;
	for ( unsigned int i = frames * 2; i > 0; i-- ){
		*p_output = *p_A + *p_B;
		p_output++;
		p_A++;
		p_B++;
	}
	return frames;
}
#define UINT8_TO_FLOAT(src, dst) dst = (float)src / 127.0 - 1.0
#define INT16_TO_FLOAT(src, dst) dst = (float)src / 32768.0
void decode_int16_to_float(void * _in, float ** out, int num_channels, int num_samples)
  {
  int i, j;
  int16_t * in;
  for(i = 0; i < num_channels; i++)
    {
    if(out[i])
      {
      in = ((int16_t*)_in) + i;
      for(j = 0; j < num_samples; j++)
        {
        INT16_TO_FLOAT((*in), out[i][j]);
        in += num_channels;
        }
      }
    }
  }

// useless, because it wont do interleaved sample, and this is what it's all
// about.
void decode_uint8_to_float(void * _in, float ** out, int num_channels, int num_samples)
  {
  int i, j;
  uint8_t * in;
  for(i = 0; i < num_channels; i++)
    {
    if(out[i])
      {
      in = ((uint8_t*)_in) + i;
      for(j = 0; j < num_samples; j++)
        {
        UINT8_TO_FLOAT((*in), out[i][j]);
        in += num_channels;
        }
      }
    }
  }


void crop_format( int src_w, int src_h, float src_aspect, int src_blank,
		int dst_w, int dst_h, float dst_aspect, int dst_blank,
		double& src_rect_x, double& src_rect_y, double& src_rect_w, double& src_rect_h,
       		int& dst_rect_x, int& dst_rect_y, int& dst_rect_w, int& dst_rect_h )
{
		if ( src_aspect < dst_aspect ) { // Skyscraper
			src_rect_x = src_blank;
			src_rect_w = (src_w - 2 * src_blank);
			src_rect_h = src_h / dst_aspect;
			src_rect_y = ( src_h - src_rect_h ) / 2 ;
		} else { // Banner
			src_rect_y = 0.0;
			src_rect_w = (src_w - 2 * src_blank) / dst_aspect;
			src_rect_x = ( src_w - src_rect_w ) / 2;
			src_rect_h = src_h;
		}
		dst_rect_x = dst_blank;
		dst_rect_y = 0;
		dst_rect_w = dst_w - 2 * dst_blank;
		dst_rect_h = dst_h;

}
void fit_format(
		int src_w, int src_h, float src_aspect, int src_blank,
		int dst_w, int dst_h, float dst_aspect, int dst_blank,
		double& src_rect_x, double& src_rect_y, double& src_rect_w, double& src_rect_h,
       		int& dst_rect_x, int& dst_rect_y, int& dst_rect_w, int& dst_rect_h )
{
	if ( src_aspect > dst_aspect ) { // Banner
		src_rect_x = src_blank;
		src_rect_w = (src_w - 2 * src_blank);
		src_rect_h = src_h * dst_aspect;
		src_rect_y = ( src_h - src_rect_h ) / 2 ;
	} else { // Skyscraper
		src_rect_y = 0.0;
		src_rect_w = (src_w - 2 * src_blank) * dst_aspect;
		src_rect_x = ( src_w - src_rect_w ) / 2;
		src_rect_h = src_h;
	}
	dst_rect_x = dst_blank;
	dst_rect_y = 0;
	dst_rect_w = dst_w - 2 * dst_blank;
	dst_rect_h = dst_h;

}
void stretch_format(
		int src_w, int src_h, float /*src_aspect*/, int src_blank,
		int dst_w, int dst_h, float /*dst_aspect*/, int dst_blank,
		double& src_rect_x, double& src_rect_y, double& src_rect_w, double& src_rect_h,
       		int& dst_rect_x, int& dst_rect_y, int& dst_rect_w, int& dst_rect_h )
{
	src_rect_x = src_blank;
	src_rect_y = 0;
	src_rect_w = src_w - 2 * src_blank;
	src_rect_h = src_h;

	dst_rect_x = dst_blank;
	dst_rect_y = 0;
	dst_rect_w = dst_w - 2 * dst_blank;
	dst_rect_h = dst_h;

}


void frame_to_fields( int interlace_order /* 1=top ff, 2=bottom ff */, unsigned
	char* source, unsigned char* destination, int w, int h, bool src_alpha )
{
	
	if ( src_alpha ) {
		uint32_t *src, *dst, *end;
		int h_2 = h/2;
		if ( interlace_order == 1 ) {
			src = (uint32_t*)source;
			dst = (uint32_t*)destination;
		} else {
			src = (uint32_t*)( source + ( w * 4 ) );
			dst = (uint32_t*)destination;
		}
		for ( int y = 0; y < h_2; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				dst[x+y*w] = src[x+y*w*2];
			}
		}
		if ( interlace_order == 1 ) {
			src = (uint32_t*)( source + ( w * 4 ) );
			dst = (uint32_t*)( destination + ( w*h*2 ) );
		} else {
			src = (uint32_t*)source;
			dst = (uint32_t*)( destination + ( w*h*2 ) );
		}
		for ( int y = 0; y < h_2; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				dst[x+y*w] = src[x+y*w*2];
			}
		}
	} else {
		unsigned char *src, *dst, *end;
		int h_2 = h/2;
		if ( interlace_order == 1 ) {
			src = source;
			dst = destination;
		} else {
			src = ( source + ( w * 3 ) );
			dst = destination;
		}
		for ( int y = 0; y < h_2; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				dst[(x+y*w)*4] = src[(x+y*w*2)*3];
				dst[(x+y*w)*4+1] = src[(x+y*w*2)*3+1];
				dst[(x+y*w)*4+2] = src[(x+y*w*2)*3+2];
				dst[(x+y*w)*4+3] = 255;
			}
		}
		if ( interlace_order == 1 ) {
			src = ( source + ( w * 3 ) );
			dst = ( destination + ( w*h*2 ) );
		} else {
			src = source;
			dst = ( destination + ( w*h*2 ) );
		}
		for ( int y = 0; y < h_2; y++ ) {
			for ( int x = 0; x < w; x++ ) {
				dst[(x+y*w)*4] = src[(x+y*w*2)*3];
				dst[(x+y*w)*4+1] = src[(x+y*w*2)*3+1];
				dst[(x+y*w)*4+2] = src[(x+y*w*2)*3+2];
				dst[(x+y*w)*4+3] = 255;
			}
		}
	}
}

void fields_to_frames( int interlace_order /* 1=top ff, 2=bottom ff */, unsigned
	char* source, unsigned char* destination, int w, int h )
{
	uint32_t *src, *dst, *end;
	int h_2 = h/2;
	if ( interlace_order == 1 ) {
		src = (uint32_t*)source;
		dst = (uint32_t*)destination;
	} else {
		src = (uint32_t*)source;
		dst = (uint32_t*)( destination + ( w * 4 ) );
	}
	for ( int y = 0; y < h_2; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			dst[x+y*w*2] = src[x+y*w];
		}
	}
	if ( interlace_order == 1 ) {
		src = (uint32_t*)( source + ( w*h*2 ) );
		dst = (uint32_t*)( destination + ( w * 4 ) );
	} else {
		src = (uint32_t*)( source + ( w*h*2 ) );
		dst = (uint32_t*)destination;
	}
	for ( int y = 0; y < h_2; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			dst[x+y*w*2] = src[x+y*w];
		}
	}

}
void rectangle_crop_aspect(const gavl_rectangle_i_t * dst_rect,
                               const gavl_video_format_t * src_format,
			       gavl_rectangle_f_t * r,
			       const gavl_video_format_t * dst_format)
{
	float dst_display_aspect;
	float src_pixel_aspect;
	float src_display_aspect;

	src_pixel_aspect =
		(float)(src_format->pixel_width) /
		(float)(src_format->pixel_height);

	src_display_aspect =  
		src_pixel_aspect * 
		(float)(src_format->image_width) /
		(float)(src_format->image_height);

	dst_display_aspect =
		dst_rect->w * (float)(dst_format->pixel_width) /
		(dst_rect->h * (float)(dst_format->pixel_height));

	if(dst_display_aspect > src_display_aspect) { /* Crop top and bottom */
		r->w = (float)src_format->image_width;
		r->h = (float)src_format->image_width * src_pixel_aspect / dst_display_aspect;
	} else {  /* Crop left and right */
		r->w = (float)src_format->image_height * dst_display_aspect / src_pixel_aspect;
		r->h = (float)src_format->image_height;
	}
	r->x = (src_format->image_width - r->w)/2;
	r->y = (src_format->image_height - r->h)/2;
}


} /* namespace nle */

