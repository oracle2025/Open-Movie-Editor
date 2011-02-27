//
// "$Id$"
//
// Flmm_Cursor_Shape header file for the FLMM extension to FLTK.
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

#ifndef Flmm_Cursor_Shape_H
#define Flmm_Cursor_Shape_H

#include <FL/Enumerations.H>

class Flmm_Cursor_Shape;
class Fl_Window;

extern void flmm_cursor(Fl_Window*, Flmm_Cursor_Shape*);

typedef struct {
  int version;
  int hotx32, hoty32;
  Fl_Color outline_color32, fill_color32;
  unsigned int and_pattern32[32], xor_pattern32[32];
  int hotx16, hoty16;
  Fl_Color outline_color16, fill_color16;
  unsigned short and_pattern16[16], xor_pattern16[16];
} Flmm_Cursor_Shape_Data;

class Flmm_Cursor_Shape 
{
public:
  Flmm_Cursor_Shape() { init(); }
  Flmm_Cursor_Shape(Flmm_Cursor_Shape_Data&);
  ~Flmm_Cursor_Shape();
  void shape(Flmm_Cursor_Shape_Data&);
  void color(Fl_Color ol, Fl_Color fl);
  Fl_Color outlineColor() { return ol_; }
  Fl_Color fillColor() { return fl_; }
  void *get_handle() { return handle_; }
private:
  Flmm_Cursor_Shape(int hotx, int hoty, unsigned short *andPattern, 
    unsigned short *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
  Flmm_Cursor_Shape(int hotx, int hoty, unsigned int *andPattern, 
    unsigned int *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
  void shape(int hotx, int hoty, unsigned short *andPattern, 
    unsigned short *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
  void shape(int hotx, int hoty, unsigned int *andPattern, 
    unsigned int *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
  static int hgt_, wdt_;
  Fl_Color ol_, fl_;
  void *handle_;
  void init();
};

#endif

//
// End of "$Id$".
//
