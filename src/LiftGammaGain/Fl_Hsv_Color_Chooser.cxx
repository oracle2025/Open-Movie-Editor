
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

#include "Fl_Hsv_Color_Chooser.H"


#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/math.h>
#include <stdio.h>

#define UPDATE_HUE_BOX 1

void Fl_Hsv_Color_Chooser::hsv2rgb(
	double H, double S, double V, double& R, double& G, double& B) {
  if (S < 5.0e-6) {
    R = G = B = V;
  } else {
    int i = (int)H;  
    double f = H - (float)i;
    double p1 = V*(1.0-S);
    double p2 = V*(1.0-S*f);
    double p3 = V*(1.0-S*(1.0-f));
    switch (i) {
    case 0: R = V;   G = p3;  B = p1;  break;
    case 1: R = p2;  G = V;   B = p1;  break;
    case 2: R = p1;  G = V;   B = p3;  break;
    case 3: R = p1;  G = p2;  B = V;   break;
    case 4: R = p3;  G = p1;  B = V;   break;
    case 5: R = V;   G = p1;  B = p2;  break;
    }
  }
}

void Fl_Hsv_Color_Chooser::rgb2hsv(
	double R, double G, double B, double& H, double& S, double& V) {
  double maxv = R > G ? R : G; if (B > maxv) maxv = B;
  V = maxv;
  if (maxv>0) {
    double minv = R < G ? R : G; if (B < minv) minv = B;
    S = 1.0 - double(minv)/maxv;
    if (maxv > minv) {
      if (maxv == R) {H = (G-B)/double(maxv-minv); if (H<0) H += 6.0;}
      else if (maxv == G) H = 2.0+(B-R)/double(maxv-minv);
      else H = 4.0+(R-G)/double(maxv-minv);
    }
  }
}

enum {M_RGB, M_BYTE, M_HEX, M_HSV}; // modes
static Fl_Menu_Item mode_menu[] = {
  {"rgb"},
  {"byte"},
  {"hex"},
  {"hsv"},
  {0}
};

int Flcc_Hsv_Value_Input::format(char* buf) {
  Fl_Hsv_Color_Chooser* c = (Fl_Hsv_Color_Chooser*)parent();
  if (c->mode() == M_HEX) return sprintf(buf,"0x%02X", int(value()));
  else return Fl_Valuator::format(buf);
}

void Fl_Hsv_Color_Chooser::set_valuators() {
  switch (mode()) {
  case M_RGB:
    rvalue.range(0,1); rvalue.step(1,1000); rvalue.value(r_);
    gvalue.range(0,1); gvalue.step(1,1000); gvalue.value(g_);
    bvalue.range(0,1); bvalue.step(1,1000); bvalue.value(b_);
    break;
  case M_BYTE:
  case M_HEX:
    rvalue.range(0,255); rvalue.step(1); rvalue.value(int(255*r_+.5));
    gvalue.range(0,255); gvalue.step(1); gvalue.value(int(255*g_+.5));
    bvalue.range(0,255); bvalue.step(1); bvalue.value(int(255*b_+.5));
    break;
  case M_HSV:
    rvalue.range(0,6); rvalue.step(1,1000); rvalue.value(hue_);
    gvalue.range(0,1); gvalue.step(1,1000); gvalue.value(saturation_);
    bvalue.range(0,1); bvalue.step(1,1000); bvalue.value(value_);
    break;
  }
}

int Fl_Hsv_Color_Chooser::rgb(double R, double G, double B) {
  if (R == r_ && G == g_ && B == b_) return 0;
  r_ = R; g_ = G; b_ = B;
  double ph = hue_;
  double ps = saturation_;
  double pv = value_;
  rgb2hsv(R,G,B,hue_,saturation_,value_);
  set_valuators();
  set_changed();
  if (value_ != pv) {
#ifdef UPDATE_HUE_BOX
    huebox.damage(FL_DAMAGE_SCROLL);
#endif
    valuebox.damage(FL_DAMAGE_EXPOSE);}
  if (hue_ != ph || saturation_ != ps) {
    huebox.damage(FL_DAMAGE_EXPOSE); 
    valuebox.damage(FL_DAMAGE_SCROLL);
  }
  return 1;
}

int Fl_Hsv_Color_Chooser::hsv(double H, double S, double V) {
  H = fmod(H,6.0); if (H < 0.0) H += 6.0;
  if (S < 0.0) S = 0.0; else if (S > 1.0) S = 1.0;
  if (V < 0.0) V = 0.0; else if (V > 1.0) V = 1.0;
  if (H == hue_ && S == saturation_ && V == value_) return 0;
  double ph = hue_;
  double ps = saturation_;
  double pv = value_;
  hue_ = H; saturation_ = S; value_ = V;
  if (value_ != pv) {
#ifdef UPDATE_HUE_BOX
    huebox.damage(FL_DAMAGE_SCROLL);
#endif
    valuebox.damage(FL_DAMAGE_EXPOSE);}
  if (hue_ != ph || saturation_ != ps) {
    huebox.damage(FL_DAMAGE_EXPOSE); 
    valuebox.damage(FL_DAMAGE_SCROLL);
  }
  hsv2rgb(H,S,V,r_,g_,b_);
  set_valuators();
  set_changed();
  return 1;
}

Fl_Hsv_Color_Chooser::Fl_Hsv_Color_Chooser(int X, int Y, int W, int H, const char* L)
  : Fl_Group(0,0,195,115,L),
    huebox(0,0,115,115),
    valuebox(115,0,20,115),
    choice(140,0,55,25),
    rvalue(140,30,55,25),
    gvalue(140,60,55,25),
    bvalue(140,90,55,25),
    resize_box(0,0,115,115)
{
  end();
  resizable(resize_box);
  resize(X,Y,W,H);
  r_ = g_ = b_ = 0;
  hue_ = 0.0;
  saturation_ = 0.0;
  value_ = 0.0;
  huebox.box(FL_DOWN_FRAME);
  valuebox.box(FL_DOWN_FRAME);
  choice.menu(mode_menu);
  set_valuators();
  rvalue.callback(rgb_cb);
  gvalue.callback(rgb_cb);
  bvalue.callback(rgb_cb);
  choice.callback(mode_cb);
  choice.box(FL_THIN_UP_BOX);
  choice.textfont(FL_HELVETICA_BOLD_ITALIC);
  choice.value(3);
}

void Fl_Hsv_Color_Chooser::rgb_cb(Fl_Widget* o, void*) {
  Fl_Hsv_Color_Chooser* c = (Fl_Hsv_Color_Chooser*)(o->parent());
  double R = c->rvalue.value();
  double G = c->gvalue.value();
  double B = c->bvalue.value();
  if (c->mode() == M_HSV) {
    if (c->hsv(R,G,B)) c->do_callback();
    return;
  }
  if (c->mode() != M_RGB) {
    R = R/255;
    G = G/255;
    B = B/255;
  }
  if (c->rgb(R,G,B)) c->do_callback();
}

void Fl_Hsv_Color_Chooser::mode_cb(Fl_Widget* o, void*) {
  Fl_Hsv_Color_Chooser* c = (Fl_Hsv_Color_Chooser*)(o->parent());
  // force them to redraw even if value is the same:
  c->rvalue.value(-1);
  c->gvalue.value(-1);
  c->bvalue.value(-1);
  c->set_valuators();
}
////////////////////////////////////////////////////////////////
