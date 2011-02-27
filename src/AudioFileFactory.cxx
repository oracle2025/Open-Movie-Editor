/*  AudioFileFactory.cxx
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

#include "AudioFileFactory.H"
#include "AudioFileSnd.H"
#include "AudioFileProject.H"
#include "AudioFileGmerlin.H"
#include "Resampler.H"
#include <FL/filename.H>

#include <cstring>

namespace nle
{

IAudioFile* AudioFileFactory::get( string filename )
{
	const char* ext = fl_filename_ext( filename.c_str() );
	IAudioFile *af = 0;
	if ( strcmp( ext, ".vproj" ) == 0 ) {
		af = new AudioFileProject( filename );
		return af;
	}

	af = new AudioFileSnd( filename );
	if ( !af || !af->ok() ) {
		if ( af ) {
			delete af;
		}
		af = new AudioFileGmerlin( filename );
	}
	if ( !af->ok() ) {
		delete af;
		af = 0;
	}
	if ( af && af->samplerate() != 48000 ) {
		af = new Resampler( af );
	}
	return af;
}


} /* namespace nle */
