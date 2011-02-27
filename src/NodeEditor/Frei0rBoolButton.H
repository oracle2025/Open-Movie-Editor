#ifndef _FREI0R_BOOL_BUTTON_H_
#define _FREI0R_BOOL_BUTTON_H_

#include <FL/Fl.H>
#include <FL/Fl_Light_Button.H>
#include "frei0r.h"

typedef void (*f0r_set_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);
typedef void (*f0r_get_param_value_f)(f0r_instance_t instance, f0r_param_t param, int param_index);  


class Frei0rBoolButton : public Fl_Light_Button
{
	public:
		Frei0rBoolButton( int x, int y, int w, int h, const char *l = 0 );
		~Frei0rBoolButton();
		void toggle_value();
		void set_instance( f0r_instance_t i, f0r_set_param_value_f g, f0r_set_param_value_f s, int param_index );
	private:
		f0r_instance_t m_instance;
		f0r_set_param_value_f f0r_set_param_value;
		f0r_get_param_value_f f0r_get_param_value;
		int m_param_index;	
};

#endif /* _FREI0R_BOOL_BUTTON_H_ */
