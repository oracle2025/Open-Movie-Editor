//
// "$Id:$"
//
// Flmm_Cursor_Shape implementation file for the FLMM extension to FLTK.
//
// Copyright 2002-2004 by Matthias Melcher.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "flmm@matthiasm.com".
//


/** \function void flmm_cursor(Fl_Window*, Flmm_Cursor_Shape*)
 * \warning class not documented yet!
 */

/** \class Flmm_Cursor_Shape
 * This class manages custom created mouse pointer graphics.
 *
 * Use this class to generate an operating system specific 
 * data structure out of the cross platform data set Flmm_Cursor_Shape_Data. 
 * The claa flmm_cursor() then changes the mouse pointer in the same
 * way as the core fl_cursor() does.
 *
 * \image html yingyang.png "Cursor Shape Editor"
 */

/** \struct Flmm_Cursor_Shape_Data
 * This struct holds the graphical information for two color 16x16 
 * and 32x32 pixel mouse pointer shapes. Since creating this struct
 * by hand involves a lot of counting an typing, I recommend the 
 * cursor_editor aplication in the test subdirectory to create 
 * Flmm_Cursor_Shape_Data structures.
 *
 * Both 32 and 16 pixel large cursors should be created to support 
 * all possible window managers and operating systems.
 */

/** \fn Flmm_Cursor_Shape::get_handle()
 * Return the OS specific handle to a cursor shape.
 */

/** \fn Flmm_Cursor_Shape::outlineColor()
 * Deprecated.
 * \deprecated Cursor color is not supported anymore.
 */

/** \fn Flmm_Cursor_Shape::fillColor()
 * Deprecated.
 * \deprecated Cursor color is not supported anymore.
 */

/** \fn Flmm_Cursor_Shape::color(Fl_Color, Fl_Color)
 * Deprecated.
 * \deprecated Cursor color is not supported anymore.
 */

// The "and" and "xor" members should represent bit arrays with the bit meaning
// and xor
//-------------
//  0   0  outline color
//  0   1  fill color
//  1   0  transparent
//  1   1  don't use

//#define SWAP_BYTES

#include "Flmm_Cursor_Shape.H"
#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/x.H>

#ifdef WIN32
# include <windows.h>
# include <stdlib.h>
# define HCursor HCURSOR
#elif defined(__APPLE__)
# include <Carbon/Carbon.h>
# define HCursor CursPtr
#else
# include <stdlib.h>
# include <string.h>
# include <X11/Xlib.h>
# include <X11/cursorfont.h> 
# define HCursor ::Cursor
#endif

/**
 * Activate this cursor shape for the given application.
 *
 * \param win top level window
 * \param user defined cursor shape
 */
void flmm_cursor(Fl_Window *win, Flmm_Cursor_Shape *c) {
  if ( !win ) return;
  if ( !c ) return;
#ifdef WIN32
  Fl_X::i(win)->cursor = (HCursor)c->get_handle(); 
  SetCursor( (HCursor)c->get_handle() );
#elif defined(__APPLE__)
  SetCursor( (CursPtr)c->get_handle() );
#else
  XDefineCursor( fl_display, fl_xid(win), (HCursor)c->get_handle() );
#endif
}

//|
//| we need to swap bytes if we are on a LSB-First machine. The only way to test this at
//| compile time is through preprocessor defines which are different for different compilers.
//| Runtime checks would slow these routines down pretty much... .
//| 

// Intel requires us to swap bits!
#if defined(__APPLE__) || defined(WIN32)
# define SB(x) x
#else
  static uchar hiNibble[16] =
  { 0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0 };
  static uchar loNibble[16] =
  { 0x00, 0x08, 0x04, 0x0c, 0x02, 0x0a, 0x06, 0x0e,
    0x01, 0x09, 0x05, 0x0d, 0x03, 0x0b, 0x07, 0x0f };
# define SB(b) ( ( hiNibble[(b)&15] ) | ( loNibble[((b)>>4)&15] ) )
#endif

