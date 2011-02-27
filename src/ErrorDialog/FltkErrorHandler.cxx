/*  FltkErrorHandler.cxx
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#include "ErrorDialog.H"
#include "globals.H"
#include "FltkErrorHandler.H"

void error_callback( void* data )
{
	nle::FltkErrorHandler* h = (nle::FltkErrorHandler*)data;
	h->errorDialog();
}

namespace nle
{

IErrorHandler* g_errorHandler;

FltkErrorHandler::FltkErrorHandler()
{
	g_errorHandler = this;
}
FltkErrorHandler::~FltkErrorHandler()
{
	g_errorHandler = 0;
}
void FltkErrorHandler::showError( string msg )
{
	m_msg = msg;
	m_detailsBuffer = m_details;
	Fl::add_idle( error_callback, this );
}
void FltkErrorHandler::errorDialog()
{
	Fl::remove_idle( error_callback, this );
	ErrorDialog dlg;
	dlg.error( m_msg.c_str() );
	dlg.details( m_detailsBuffer.c_str() );
	dlg.show();
	while ( dlg.shown() ) {
		Fl::wait();
	}
}

	
} /* namespace nle */

