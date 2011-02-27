/*  VideoEffectClip.cxx
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

#include <cassert>
#include <gavl/gavl.h>
#include "sl/sl.h"
#include "global_includes.H"

#include "Timeline.H"
#include "VideoEffectClip.H"
#include "FilterFactory.H"
#include "IVideoEffect.H"
#include "VideoClip.H"
#include "TitleClip.H"
#include "render_helper.H"
#include "frei0r.h"
#include "globals.H"
#include "IVideoFile.H"
#include "XmlClipData.H"
#include "MainFilterFactory.H"
#include <tinyxml.h>
#include "LazyFrame.H"

namespace nle
{
struct effect_parameter {
	string name;
	f0r_param_bool param_bool;
	f0r_param_double param_double;
	f0r_param_color_t param_color;
	f0r_param_position_t param_position;
	f0r_param_string* param_string;
};
struct effect_data {
	string name;
	struct effect_parameter* parameters;
};

class EffectClipData : public ClipData
{
	public:
		~EffectClipData() {
			for ( int i = 0; i < effect_count; i++ ) {
				delete [] effects[i].parameters;
			}
			if ( effects ) {
				delete [] effects;
			}
		}
		struct effect_data* effects;
		int effect_count;
};

VideoEffectClip::VideoEffectClip( FilterClip* filterClip )
{
	m_filterClip = filterClip;
	//m_effects = 0;
}
void VideoEffectClip::setEffects( ClipData* clip_data )
{
	if ( XmlClipData* xml = dynamic_cast<XmlClipData*>(clip_data) ) {
		TiXmlElement* filterXml = TiXmlHandle( xml->m_xml ).FirstChildElement( "filter" ).Element();
		for ( ; filterXml; filterXml = filterXml->NextSiblingElement( "filter" ) ) {
			FilterFactory* ff = g_mainFilterFactory->get( filterXml->Attribute( "identifier" ) );
			if ( ff ) {
				FilterBase* filter = m_filterClip->appendFilter( ff );
				filter->readXML( filterXml );
			}
		}
	}

}
VideoEffectClip::~VideoEffectClip()
{
}
LazyFrame* VideoEffectClip::getFrame( int64_t position )
{
	int64_t position_in_file = 0;
	LazyFrame*f = getRawFrame( position, position_in_file );
	if ( !f ) {
		return 0;
	}
	filter_stack *node = m_filterClip->getFilters();
	IVideoEffect* effect;
	while ( node ) {
		effect = dynamic_cast<IVideoEffect*>( node->filter );
		node = node->next;
		if ( !effect ) { continue; }
		f = effect->getFrame( f, position_in_file );
	}

	// TODO: Copy pixel aspect
	//f->render_strategy = m_render_strategy;
	return f;

}

ClipData* VideoEffectClip::vec_getClipData()
{
	XmlClipData* xml = new XmlClipData;
	xml->m_xml = new TiXmlElement( "clip_data" );
	filter_stack* filters;
	for ( filters = m_filterClip->getFilters(); filters; filters = filters->next ) {
		TiXmlElement* filter_xml = new TiXmlElement( "filter" );
		xml->m_xml->LinkEndChild( filter_xml );
		filter_xml->SetAttribute( "name", filters->filter->name() );
		filter_xml->SetAttribute( "identifier", filters->filter->identifier() );
		filters->filter->writeXML( filter_xml );
	}
	return xml;
}


} /* namespace nle */