#ifdef __APPLE__
#  define intelSwap32(x) x
#  define intelSwap16(x) x
#elif !WORDS_BIGENDIAN 
  static unsigned short intelSwap16(unsigned short x) { 
    return (SB(x>>8)|(SB(x&255)<<8));
  } 
  static unsigned int intelSwap32(unsigned int& x) { 
    unsigned char* p = (unsigned char*)(&x);
    return (SB(p[0])<<24)|(SB(p[1])<<16)|(SB(p[2])<<8)|SB(p[3]);
  }
#else
static unsigned int intelSwap32(unsigned int x) { 
  union { unsigned int v; unsigned char d[4]; };
  v = x;
  d[0] = SB( d[0] );
  d[1] = SB( d[1] );
  d[2] = SB( d[2] );
  d[3] = SB( d[3] );
  return v;
} 
static unsigned short intelSwap16(unsigned short x) { 
  union { unsigned short v; unsigned char d[2]; };
  v = x;
  d[0] = SB( d[0] );
  d[1] = SB( d[1] );
  return v;
} 
#endif

//|________________________________________________________________
//| machine independent part

//|
//| initialize the cursor to 16x16 2 colors + mask
//|
Flmm_Cursor_Shape::Flmm_Cursor_Shape( int hotx, int hoty, 
    unsigned short *andPattern, unsigned short *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  init();
  shape(hotx, hoty, andPattern, xorPattern, ol, fl);
}


//|
//| initialize the cursor to 32x32 2 colors + mask
//|
Flmm_Cursor_Shape::Flmm_Cursor_Shape( int hotx, int hoty, 
  unsigned int *andPattern, unsigned int *xorPattern, 
  Fl_Color ol, Fl_Color fl) 
{
  init();
  shape(hotx, hoty, andPattern, xorPattern, ol, fl);
}

/**
 * Create an operating system specific data structure from data.
 *
 * \param d cursor shape data
 */
Flmm_Cursor_Shape::Flmm_Cursor_Shape(Flmm_Cursor_Shape_Data &d) {
  init();
  shape(d);
}

/**
 * Create an operating system specific data structure from data.
 *
 * \param d cursor shape data
 */
void Flmm_Cursor_Shape::shape(Flmm_Cursor_Shape_Data &d) {
#ifdef WIN32
  // check Windows for the support of 16x16 and 32x32 cursors
  shape(d.hotx32, d.hoty32, d.and_pattern32, d.xor_pattern32,
        d.outline_color32, d.fill_color32);
#elif defined(__APPLE__)
  // check OS X for the support of 16x16 and 32x32 cursors
  shape(d.hotx16, d.hoty16, d.and_pattern16, d.xor_pattern16,
        d.outline_color16, d.fill_color16);
#else
  // check X11 for the support of 16x16 and 32x32 cursors
  shape(d.hotx32, d.hoty32, d.and_pattern32, d.xor_pattern32,
        d.outline_color32, d.fill_color32);
#endif
}

//|
//| preset everything nicely
//|
void Flmm_Cursor_Shape::init() {
  handle_ = 0L;
  ol_ = FL_BLACK;
  fl_ = FL_WHITE;
}


//|________________________________________________________________
#ifdef _WIN32
//| windows part

//|
//| Windows 95/98 supports only one single cursor size (usually 32x32)
//| Windows also supports full color cursors which I will ignore here for now
//|
int Flmm_Cursor_Shape::wdt_ = GetSystemMetrics(SM_CXCURSOR);
int Flmm_Cursor_Shape::hgt_ = GetSystemMetrics(SM_CYCURSOR);


//| WIN32:
//| set or change the cursor to 16x16 2 colors + mask
//|
void Flmm_Cursor_Shape::shape( int hotx, int hoty, 
    unsigned short *andPattern, unsigned short *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl;
  int x = (wdt_+31)>>5; x = x<<1;
  unsigned short *And = (unsigned short*)malloc(x*hgt_*2); memset(And, 0xff, x*hgt_*2);
  unsigned short *Xor = (unsigned short*)calloc(x*hgt_, 2);
  unsigned short *dAnd = And, *dXor = Xor, *sAnd = andPattern, *sXor = xorPattern;
  for (int i=0; i<16; i++) {
    *dAnd = intelSwap16(*sAnd++); dAnd += x;
    *dXor = intelSwap16(*sXor++); dXor += x;
  }
  handle_ = CreateCursor(fl_display, hotx, hoty, wdt_, hgt_, And, Xor);
  free(And);
  free(Xor);
  color(ol, fl); //: ignored on WIN32 anyway
}


