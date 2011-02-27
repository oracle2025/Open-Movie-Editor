/*  IErrorHandler.H
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

#ifndef _I_ERROR_HANDLER_H_
#define _I_ERROR_HANDLER_H_

#include "global_includes.H"

#define ERROR_DETAIL( s ) g_errorHandler->detail( s )
#define SHOW_ERROR( s ) g_errorHandler->error( s )
#define CLEAR_ERRORS() g_errorHandler->clear()

namespace nle
{

/**
 * This is a simple Error Handler. Detailed error descriptions are added using
 * the detail() call, these are stored internally until the error() function is
 * called. Then the given error message is displayed, featuring the details as
 * extended documentation. The details buffer is then cleared.
 */

class IErrorHandler
{
	public:
		virtual ~IErrorHandler() {}
		void detail( string msg );
		void error( string msg );
		void clear();
	protected:
		virtual void showError( string msg ) = 0;
		string m_details;
};

extern IErrorHandler* g_errorHandler;

} /* namespace nle */

#endif /* _I_ERROR_HANDLER_H_ */
