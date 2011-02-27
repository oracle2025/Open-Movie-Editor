/*  PreviewNode.cxx
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


#include "PreviewNode.H"
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

namespace nle
{

class PreviewNodeWidget : public Fl_Gl_Window
{
	public:
		PreviewNodeWidget( int x, int y, int w, int h, const char *l = 0 )
			: Fl_Gl_Window( x, y, w, h, l )
		{
			m_once = true;
			m_dirty = true;
			m_video_w = 0;
			m_video_h = 0;
			m_video_buffer = 0;
		}
		~PreviewNodeWidget()
		{
		}
		void draw()
		{
			static GLint max[2];
			if ( !valid() ) {
				glLoadIdentity(); glViewport( 0, 0, w(), h() ); // glViewport( _x, _y, _w, _h );
				glOrtho( 0.0, 10.0, 10.0, 0.0, -20000, 10000 ); glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable (GL_TEXTURE_2D);
			}
			if ( m_once ) {
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, max);
				glGenTextures( 1, m_video_texture );

				glBindTexture (GL_TEXTURE_2D, m_video_texture[0] );
				glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, max[0], max[0], 0, GL_RGB,GL_UNSIGNED_BYTE, 0 );
				m_once = false;
			}
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glBindTexture( GL_TEXTURE_2D, m_video_texture[0] );

			if ( !m_video_buffer ) {
				return;
			}

			if ( m_dirty ) {
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_video_w, m_video_h, GL_RGBA, GL_UNSIGNED_BYTE, m_video_buffer );
				m_dirty = false;
			}

			float gl_x, gl_y, gl_w, gl_h;
			{
				float f_v = ((float)m_video_w) / ((float)m_video_h);
				float f_w = ( (float)w() / (float)h() );
				float f_g = f_v / f_w;
				if ( f_g > 1.0 ) {
					gl_h = 10.0 / f_g;
					gl_w = 10.0;
				} else {
					gl_h = 10.0;
					gl_w = f_g * 10.0;
				}
				gl_x = ( 10.0 - gl_w ) / 2.0;
				gl_y = ( 10.0 - gl_h ) / 2.0;
			}

			float ww = ((float)m_video_w) / max[0];
			float xx = 0;
			float hh = ((float)m_video_h) / max[0];

			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
			glBegin (GL_QUADS);
				glTexCoord2f (  xx,          0.0              );
				glVertex3f   (  gl_x,        gl_y,        0.0 );
				glTexCoord2f (  xx + ww,     0.0              );
				glVertex3f   (  gl_x + gl_w, gl_y,        0.0 );
				glTexCoord2f (  xx + ww,     hh               );
				glVertex3f   (  gl_x + gl_w, gl_y + gl_h, 0.0 );
				glTexCoord2f (  xx,          hh               );
				glVertex3f   (  gl_x,        gl_y + gl_h, 0.0 );
			glEnd ();
		}
		void touch() { m_dirty = true; }
		void set_video_buffer( unsigned char* b, int w, int h )
		{ m_video_buffer = b; m_video_w = w; m_video_h = h; }
	private:
		bool m_once;
		int m_video_w;
		int m_video_h;
		unsigned char* m_video_buffer;
		bool m_dirty;
		GLuint m_video_texture[1];
};

PreviewNode::PreviewNode( int w, int h )
{
	m_width = w;
	m_height = h;
	m_view = 0;
}

PreviewNode::~PreviewNode()
{
	delete_widgets();
}
uint32_t* PreviewNode::getFrame( int output, double position )
{
	if ( !m_view ) {
		return 0;
	}
	if ( !m_view->shown() && m_view->window()->shown() ) {
		m_view->show();
	}
	if ( !m_view->window()->shown() ) {
		return 0;
	}
	uint32_t* input_frame;
	if ( node->inputs[0] == 0 || !node->inputs[0]->node ) {
		return 0;
	}
	input_frame = node->inputs[0]->node->getFrame( 0, position );
	if ( !input_frame ) {
		return 0;
	}
	m_view->set_video_buffer( (unsigned char*)input_frame, m_width, m_height );
	m_view->touch();
	m_view->redraw();
	return 0;
}

void PreviewNode::init_widgets()
{
	m_view = new PreviewNodeWidget( 0, 0, 320, 240 );
	node->widgets[0] = m_view;
	node->widgets[1] = 0;
	//view->show();
}
void PreviewNode::delete_widgets()
{
	for ( int i = 0; node->widgets[i]; i++ ) {
		node->widgets[i]->parent()->remove( node->widgets[i] );
		delete node->widgets[i];
		node->widgets[i] = 0;
	}
	m_view = 0;
}
void PreviewNode::readXML( TiXmlElement* xml_node )
{
}
void PreviewNode::writeXML( TiXmlElement* xml_node )
{
}

} /* namespace nle */

