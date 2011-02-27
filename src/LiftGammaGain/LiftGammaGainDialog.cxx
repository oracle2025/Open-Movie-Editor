/* LiftGammaGainDialog.cxx
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

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Return_Button.H>

#include "LiftGammaGainDialog.H"
#include "LiftGammaGainFilter.H"
#include "LiftGammaGainWidget.H"
#include "VideoViewGL.H"


namespace nle
{

static void closeCallback( Fl_Widget*, void* data ) {
	LiftGammaGainDialog* dlg = (LiftGammaGainDialog*)data;
	delete dlg;
}

LiftGammaGainDialog::LiftGammaGainDialog( LiftGammaGainFilter* filter )
	: m_filter( filter )
{
	m_dialog = new Fl_Double_Window( 585, 190 + 25 + 10, "Lift Gamma Gain" );

	m_widget = new LiftGammaGainWidget( 0, 0, 585, 190 );
	m_widget->lift->rgb( m_filter->lift()[0], m_filter->lift()[1], m_filter->lift()[2] );
	m_widget->gamma->rgb( m_filter->gamma()[0], m_filter->gamma()[1], m_filter->gamma()[2] );
	m_widget->gain->rgb( m_filter->gain()[0], m_filter->gain()[1], m_filter->gain()[2] );
	m_widget->lift_slider->value( m_filter->lift()[3] );
	m_widget->gamma_slider->value( m_filter->gamma()[3] );
	m_widget->gain_slider->value( m_filter->gain()[3] );
	m_widget->m_dialog = this;

	{
		Fl_Return_Button* o = new Fl_Return_Button( 5, 195, 575, 25, "Close" );
		o->callback( closeCallback, this );
		m_dialog->hotspot( o );
	}


	m_dialog->set_non_modal();
	m_dialog->end();
}
LiftGammaGainDialog::~LiftGammaGainDialog()
{
	m_filter->m_dialog = 0;
	m_dialog->hide();
	delete m_dialog;
}
void LiftGammaGainDialog::show()
{
	m_dialog->show();
}
int LiftGammaGainDialog::shown()
{
	return m_dialog->shown();
}
void LiftGammaGainDialog::read_values()
{
	m_filter->lift( m_widget->lift->r(), m_widget->lift->g(), m_widget->lift->b(), m_widget->lift_slider->value() );
	m_filter->gamma( m_widget->gamma->r(), m_widget->gamma->g(), m_widget->gamma->b(), m_widget->gamma_slider->value() );
	m_filter->gain( m_widget->gain->r(), m_widget->gain->g(), m_widget->gain->b(), m_widget->gain_slider->value() );
	m_filter->calculate_values();
	m_filter->bypass( m_widget->bypass->value() );

	g_videoView->redraw();
}

} /* namespace nle */
