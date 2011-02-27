/*  WaveForm.H
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

#include "global_includes.H"
#include "JobManager.H"

#include <string>

namespace nle
{

class IAudioFile;
class DiskCache;

class WaveForm : public Job
{
	public:
		WaveForm( JobDoneListener* listener, string filename );
		~WaveForm();
		bool process( double& progress );
		bool done();
		const char* name() { return "Generating Wave Form"; }
		const char* description() { return "Generating the little audio wave form for the timeline"; }
		float* m_peaks;
		inline int64_t length() { return m_peakLength; }
		const char* filename();
	private:
		int64_t m_peakLength;
		int64_t m_finalLength;
		IAudioFile* m_af;
		//FILE* m_waveformCache;
		//bool m_readFromCache;
		DiskCache* m_cache;
		std::string m_filename;
};

} /* namespace nle */
