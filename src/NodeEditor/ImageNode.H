/*  ImageNode.H
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

#ifndef _IMAGE_NODE_H_
#define _IMAGE_NODE_H_

#include <string>

#include "INode.H"

class Fl_Shared_Image;

namespace nle
{

class PreviewNodeWidget;
class ImageNode : public INode
{
	public:
		ImageNode( int w, int h );
		ImageNode( std::string filename, int w, int h );
		~ImageNode();
		void setInput( int, INode* ) {}
		int getInputCount() { return 0; }
		int getOutputCount() { return 1; }
		uint32_t* getFrame( int output, double position );
		void init_widgets();
		void delete_widgets();
		void readXML( TiXmlElement* xml_node );
		void writeXML( TiXmlElement* xml_node );
		const char* name() { return "Image"; }
		const char* identifier() { return "effect:builtin:Image"; }
	private:
		void init( const char* filename );
		int m_width;
		int m_height;
		PreviewNodeWidget* m_view;
		Fl_Shared_Image* m_image;
		uint32_t* m_frame;
		std::string m_filename;

};

} /* namespace nle */


#endif /* _IMAGE_NODE_H_ */
