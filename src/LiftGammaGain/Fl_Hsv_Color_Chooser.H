

/* Modified Color Chooser Widget by Richard Spindler,
 * Original Copyright: See below
 */

// Color chooser for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
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

#ifndef _FL_HSV_COLOR_CHOSER_H_
#define _FL_HSV_COLOR_CHOSER_H_

#include <FL/Fl_Color_Chooser.H>

class Flcc_Hsv_Value_Input : public Fl_Value_Input {
public:
  int format(char*);
  Flcc_Hsv_Value_Input(int X, int Y, int W, int H) : Fl_Value_Input(X,Y,W,H) {}
};

class Fl_Hsv_Color_Chooser : public Fl_Group {
  Flcc_HueBox huebox;
  Flcc_ValueBox valuebox;
  Fl_Choice choice;
  Flcc_Hsv_Value_Input rvalue;
  Flcc_Hsv_Value_Input gvalue;
  Flcc_Hsv_Value_Input bvalue;
  Fl_Box resize_box;
  double hue_, saturation_, value_;
  double r_, g_, b_;
  void set_valuators();
  static void rgb_cb(Fl_Widget*, void*);
  static void mode_cb(Fl_Widget*, void*);
public:
  int mode() {return choice.value();}
  double hue() const {return hue_;}
  double saturation() const {return saturation_;}
  double value() const {return value_;}
  double r() const {return r_;}
  double g() const {return g_;}
  double b() const {return b_;}
  int hsv(double,double,double);
  int rgb(double,double,double);
  static void hsv2rgb(double, double, double,double&,double&,double&);
  static void rgb2hsv(double, double, double,double&,double&,double&);
  Fl_Hsv_Color_Chooser(int,int,int,int,const char* = 0);
};

#endif /* _FL_HSV_COLOR_CHOSER_H_ */
