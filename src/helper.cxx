/*  helper.cxx
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

#include "config.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "globals.H"
#include "helper.H"
#include "fps_definitions.H"

#define BUFFER_LEN 1024

namespace nle
{

const char* pixel_aspect_ratio_to_string( float pixel_aspect_ratio )
{
	static char buffer[255];
	snprintf( buffer, 255, "%.5f", pixel_aspect_ratio );
	buffer[254] = '\0';
	return buffer;
}
float string_to_pixel_aspect_ratio( const char* par_str )
{
	char buffer[255];
	strncpy( buffer, par_str, 255 );
	buffer[254] = '\0';
	char* p = buffer;
	while ( *p != ' ' && *p != '\0' ) {
		p++;
	}
	*p = '\0';
	return atof( buffer );
}

const char* timestamp_to_string( int64_t timestamp )
{
	static char buffer[256];
	int hours = (int) ( timestamp / NLE_TIME_BASE / 60 / 60 );
	int minutes = (int) ( ( timestamp / ( NLE_TIME_BASE * 60 ) ) ) % 60;
	int seconds = (int) ( ( timestamp / NLE_TIME_BASE ) ) % 60;
	snprintf( buffer, 256, "%02d:%02d:%02d", hours, minutes, seconds );
	return buffer;
}
const char* timestamp_to_smil_string( int64_t timestamp, int frame_length )
{
	static char buffer[256];
	int hours = (int) ( timestamp / NLE_TIME_BASE / 60 / 60 );
	int minutes = (int) ( ( timestamp / ( NLE_TIME_BASE * 60 ) ) ) % 60;
	int seconds = (int) ( ( timestamp / NLE_TIME_BASE ) ) % 60;
	int ticks = (int) ( ( timestamp * 1000 / NLE_TIME_BASE ) ) % 1000 / frame_length * frame_length;
	snprintf( buffer, 256, "%02d:%02d:%02d.%03d", hours, minutes, seconds, ticks );
	return buffer;
}

int mkdirp( const char* pathname )
{
	char* p = (char*)pathname;
	int len = strlen(pathname);
	char buffer[BUFFER_LEN];
	struct stat statbuf;
	strncpy( buffer, pathname, BUFFER_LEN );
	while ( p - pathname < len ) {
		buffer[p - pathname] = pathname[p - pathname];
		p++;
		while ( *p != '/' && p - pathname < len )
			p++;
		strncpy( buffer, pathname, BUFFER_LEN/*p - pathname*/ );
		buffer[p - pathname] = '\0';
		if ( stat( buffer, &statbuf ) == -1 && errno == ENOENT ) {
			mkdir( buffer, 0700 );
		}
		
	}
	return 0;
}
void findpath( const char* filename, char* buffer, int bufferlen )
{
	char* p = (char*)filename;
	int len = strlen(filename);
	struct stat statbuf;
	char buffer2[BUFFER_LEN];
	strncpy( buffer2, filename, BUFFER_LEN );
	while( p - filename < len ) {
		buffer2[p - filename] = filename[p - filename];
		p++;
		while ( *p != '/' && p - filename < len )
			p++;
		buffer2[p - filename] = '\0';
		if ( stat( buffer2, &statbuf ) != -1 ) {
			if ( !S_ISDIR( statbuf.st_mode ) ) {
				return;
			}
		}
		strncpy( buffer, buffer2, bufferlen );
	}
	return;
}

void secs2HMS( double secs, int &H, int &M, int &S )
{
  S = (int)secs;
  M = S / 60; S -= M*60;
  H = M / 60; M -= H*60;
}

/*
http://encodingwissen.de/spezial/itur-bt601.html:
Exaktes PAR nach ITU-R BT.601
        PAL         NTSC
4:3     128/117     4320/4739
16:9    512/351     5760/4739
        720×576     720x480

PAR nach MPEG-4
        PAL     NTSC
4:3     12/11   10/11
16:9    16/11   40/33

NTSC: 704x480  ->  4320/4739
PAL: 768x576
See:
http://en.wikipedia.org/wiki/ATSC_Standards
http://en.wikipedia.org/wiki/Aspect_ratio_(image)
Handy Reference Table:
http://www.mir.com/DMG/aspect.html
http://lipas.uwasa.fi/~f76998/video/conversion/#conversion_table
*/



/* The following code was taken from gettext-0.14.6/gettext-tools/lib/gcd.c */

/* Arithmetic.
   Copyright (C) 2001-2002 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

unsigned long
gcd (unsigned long a, unsigned long b)
{
  /* Why no division, as in Euclid's algorithm? Because in Euclid's algorithm
     the division result floor(a/b) or floor(b/a) is very often = 1 or = 2,
     and nearly always < 8.  A sequence of a few subtractions and tests is
     faster than a division.  */
  /* Why not Euclid's algorithm? Because the two integers can be shifted by 1
     bit in a single instruction, and the algorithm uses fewer variables than
     Euclid's algorithm.  */

  unsigned long c = a | b;
  c = c ^ (c - 1);
  /* c = largest power of 2 that divides a and b.  */

  if (a & c)
    {
      if (b & c)
	goto odd_odd;
      else
	goto odd_even;
    }
  else
    {
      if (b & c)
	goto even_odd;
      else
	abort ();
    }

  for (;;)
    {
    odd_odd: /* a/c and b/c both odd */
      if (a == b)
	break;
      if (a > b)
	{
	  a = a - b;
	even_odd: /* a/c even, b/c odd */
	  do
	    a = a >> 1;
	  while ((a & c) == 0);
	}
      else
	{
	  b = b - a;
	odd_even: /* a/c odd, b/c even */
	  do
	    b = b >> 1;
	  while ((b & c) == 0);
	}
    }

  /* a = b */
  return a;
}

void resizePixels( const unsigned char* in, unsigned char* out, int w1, int h1, int w2, int h2 )
{
	int x_ratio = (int)( ( w1 << 16 ) / w2 );
	int y_ratio = (int)( ( h1 << 16 ) / h2);
	int x2, y2;
	for ( int i = 0; i < h2; i++ ) {
		for ( int j = 0; j < w2; j++ ) {
			x2 = ( ( j * x_ratio ) >> 16 );
			y2 = ( (i * y_ratio ) >> 16 );
			out[ ( ( i * w2 ) + j ) * 3 ] = in[ ( ( y2 * w1 ) + x2 ) * 3 ];
			out[ ( ( i * w2 ) + j ) * 3 + 1 ] = in[ ( ( y2 * w1 ) + x2 ) * 3 + 1 ];
			out[ ( ( i * w2 ) + j ) * 3 + 2 ] = in[ ( ( y2 * w1 ) + x2 ) * 3 + 2 ];
		}                
	}                
}

} /* namespace nle */
