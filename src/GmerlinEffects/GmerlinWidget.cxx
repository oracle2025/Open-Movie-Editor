/* GmerlinWidget.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include "GmerlinWidget.H"
#include "GmerlinEffect.H"
#include "VideoViewGL.H"

#include <FL/Fl.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Color_Chooser.H>

namespace nle
{

static void spin_check_callback( Fl_Widget* i, void* v )
{
	Fl_Check_Button* vi = dynamic_cast<Fl_Check_Button*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	val.val_i = vi->value();
	info->widget->setValue( info->name, info->info, &val );
}
static void spin_int_callback( Fl_Widget* i, void* v )
{
	Fl_Spinner* vi = dynamic_cast<Fl_Spinner*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	val.val_i = vi->value();
	info->widget->setValue( info->name, info->info, &val );
}
static void spin_float_callback( Fl_Widget* i, void* v )
{
	Fl_Spinner* vi = dynamic_cast<Fl_Spinner*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	val.val_f = vi->value();
	info->widget->setValue( info->name, info->info, &val );
}
static void slider_int_callback( Fl_Widget* i, void* v )
{
	Fl_Slider* vi = dynamic_cast<Fl_Slider*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	val.val_f = vi->value();
	info->widget->setValue( info->name, info->info, &val );
}
static void slider_float_callback( Fl_Widget* i, void* v )
{
	Fl_Slider* vi = dynamic_cast<Fl_Slider*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	val.val_f = vi->value();
	info->widget->setValue( info->name, info->info, &val );
}

static void string_list_menu_callback( Fl_Menu_* o, void* v )
{
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)o->user_data();
	char* name = (char*)v;
	bg_parameter_value_t val;
	val.val_str = name;
	info->widget->setValue( info->name, info->info, &val );
}
static void input_string_callback( Fl_Widget* i, void* v )
{
	Fl_Input* vi = dynamic_cast<Fl_Input*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	val.val_str = (char*)vi->value();
	info->widget->setValue( info->name, info->info, &val );
}
static void color_rgb_callback( Fl_Widget* i, void* v )
{
	Fl_Button* vi = dynamic_cast<Fl_Button*>(i);
	gmerlin_widget_callback_info* info = (gmerlin_widget_callback_info*)v;
	bg_parameter_value_t val;
	double r, g, b;
	if ( fl_color_chooser( "Choose Color", r, g, b ) ) {
		val.val_color[0] = r;
		val.val_color[1] = g;
		val.val_color[2] = b;
		val.val_color[3] = 1.0; //alpha
		vi->color( fl_rgb_color( (uchar)( r * 255 ), (uchar)( g * 255), (uchar)( b * 255 ) ) );
		vi->labelcolor( fl_contrast( FL_FOREGROUND_COLOR, vi->color() ) );
		info->widget->setValue( info->name, info->info, &val );
	}
}

GmerlinWidget::GmerlinWidget( GmerlinEffect* effect )
	: IEffectWidget( 0, 0, 340, effect->numberOfParams() * 25 + 5 )
{
	m_effect = effect;
	m_filter = m_effect->filter();
	m_filter_instance = m_effect->instance();
	m_infostack = new gmerlin_widget_callback_info[effect->numberOfParams()];
	const bg_parameter_info_t* parameters = m_filter->common.get_parameters( m_filter_instance );
	for ( int i = 0; parameters[i].name; i++ ) {
		int x, y, w, h;
		w = 225;
		h = 20;
		x = 100;
		y = 5 + ( i * 25 );
		m_infostack[i].widget = this;
		m_infostack[i].name = parameters[i].name;
		m_infostack[i].info = &parameters[i];
		switch ( parameters[i].type ) {
			case BG_PARAMETER_SECTION:
				break;
			case BG_PARAMETER_CHECKBUTTON:
			{
				Fl_Check_Button* b = new Fl_Check_Button( x, y, w, h, parameters[i].long_name );
				b->labelsize( 12 );
				b->callback( spin_check_callback, &(m_infostack[i]) );
				b->tooltip( parameters[i].help_string );
				bg_parameter_value_t val;
				bg_cfg_section_get_parameter( m_effect->config(), &parameters[i], &val );
				b->value( val.val_i );
				/*
				//bg_cfg_section_get_parameter (bg_cfg_section_t *section, const bg_parameter_info_t *info, bg_parameter_value_t *value)

				bg_parameter_value_t val;
				m_filter->common.get_parameter( m_filter_instance, parameters[i].name, &val );
				b->value( val.val_i );
				*/
				//b->value( parameters[i].val_default.val_i );
				break;
			}
			case BG_PARAMETER_INT :
			{
				Fl_Spinner* o = new Fl_Spinner( x, y, w, h, parameters[i].long_name );
				o->labelsize( 12 );
				o->type( FL_INT_INPUT );
				o->align( FL_ALIGN_LEFT );
				o->callback( spin_int_callback, &(m_infostack[i]) );
				o->tooltip( parameters[i].help_string );
				o->maximum( parameters[i].val_max.val_i );
				o->minimum( parameters[i].val_min.val_i );
				bg_parameter_value_t val;
				bg_cfg_section_get_parameter( m_effect->config(), &parameters[i], &val );
				o->value( val.val_i );
				/*
				bg_parameter_value_t val;
				m_filter->common.get_parameter( m_filter_instance, parameters[i].name, &val );
				o->value( val.val_i );
				*/
				//o->value( parameters[i].val_default.val_i );
				break;
			}
			case BG_PARAMETER_FLOAT:
			{
				Fl_Spinner* o = new Fl_Spinner( x, y, w, h, parameters[i].long_name );
				o->labelsize( 12 );
				o->type( FL_FLOAT_INPUT );
				o->align( FL_ALIGN_LEFT );
				o->callback( spin_float_callback, &(m_infostack[i]) );
				o->tooltip( parameters[i].help_string );
				o->maximum( parameters[i].val_max.val_f );
				o->minimum( parameters[i].val_min.val_f );
				bg_parameter_value_t val;
				bg_cfg_section_get_parameter( m_effect->config(), &parameters[i], &val );
				o->value( val.val_f );
				/*
				bg_parameter_value_t val;
				m_filter->common.get_parameter( m_filter_instance, parameters[i].name, &val );
				o->value( val.val_f );
				*/
				//o->value( parameters[i].val_default.val_f );
				break;
			}
			case BG_PARAMETER_SLIDER_INT :
			{
				Fl_Slider* o = new Fl_Slider( x, y, w, h, parameters[i].long_name );
				o->labelsize( 12 );
				o->type( 5 );
				o->align( FL_ALIGN_LEFT );
				o->callback( slider_int_callback, &(m_infostack[i]) );
				o->tooltip( parameters[i].help_string );
				o->maximum( parameters[i].val_max.val_i );
				o->minimum( parameters[i].val_min.val_i );
				bg_parameter_value_t val;
				bg_cfg_section_get_parameter( m_effect->config(), &parameters[i], &val );
				o->value( val.val_i );
				/*
				bg_parameter_value_t val;
				m_filter->common.get_parameter( m_filter_instance, parameters[i].name, &val );
				o->value( val.val_i );
				*/
				//o->value( parameters[i].val_default.val_i );
				break;
			}
			case BG_PARAMETER_SLIDER_FLOAT :
			{
				Fl_Slider* o = new Fl_Slider( x, y, w, h, parameters[i].long_name );
				o->labelsize( 12 );
				o->type( 5 );
				o->align( FL_ALIGN_LEFT );
				o->callback( slider_float_callback, &(m_infostack[i]) );
				o->tooltip( parameters[i].help_string );
				o->maximum( parameters[i].val_max.val_f );
				o->minimum( parameters[i].val_min.val_f );
				bg_parameter_value_t val;
				bg_cfg_section_get_parameter( m_effect->config(), &parameters[i], &val );
				o->value( val.val_f );
				/*
				bg_parameter_value_t val;
				m_filter->common.get_parameter( m_filter_instance, parameters[i].name, &val );
				o->value( val.val_f );
				*/
				//o->value( parameters[i].val_default.val_f );
				break;
			}
			case BG_PARAMETER_STRING :
			case BG_PARAMETER_STRING_HIDDEN:
			{
				Fl_Input* o;
				if ( parameters[i].type == BG_PARAMETER_STRING ) {
					o = new Fl_Input( x, y, w, h, parameters[i].long_name );
				} else {
					o = new Fl_Secret_Input( x, y, w, h, parameters[i].long_name );
				}
				o->labelsize( 12 );
				o->align( FL_ALIGN_LEFT );
				o->tooltip( parameters[i].help_string );
				o->textsize( 12 );
				o->callback( input_string_callback, &(m_infostack[i]) );
				o->value( parameters[i].val_default.val_str );
				break;
			}
			case BG_PARAMETER_STRINGLIST :
			{
				Fl_Choice* o = new Fl_Choice( x, y, w, h, parameters[i].long_name );
				o->labelsize( 12 );
				o->align( FL_ALIGN_LEFT );
				o->tooltip( parameters[i].help_string );
				o->textsize( 12 );
				o->user_data( &(m_infostack[i]) );
				for ( int j = 0; parameters[i].multi_labels[j]; j++ ) {
					o->add( parameters[i].multi_labels[j], 0, (Fl_Callback*)string_list_menu_callback, (void*)parameters[i].multi_names[j] );
				}
				o->value(0);
				break;
			}
			case BG_PARAMETER_COLOR_RGB :
			case BG_PARAMETER_COLOR_RGBA:
			{
				Fl_Button* o = new Fl_Button( x, y, w, h, parameters[i].long_name );
				o->labelsize( 12 );
				o->align( FL_ALIGN_LEFT );
				o->tooltip( parameters[i].help_string );
				o->callback( color_rgb_callback, &(m_infostack[i]) );
				break;
			}
			case BG_PARAMETER_FONT :
			case BG_PARAMETER_DEVICE:
			case BG_PARAMETER_FILE :
			case BG_PARAMETER_DIRECTORY:
			case BG_PARAMETER_MULTI_MENU:
			case BG_PARAMETER_MULTI_LIST:
			case BG_PARAMETER_MULTI_CHAIN:
			case BG_PARAMETER_TIME :
			case BG_PARAMETER_POSITION:
	;	}
	}
}
GmerlinWidget::~GmerlinWidget()
{
	delete [] m_infostack;
}
void GmerlinWidget::setValue( const char* name, const bg_parameter_info_t* info, bg_parameter_value_t *v )
{
	m_filter->common.set_parameter( m_filter_instance, name, v );
	bg_cfg_section_set_parameter( m_effect->config(), info, v );
	m_filter->common.set_parameter( m_filter_instance, 0, 0 );
	g_videoView->redraw();
}

} /* namespace nle */
