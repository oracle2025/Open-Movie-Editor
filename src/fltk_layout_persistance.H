

#ifndef _FLTK_LAYOUT_PERSISTANCE_H_
#define _FLTK_LAYOUT_PERSISTANCE_H_

#include <FL/Fl_Preferences.H>
#include <FL/Fl_Group.H>

void fltk_save_layout( Fl_Preferences* p, Fl_Group* g );
void fltk_load_layout( Fl_Preferences* p, Fl_Group* g);

#endif /* _FLTK_LAYOUT_PERSISTANCE_H_ */