//| WIN32:
//| set or change the cursor to 32x32 2 colors + mask
//|
void Flmm_Cursor_Shape::shape( int hotx, int hoty, 
    unsigned int *andPattern, unsigned int *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl;
  int x = (wdt_+31)>>5; x = x;	//: wdt_ can be any value divedable by int boundaries (32)
  unsigned int *And = (unsigned int*)malloc(x*hgt_*4); memset(And, 0xff, x*hgt_*4);
  unsigned int *Xor = (unsigned int*)calloc(x*hgt_, 4);
  unsigned int *dAnd = And, *dXor = Xor, *sAnd = andPattern, *sXor = xorPattern;
  for (int i=0; i<32; i++) {
    *dAnd = intelSwap32(*sAnd++); dAnd += x;
    *dXor = intelSwap32(*sXor++); dXor += x;
  }
  handle_ = CreateCursor(fl_display, hotx, hoty, wdt_, hgt_, And, Xor);
  free(And);
  free(Xor);
  color(ol, fl); //: ignored on Win32 anyway
}


//| WIN32:
//| destroy the cursor shape and remove us from the list
//|
Flmm_Cursor_Shape::~Flmm_Cursor_Shape() {
  if (handle_) DestroyCursor((HCursor)handle_);
}


//| WIN32:
//| just copy the values... .
//|
void Flmm_Cursor_Shape::color(Fl_Color ol, Fl_Color fl) { ol_ = ol; fl_ = fl; }

//|________________________________________________________________
#elif defined(__APPLE__)
//| Mac OS Carbon part

int Flmm_Cursor_Shape::wdt_ = 16;	//: how do we determine the maximum size in X? Can we use any size? Up to some size?
int Flmm_Cursor_Shape::hgt_ = 16;


//| Mac OS Carbon:
//| set or change the cursor to 16x16 2 colors + mask
//|
void Flmm_Cursor_Shape::shape( int hotx, int hoty, 
    unsigned short *andPattern, unsigned short *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  if (!handle_)
    handle_ = (CursPtr)calloc(1,sizeof(Cursor));
  ol_ = ol; fl_ = fl;
  unsigned short dA, dX;
  for (int i=0; i<16; i++) {
    dA = *andPattern++;
    dX = *xorPattern++;
    ((CursPtr)handle_)->data[i] = ~( dA | dX);   // smart, huh? ;-)
    ((CursPtr)handle_)->mask[i] = ~ dA;
  }
  ((CursPtr)handle_)->hotSpot.h = hotx;
  ((CursPtr)handle_)->hotSpot.v = hoty;
}


//| Mac OS Carbon:
//| set or change the cursor to 32x32 2 colors + mask
//|
void Flmm_Cursor_Shape::shape( int hotx, int hoty, 
    unsigned int *andPattern, unsigned int *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  if (!handle_)
    handle_ = (CursPtr)calloc(1,sizeof(Cursor));
  ol_ = ol; fl_ = fl;
  unsigned int And[32];
  unsigned int Xor[32];
  unsigned int *dAnd = And, *dXor = Xor, dA, dX;
  for (int i=0; i<32; i++) {
    dA = *andPattern++;
    dX = *xorPattern++;
    *dAnd++ = ~( dA | dX);   // smart, huh? ;-)
    *dXor++ = ~ dA;
  }
  for (int i=0; i<16; i++) {
    ((CursPtr)handle_)->data[i] = And[i]>>16;
    ((CursPtr)handle_)->mask[i] = Xor[i]>>16;
  }
  ((CursPtr)handle_)->hotSpot.h = hotx;
  ((CursPtr)handle_)->hotSpot.v = hoty;
}


