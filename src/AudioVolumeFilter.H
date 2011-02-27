/*  AudioVolumeFilter.H
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

#ifndef _AUDIO_VOLUME_FILTER_H_
#define _AUDIO_VOLUME_FILTER_H_

#include <stdint.h>
#include "Rect.H"
#include "IAudioFilter.H"

namespace nle
{

class DragHandler;
class AudioClip;

typedef struct _auto_node {
	struct _auto_node *next;
	int64_t x;
	float y;
} auto_node;

class AudioVolumeFilter : public IAudioFilter
{
	public:
		AudioVolumeFilter( AudioClip* clip );
		~AudioVolumeFilter();
		void onDraw( Rect& rect );
		DragHandler* onMouseDown( Rect& rect, int x, int y, bool shift );
		int fillBuffer( float* input_output, unsigned long frames, int64_t position );
		void writeXML( TiXmlElement* xml_node );
		void readXML( TiXmlElement* xml_node );
		//void setParams();
		//void getParams();
		void reset();
		int64_t trimA( int64_t trim );
		int64_t trimB( int64_t trim );
		const char* name() { return "Audio Volume Filter"; }
		const char* identifier() { return "filter:builtin:VolumeAutomations"; }
		FilterData* getFilterData() { return 0; }
		void setFilterData( FilterData* ) {}
	private:
		float getEnvelope( int64_t position );
		auto_node* m_automationPoints;
		auto_node* m_autoCache;
		AudioClip* m_audioClip;
};

} /* namespace nle */


#endif /* _AUDIO_VOLUME_FILTER_H_ */

