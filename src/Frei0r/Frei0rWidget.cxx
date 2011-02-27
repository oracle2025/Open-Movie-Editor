#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Pixmap.H>
#include <cassert>
#include "Frei0rWidget.H"

#include "Frei0rEffect.H"
#include "frei0r.h"
#include "VideoViewGL.H"
#include <iostream>

namespace nle
{
static void doubleCallback( Fl_Widget* i, void* v )
{
	Fl_Slider* vi = dynamic_cast<Fl_Slider*>(i);
	widget_callback_info* info = (widget_callback_info*)v;
	info->widget->setDouble( info->number, vi->value() );
}
static void stringCallback( Fl_Widget* i, void* v )
{
	Fl_Input* fi = dynamic_cast<Fl_Input*>(i);
	widget_callback_info* info = (widget_callback_info*)v;
	info->widget->setString( info->number, fi->value() );
}

static void boolCallback( Fl_Widget* i, void* v )
{
	Fl_Check_Button* vi = dynamic_cast<Fl_Check_Button*>(i);
	widget_callback_info* info = (widget_callback_info*)v;
	info->widget->setDouble( info->number, vi->value() );
}
static void xCallback( Fl_Widget* i, void* v )
{
	Fl_Value_Input* vi = dynamic_cast<Fl_Value_Input*>(i);
	widget_callback_info* info = (widget_callback_info*)v;
	info->widget->setPositionX( info->number, vi->value() );
}
static void yCallback( Fl_Widget* i, void* v )
{
	Fl_Value_Input* vi = dynamic_cast<Fl_Value_Input*>(i);
	widget_callback_info* info = (widget_callback_info*)v;
	info->widget->setPositionY( info->number, vi->value() );
}

Frei0rWidget::Frei0rWidget( Frei0rEffect* effect )
	: IEffectWidget( 0, 0, 340, effect->getPluginInfo()->num_params * 25 + 5 ), m_effect( effect )
	//: IEffectWidget(0, 0, 340, effect->getPluginInfo()->num_params * 30 + 5 ),
{
	f0r_plugin_info_t* finfo;
	f0r_param_info_t pinfo;
	finfo = m_effect->getPluginInfo();

	m_infostack = new widget_callback_info[finfo->num_params];

	for ( int i = 0; i < finfo->num_params; i++ ) {
		m_effect->getParamInfo( &pinfo, i );
		m_infostack[i].widget = this;
		m_infostack[i].number = i;
		int x, y, w, h;
		w = 225;
		h = 20;
		x = 100;
		y = 5 + ( i * 25 );
		switch ( pinfo.type ) {
			case F0R_PARAM_DOUBLE: //Seems to be always between 0.0 and 1.0
				{
				Fl_Slider* o = new Fl_Slider( x, y, w /*- 25*/, h, pinfo.name );
				o->labelsize( 12 );
				o->type( 5 );
				o->callback( doubleCallback, &(m_infostack[i]) );
				o->align(FL_ALIGN_LEFT);
				o->tooltip( pinfo.explanation );
				f0r_param_double dvalue;
				m_effect->getValue( &dvalue, i );
				o->value( dvalue );
				
				/*
				Fl_Button* b = new Fl_Button( x + w - 25, y, 25, h, "@2>" );
				b->callback( mapButtonCallback, &(m_infostack[i]) ); */ /* Where to specify the parameter? Create a custom button class */
				break;
				}
			case F0R_PARAM_BOOL:
				{
				Fl_Check_Button* b = new Fl_Check_Button( x, y, w, h, pinfo.name );
				b->labelsize( 12 );
				b->callback( boolCallback, &(m_infostack[i]) );
				b->tooltip( pinfo.explanation );
				f0r_param_bool bvalue;
				m_effect->getValue( &bvalue, i );
				b->value( ( bvalue >= 0.5 ) );
				break;
				}
			case F0R_PARAM_COLOR:
				break;
			case F0R_PARAM_POSITION:
				{
				Fl_Value_Input* sx = new Fl_Value_Input( x, y, 110, h, pinfo.name);
				sx->labelsize( 12 );
				Fl_Value_Input* sy = new Fl_Value_Input( x + 115, y, 110, h );
				sx->callback( xCallback, &(m_infostack[i]) );
				sy->callback( yCallback, &(m_infostack[i]) );
				sx->range( 0.0, 1.0 );
				sy->range( 0.0, 1.0 );
				sx->step( 0.01 );
				sy->step( 0.01 );
				f0r_param_position_t pvalue;
				m_effect->getValue( &pvalue, i );
				sx->value( pvalue.x );
				sy->value( pvalue.y );
				m_infostack[i].x = pvalue.x;
				m_infostack[i].y = pvalue.y;
				break;
				}
			case F0R_PARAM_STRING:
				{
				Fl_Input* o = new Fl_Input( x, y, w, h, pinfo.name );
				o->labelsize( 12 );
				o->align(FL_ALIGN_LEFT);
				o->tooltip( pinfo.explanation );
				o->callback( stringCallback, &(m_infostack[i]) );
				char *txt = new char[1024];
				if ( txt != NULL ) {
					m_effect->getValue( txt, i );
					o->value( txt );
					delete [] txt;
				}
				break;
				}
			default:
				break;
		};
	}

	
}
Frei0rWidget::~Frei0rWidget()
{
	delete [] m_infostack;
}

void Frei0rWidget::setDouble( int num, double val )
{
	f0r_param_double dvalue;
	dvalue = val;
	m_effect->setValue( &dvalue, num );
	g_videoView->redraw();
}
void Frei0rWidget::setBool( int num, bool val )
{
	f0r_param_bool bvalue;
	bvalue = val;
	m_effect->setValue( &bvalue, num );
	g_videoView->redraw();
}
void Frei0rWidget::setPositionX( int num, double val )
{
	f0r_param_position_t pos;
	pos.y = m_infostack[num].y;
	pos.x = val;
	m_infostack[num].x = val;
	m_effect->setValue( &pos, num );
	g_videoView->redraw();
}
void Frei0rWidget::setPositionY( int num, double val )
{
	f0r_param_position_t pos;
	pos.x = m_infostack[num].x;
	pos.y = val;
	m_infostack[num].y = val;
	m_effect->setValue( &pos, num );
	g_videoView->redraw();
}
void Frei0rWidget::setString( int num, const char* val )
{
	f0r_param_string *fstr;
	fstr = (char*)val;
	m_effect->setValue( fstr, num );
	g_videoView->redraw();
}


} /* namespace nle */
