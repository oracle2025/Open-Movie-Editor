/* Frei0rDialog.H
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _FREI0R_DIALOG_H_
#define _FREI0R_DIALOG_H_

#include "IEffectDialog.H"

class Fl_Double_Window;

namespace nle
{

class Frei0rDialog;

typedef struct _callback_info {
	Frei0rDialog* dialog;
	int number;
	double x, y;
} callback_info;


class Frei0rEffect;
class AutoTrack;

class Frei0rDialog : public IEffectDialog
{
	public:
		Frei0rDialog( Frei0rEffect* effect );
		~Frei0rDialog();
		void show();
		int shown();
		void setDouble( int num, double val );
		void setBool( int num, bool val );
		void setPositionX( int num, double val );
		void setPositionY( int num, double val );
		void setString( int num, const char* val );
		void setAutomation( int num, AutoTrack* track );
	private:
		Fl_Double_Window* m_dialog;
		Frei0rEffect* m_effect;
		callback_info* m_infostack;
};

} /* namespace nle */

#endif /* _FREI0R_DIALOG_H_ */
