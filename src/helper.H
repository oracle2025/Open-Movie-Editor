/*  helper.H
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

#ifndef _HELPER_H_
#define _HELPER_H_

#include <stdint.h>
#include <gavl/gavl.h>

namespace nle
{

const char* pixel_aspect_ratio_to_string( float pixel_aspect_ratio );
float string_to_pixel_aspect_ratio( const char* par_str );

const char* timestamp_to_string( int64_t timestamp );
int mkdirp( const char* pathname );
void findpath( const char* filename, char* buffer, int bufferlen );
void secs2HMS( double secs, int &H, int &M, int &S ); /* copied from FLU */

const char* timestamp_to_smil_string( int64_t timestamp, int frame_length = 1 );

unsigned long gcd (unsigned long a, unsigned long b);

void guess_aspect( int w, int h, int* aspect_height, int* aspect_width, float* aspect_ratio, int* analog_blank, int* pixel_width, int* pixel_height );

void resizePixels( const unsigned char* in, unsigned char* out, int w1, int h1, int w2, int h2 );

} /* namespace nle */


#endif /* _HELPER_H_ */

