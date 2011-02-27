/*  BezierCurveNode.H
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

#ifndef _BEZIER_CURVE_NODE_H_
#define _BEZIER_CURVE_NODE_H_

#include "INode.H"
#include "CurveEditorBezier.H"

class BezierCurveNode : public INode
{
	public:
		BezierCurveNode( int w, int h );
		~BezierCurveNode();
		virtual void setInput( int, INode* ) {}
		virtual int getInputCount() { return 1; }
		virtual int getOutputCount() { return 1; }
		virtual uint32_t* getFrame( int output, double position );
		void init_widgets();
		void delete_widgets();
		void readXML( TiXmlElement* xml_node );
		void writeXML( TiXmlElement* xml_node );
		const char* name() { return "Bezier Color Curve"; }
		const char* identifier() { return "effect:builtin:BezierCurveFilter"; }
	private:
		void calc_lut();
		void calc_curve_lut();
		uint32_t* m_frame;
		unsigned char m_values[256];
		int m_length;
		point_list *m_points;
};

#endif /* _BEZIER_CURVE_NODE_H_ */
