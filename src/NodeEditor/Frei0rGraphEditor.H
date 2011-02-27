#ifndef _FREI0R_GRAPH_EDITOR_H_
#define _FREI0R_GRAPH_EDITOR_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include "NodeFilter.H"
#include <string>

class INodeFilterFactoryPlugin;
class NodeFilterFrei0rFactory;
class INode;

filters* filters_create( int x, int y, int w, int h, INode* node, std::string name = "", int id = -1 );

class Frei0rGraphEditor : public Fl_Widget
{
        public:
                Frei0rGraphEditor( int x, int y, int w, int h, const char *label = 0 );
		~Frei0rGraphEditor();
		void draw();
		int handle( int event );
	//	INode* getSink() { return m_sink_node; }
		void addNode( INodeFilterFactoryPlugin* ffp );
		Fl_Widget* m_trash;
		void setFilter( nle::NodeFilter* filter );
		void init_all_widgets();
	private:
		nle::NodeFilter* m_filter;
//		filters* m_filters;
		filters* m_current;
		int m_x_drag_offset;
		int m_y_drag_offset;
		int m_input_drag;
		int m_output_drag;
		int m_connection_x;
		int m_connection_y;
	//	INode* m_sink_node;
		NodeFilterFrei0rFactory* m_factory;

};

#endif /* _FREI0R_GRAPH_EDITOR_H_ */

