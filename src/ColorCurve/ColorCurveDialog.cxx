/* ColorCurveDialog.cxx
 *
 *  Copyright (C) 2007 Richard Spindler <richard.spindler AT gmail.com>
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

#include "ColorCurveDialog.H"
#include "ColorCurveFilter.H"
#include "ColorGrader2.h"


namespace nle
{
static void closeCallback( Fl_Widget*, void* data ) {
//	i->window()->hide();
	ColorCurveDialog* dlg = (ColorCurveDialog*)data;
	delete dlg;
}

ColorCurveDialog::ColorCurveDialog( ColorCurveFilter* filter )
	: m_filter( filter )
{
	struct color_curve_data* cd = &(filter->m_parameters);
	m_dialog = new Fl_Double_Window( 605, 200 + 25 + 10, "Color Curves" );

	//ColorGrader2* cg = new ColorGrader2( 0, 0, 430, 335 );
	ColorGrader2* cg = m_cg = new ColorGrader2( 0, 0, 605, 200 );
	cg->m_dialog = this;
	cg->editor_red->lineColor( FL_RED );
	cg->editor_green->lineColor( FL_GREEN );
	cg->editor_blue->lineColor( FL_BLUE );
	cg->editor_master->lineColor( FL_BLACK );

	cg->editor_red->set( cd->r.p1.x, cd->r.p1.y, cd->r.p2.x, cd->r.p2.y );
	cg->editor_green->set( cd->g.p1.x, cd->g.p1.y, cd->g.p2.x, cd->g.p2.y );
	cg->editor_blue->set( cd->b.p1.x, cd->b.p1.y, cd->b.p2.x, cd->b.p2.y );
	cg->editor_master->set( cd->m.p1.x, cd->m.p1.y, cd->m.p2.x, cd->m.p2.y );
	

	{
		Fl_Return_Button* o = new Fl_Return_Button( 5, 205, 595, 25, "Close" );
		o->callback( closeCallback, this );
		m_dialog->hotspot( o );
	}
	
	m_dialog->set_non_modal();
	m_dialog->end();
}
ColorCurveDialog::~ColorCurveDialog()
{
	m_filter->m_dialog = 0;
	m_dialog->hide();
	delete m_dialog;
}
void ColorCurveDialog::show()
{
	m_dialog->show();
}
int ColorCurveDialog::shown()
{
	return m_dialog->shown();
}
void ColorCurveDialog::read_values()
{
	struct color_curve_data* cd = &(m_filter->m_parameters);
	m_cg->editor_red->get( cd->r.p1.x, cd->r.p1.y, cd->r.p2.x, cd->r.p2.y );
	m_cg->editor_green->get( cd->g.p1.x, cd->g.p1.y, cd->g.p2.x, cd->g.p2.y );
	m_cg->editor_blue->get( cd->b.p1.x, cd->b.p1.y, cd->b.p2.x, cd->b.p2.y );
	m_cg->editor_master->get( cd->m.p1.x, cd->m.p1.y, cd->m.p2.x, cd->m.p2.y );
	m_filter->calculate_values();
	m_filter->bypass( m_cg->bypass_check->value() );
	

	g_videoView->redraw();
}

} /* namespace nle */