//| Mac OS Carbon: 
//| destroy the cursor shape and remove us from the list
//|
Flmm_Cursor_Shape::~Flmm_Cursor_Shape() {
  if (handle_) free(handle_);
}


void Flmm_Cursor_Shape::color(Fl_Color fg, Fl_Color bg)
{ 
  ol_ = fg; 
  fl_ = bg; 
}

//|________________________________________________________________
#else 
//| X11 part

int Flmm_Cursor_Shape::wdt_ = 32;	//: how do we determine the maximum size in X? Can we use any size? Up to some size?
int Flmm_Cursor_Shape::hgt_ = 32;


//| XWindows:
//| set or change the cursor to 16x16 2 colors + mask
//|
void Flmm_Cursor_Shape::shape( int hotx, int hoty, 
    unsigned short *andPattern, unsigned short *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl; 
  unsigned short And[16];
  unsigned short Xor[16];
  unsigned short *dAnd = And, *dXor = Xor, dA, dX;
  for (int i=0; i<16; i++) { 
    dA = intelSwap16(*andPattern++);
    dX = intelSwap16(*xorPattern++);
    *dAnd++ = ~( dA | dX);   // smart, huh? ;-)
    *dXor++ = ~ dA;
  } 
  XColor dummy; 
  Pixmap andMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen), 
                (char*)And, 16, 16); 
  Pixmap xorMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen), 
                (char*)Xor, 16, 16); 
  if ( handle_ ) XFreeCursor( fl_display, (HCursor)handle_ );
  handle_ = (void*)XCreatePixmapCursor(fl_display, andMap, xorMap, &dummy, &dummy, hotx, hoty); 
  XFreePixmap(fl_display, andMap); 
  XFreePixmap(fl_display, xorMap); 
  color(ol, fl);
}


//| XWindows:
//| set or change the cursor to 32x32 2 colors + mask
//|
void Flmm_Cursor_Shape::shape( int hotx, int hoty, 
    unsigned int *andPattern, unsigned int *xorPattern, 
    Fl_Color ol, Fl_Color fl) 
{
  ol_ = ol; fl_ = fl;
  unsigned int And[32];
  unsigned int Xor[32];
  unsigned int *dAnd = And, *dXor = Xor, dA, dX;
  for (int i=0; i<32; i++) {
    dA = intelSwap32(*andPattern++);
    dX = intelSwap32(*xorPattern++);
    *dAnd++ = ~( dA | dX);   // smart, huh? ;-)
    *dXor++ = ~ dA;
  }
  XColor dummy;
  Pixmap andMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen),
                (char*)And, 32, 32);
  Pixmap xorMap = XCreateBitmapFromData(fl_display, RootWindow(fl_display, fl_screen),
                (char*)Xor, 32, 32);
  if ( handle_ ) XFreeCursor( fl_display, (HCursor)handle_ );
  handle_ = (void*)XCreatePixmapCursor(fl_display, andMap, xorMap, &dummy, &dummy, hotx, hoty);
  XFreePixmap(fl_display, andMap);
  XFreePixmap(fl_display, xorMap);
  color(ol, fl);
}


//| XWindows: 
//| destroy the cursor shape and remove us from the list
//|
Flmm_Cursor_Shape::~Flmm_Cursor_Shape() {
  if (handle_) XFreeCursor(fl_display, (HCursor)handle_);
}


//| XWindows:
//| recolor the cursor
//|
void Flmm_Cursor_Shape::color(Fl_Color fg, Fl_Color bg) {
  if ( !handle_ ) return;
  XColor fgc;
  uchar r,g,b;
  Fl::get_color(fg,r,g,b);
  fgc.red = r<<8; fgc.green = g<<8; fgc.blue = b<<8;
  XColor bgc;
  Fl::get_color(bg,r,g,b);
  bgc.red = r<<8; bgc.green = g<<8; bgc.blue = b<<8;
  XRecolorCursor(fl_display, (HCursor)handle_, &fgc, &bgc);
}

#endif

//
// End of "$Id:$".
//
