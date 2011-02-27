/* Prefs.H
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

#ifndef _PREFS_H_
#define _PREFS_H_


#include "global_includes.H"

namespace nle
{


class Prefs
{
	public:
		Prefs();
		~Prefs();
		string getBrowserFolder();
		void setBrowserFolder( string filename );
		string getMediaFolder() { return m_mediaFolder; }
		void setMediaFolder( string filename ) { m_mediaFolder = filename; }
		string lastProject();
		void lastProject( string filename );
		void lastVideoCodec( int codec ) { m_lastVideoCodec = codec; }
		int lastVideoCodec() { return m_lastVideoCodec; }
		void lastAudioCodec( int codec ) { m_lastAudioCodec = codec; }
		int lastAudioCodec() { return m_lastAudioCodec; }
		void lastRenderFilename( string filename ) { m_lastRenderFilename = filename; }
		string lastRenderFilename() { return m_lastRenderFilename; }
		void colorScheme( string name ) { m_colorScheme = name; }
		string colorScheme() { return m_colorScheme; }
		void lastFramesize( int fs ) { m_lastFramesize = fs; }
		int lastFramesize() { return m_lastFramesize; }
		void lastFramerate( int fr ) { m_lastFramerate = fr; }
		int lastFramerate() { return m_lastFramerate; }
		void setWindowPosition( int x, int y, int w, int h );
		void getWindowPosition( int& x, int& y, int& w, int& h );
	private:
		string m_mediaFolder;
		string m_browserFolder;
		string m_lastProject;
		string m_lastRenderFilename;
		string m_colorScheme;
		int m_lastVideoCodec;
		int m_lastAudioCodec;
		int m_lastFramesize;
		int m_lastFramerate;
		int m_window_x;
		int m_window_y;
		int m_window_w;
		int m_window_h;
		
};
extern Prefs* g_preferences;

} /* namespace nle */

#endif /* _PREFS_H_ */


