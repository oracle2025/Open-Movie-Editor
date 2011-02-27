/*  ImageNode.cxx
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

#include <FL/Fl_Shared_Image.H>
#include <tinyxml.h>

#include "ImageNode.H"
namespace nle
{

ImageNode::ImageNode( std::string filename, int w, int h )
{
	m_frame = 0;
	m_image = 0;
	m_width = w;
	m_height = h;
	init( filename.c_str() );
}
ImageNode::ImageNode( int w, int h )
{
	m_frame = 0;
	m_image = 0;
	m_width = w;
	m_height = h;
}
void ImageNode::init( const char* filename )
{
	if ( !filename ) {
		return;
	}
	m_filename = filename;
	m_image = Fl_Shared_Image::get( filename, m_width, m_height );
	char** d = (char**)m_image->data();
	if ( m_image->d() == 4 ) {
		m_frame = (uint32_t*)d[0];
	} else if ( m_image->d() == 3 ) {
		m_frame = new uint32_t[m_width*m_height];
		unsigned char* src = (unsigned char *)d[0];
		unsigned char* src_end = src + ( m_width * m_height * 3 );
		unsigned char* dst = (unsigned char *)m_frame;
		while ( src < src_end ) {
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			dst[3] = 0xFF;
			src += 3;
			dst += 4;
		}
		m_image->release();
		m_image = 0;
	}
}
ImageNode::~ImageNode()
{
	if ( m_image ) {
		m_image->release();
		m_image = 0;
	} else if ( m_frame ) {
		delete [] m_frame;
	}

}
uint32_t* ImageNode::getFrame( int output, double position )
{
	return m_frame;
}
void ImageNode::init_widgets()
{
}

void ImageNode::delete_widgets()
{
}

void ImageNode::readXML( TiXmlElement* xml_node )
{
	TiXmlElement* filename = TiXmlHandle( xml_node ).FirstChildElement( "filename" ).Element();
	if ( filename ) {
		init( filename->Attribute( "value" ) );
	}
}

void ImageNode::writeXML( TiXmlElement* xml_node )
{
	TiXmlElement* filename;
	filename = new TiXmlElement( "filename" );
	xml_node->LinkEndChild( filename );
	filename->SetAttribute( "value", m_filename.c_str() );
}

} /* namespace nle */
