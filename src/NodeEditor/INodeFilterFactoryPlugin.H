/*  INodeFilterFactoryPlugin.H
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

#ifndef _I_NODE_FILTER_FACTORY_PLUGIN_H_
#define _I_NODE_FILTER_FACTORY_PLUGIN_H_

class INode;

class INodeFilterFactoryPlugin
{
	public:
		virtual ~INodeFilterFactoryPlugin() {}
		virtual INode* get_i_node( int w, int h ) = 0;
		virtual const char* name() = 0;
		virtual const char* identifier() = 0;
};

#endif /* _I_NODE_FILTER_FACTORY_PLUGIN_H_ */
