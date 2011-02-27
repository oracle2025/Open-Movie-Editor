#include "Frei0rGraphEditor.H"
#include "NodeFilterFrei0rFactory.H"
#include "NodeFilterFrei0rFactoryPlugin.H"
#include "Frei0rNode.H"
#include "BezierCurveNode.H"
#include "PreviewNode.H"
#include "ImageNode.H"
//#include "ImageNode.H"
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/filename.H>
#include "sl/sl.h"
#include <cassert>
#include <iostream>
#include <cstring>
#include <stdlib.h>
using namespace std;

const int FILTER_IO_SIZE = 16;
const int FILTER_IO_MIDDLE = ( FILTER_IO_SIZE / 2 );
const int FILTER_IO_SPACING = ( FILTER_IO_SIZE + 5 );

filters* filters_create( int x, int y, int w, int h, INode* node, string name, int id ) {
	static int id_src = 0;
	filters* p;
	p = new filters;
	p->next = 0;
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->input_count = node->getInputCount();
	p->output_count = node->getOutputCount();
	p->name = name;
	p->node = node;
	node->node = p;
	p->widgets[0] = 0;
	node->init();
	if ( id < 0 ) {
		p->id = id_src++;
	} else {
		p->id = id;
		if ( id >= id_src ) {
			id_src = id + 1;
		}
	}
	if ( p->widgets[0] ) {
		p->h = 0;
	}
	for ( int i = 0; p->widgets[i]; i++ ) {
		p->h += 20;
	}
	for ( int i = 0; i < MAX_FILTER_IN; i++ ) {
		p->inputs[i] = 0;
	}
	for ( int i = 0; i < MAX_FILTER_OUT; i++ ) {
		p->outputs[i] = 0;
		p->target_slots[i] = 0;
	}
	return p;
}

extern NodeFilterFrei0rFactory* g_node_filter_frei0r_factory;

Frei0rGraphEditor::Frei0rGraphEditor( int x, int y, int w, int h, const char *label )
	  : Fl_Widget( x, y, w, h, label )
{
	m_factory = g_node_filter_frei0r_factory;
	m_filter = 0;
	m_trash = 0;
	m_input_drag = -1;
	m_output_drag = -1;
	m_current = 0;
	/*
	m_filters = 0;
	m_filters = (filters*)sl_push( m_filters, filters_create( 10,200, 50,
	50, new SinkNode(), "Output" ) );
	filters* C = m_filters;
	m_sink_node = C->node;*/
}
Frei0rGraphEditor::~Frei0rGraphEditor()
{
	if ( m_filter ) {
		for ( filters* i = m_filter->m_filters; i; i = i->next ) {
			i->node->delete_widgets();
		}
	}
}
void Frei0rGraphEditor::draw()
{
	if ( !m_filter ) {
		return;
	}
	fl_push_clip( x() + 2, y() + 2,  w() - 4, h() - 4 );
	fl_line_style( FL_SOLID, 1 );
	for ( filters* i = m_filter->m_filters; i; i = i->next ) {
		if ( m_current && i == m_current ) {
			if ( m_output_drag >= 0 ) {
				fl_color( FL_FOREGROUND_COLOR );
				fl_line( m_connection_x, m_connection_y, x() + i->x + ( m_output_drag * FILTER_IO_SPACING ) + FILTER_IO_MIDDLE, y() + i->y + i->h + FILTER_IO_MIDDLE );
				fl_draw_box( FL_UP_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
			} else if ( m_input_drag >= 0 ) {
				fl_color( FL_FOREGROUND_COLOR );
				fl_line( m_connection_x, m_connection_y, x() + i->x + ( m_input_drag * FILTER_IO_SPACING ) + FILTER_IO_MIDDLE, y() + i->y - FILTER_IO_MIDDLE );
				fl_draw_box( FL_UP_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
			} else {
				fl_draw_box( FL_DOWN_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
			}
		} else {
			fl_draw_box( FL_UP_BOX, x() + i->x, y() + i->y, i->w, i->h, FL_BACKGROUND_COLOR );
		}
		for ( int j = 0; j < i->input_count; j++ ) {
			fl_draw_box( FL_UP_BOX, x() + i->x + ( j * FILTER_IO_SPACING ), y() + i->y - FILTER_IO_SIZE, FILTER_IO_SIZE, FILTER_IO_SIZE, FL_GREEN );
			fl_color( FL_RED );
			if ( i->inputs[j] ) {
				filters* k = i->inputs[j];
				fl_line( x() + i->x + ( j * FILTER_IO_SPACING ) + FILTER_IO_MIDDLE,
						y() + i->y - FILTER_IO_MIDDLE,
						x() + k->x + FILTER_IO_MIDDLE,
						y() + k->y + k->h + FILTER_IO_MIDDLE );
			}
		}
		for ( int j = 0; j < i->output_count; j++ ) {
			fl_draw_box( FL_UP_BOX, x() + i->x + ( j * FILTER_IO_SPACING ), y() + i->y + i->h, FILTER_IO_SIZE, FILTER_IO_SIZE, FL_RED );
		}
		fl_font( FL_HELVETICA, 12 );
		fl_color( FL_FOREGROUND_COLOR );
		int text_w, text_h;
		fl_measure( i->name.c_str(), text_w, text_h );
		fl_draw( i->name.c_str(), x() + i->x + 3, y() + i->y + 3 + text_h );
		for ( int k = 0; i->widgets[k]; k++ ) {
			if ( dynamic_cast<BezierCurveNode*>( i->node ) ) {
				i->widgets[k]->resize(x()+i->x+i->w,y()+i->y+k*20,200,200);
			} else if ( dynamic_cast<nle::PreviewNode*>( i->node ) ) {
				i->widgets[k]->resize(x()+i->x+i->w,y()+i->y+k*20,320,240);
			} else {
				i->widgets[k]->resize(x()+i->x+i->w,y()+i->y+k*20,200,20);
			}
		}

	}

	fl_pop_clip();
}
static bool inside_filter( filters* f, int x, int y )
{
	return ( f->x < x && f->x + f->w > x && f->y < y && f->y + f->h > y );
}
static int inside_filter_input( filters* f, int x, int y )
{
	for ( int i = 0; i < f->input_count; i++ ) {
		int px1, py1, px2, py2;
		px1 = f->x + ( i * FILTER_IO_SPACING );
		py1 = f->y - FILTER_IO_SIZE;
		px2 = px1 + FILTER_IO_SIZE;
		py2 = py1 + FILTER_IO_SIZE;
		if ( x > px1 && x < px2 && y > py1 && y < py2 ) {
			return i;
		}
	}
	return -1;
}
static int inside_filter_output( filters* f, int x, int y )
{
	for ( int i = 0; i < f->output_count; i++ ) {
		int px1, py1, px2, py2;
		px1 = f->x + ( i * FILTER_IO_SPACING );
		py1 = f->y + f->h;
		px2 = px1 + FILTER_IO_SIZE;
		py2 = py1 + FILTER_IO_SIZE;
		if ( x > px1 && x < px2 && y > py1 && y < py2 ) {
			return i;
		}
	}
	return -1;
}
int Frei0rGraphEditor::handle( int event )
{
	if ( !m_filter ) {
		return Fl_Widget::handle( event );
	}
	switch ( event ) {
		case FL_DND_DRAG:
		case FL_DND_RELEASE:
		case FL_DND_ENTER:
		case FL_DND_LEAVE:
			return 1;
		case FL_PASTE:
			{
				char *fn,*filename=strdup(Fl::event_text());
				int i=strlen(filename);
				while (i>0 && (iscntrl(filename[i]) || isspace(filename[i])) ) filename[i--]=0;
				if (!strncmp(filename,"file://",7)) {
					fn=&(filename[7]); 
				} else {
					fn=filename;
				}
				if ( !fl_filename_isdir(fn)) {
					window()->begin();
					nle::ImageNode *in = new nle::ImageNode( fn, m_filter->m_w, m_filter->m_h );
					m_filter->m_filters = (filters*)sl_push( m_filter->m_filters, filters_create( Fl::event_x() - x(),Fl::event_y() - y(), 50, 50, in, in->name() ) );
					m_filter->m_filters->node->init_widgets();
					window()->end();
					redraw();
				}
			}
			return 1;
		case FL_PUSH:
			for ( filters* i = m_filter->m_filters; i; i = i->next ) {
				if ( inside_filter( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) {
					m_current = i;
					m_x_drag_offset = Fl::event_x() - x() - i->x;
					m_y_drag_offset = Fl::event_y() - y() - i->y;
					return 1;
				} else if ( ( m_output_drag = inside_filter_output( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) >= 0 ) {
					m_current = i;
					m_connection_x = Fl::event_x();
					m_connection_y = Fl::event_y();
					return 1;
				} else if ( ( m_input_drag = inside_filter_input( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) >= 0 ) {
					m_current = i;
					m_connection_x = Fl::event_x();
					m_connection_y = Fl::event_y();
					return 1;
				}
			}
			break;
		case FL_DRAG:
			if ( m_current && m_output_drag < 0 && m_input_drag < 0 ) {
				m_current->x = Fl::event_x() - x() - m_x_drag_offset;
				m_current->y = Fl::event_y() - y() - m_y_drag_offset;
				if ( m_current->x < 0 ) { m_current->x = 0; }
				if ( m_current->y < 0 ) { m_current->y = 0; }
				window()->redraw();
				return 1;
			} else if ( m_current && m_output_drag >= 0 ) {
				m_connection_x = Fl::event_x();
				m_connection_y = Fl::event_y();
				window()->redraw();
				return 1;
			} else if ( m_current && m_input_drag >= 0 ) {
				m_connection_x = Fl::event_x();
				m_connection_y = Fl::event_y();
				window()->redraw();
				return 1;
			}
			break;
		case FL_RELEASE:
			if ( m_current && m_output_drag < 0 && m_input_drag < 0 ) {
				if ( m_trash && Fl::event_x() > m_trash->x() && Fl::event_x() < m_trash->x()+m_trash->w() && Fl::event_y() > m_trash->y() && Fl::event_y() < m_trash->y()+m_trash->h() ) {
					if ( m_current->node == m_filter->m_sink_node ) {
						m_current->y -= 100;
					} else {

						for ( int j = 0; j < MAX_FILTER_OUT; j++ ) {
							if ( m_current->outputs[j] ) {
								m_current->outputs[j]->inputs[m_current->target_slots[j]] = 0;
								m_current->outputs[j] = 0;
								m_current->target_slots[j] = 0;
							}
						}
						for ( int j = 0; j < m_current->input_count; j++ ) {
							if ( m_current->inputs[j] ) {
								for ( int k = 0; k < MAX_FILTER_OUT; k++ ) {
									if ( m_current->inputs[j]->outputs[k] == m_current ) {
										m_current->inputs[j]->outputs[k] = 0;
										m_current->inputs[j]->target_slots[k] = 0;
									}
								}
								m_current->inputs[j] = 0;
							}
						}

						if ( m_current == m_filter->m_filters ) {
							m_filter->m_filters = m_current->next;
						} else {
							filters* filter_node = m_filter->m_filters;
							while ( filter_node->next !=  m_current ) {
								filter_node = filter_node->next;
							}
							filter_node->next = m_current->next;
						}
						delete m_current->node;
						delete m_current;
					}
					m_current = 0;
				} else {
					m_current->x = Fl::event_x() - x() - m_x_drag_offset;
					m_current->y = Fl::event_y() - y() - m_y_drag_offset;
					if ( m_current->x < 0 ) { m_current->x = 0; }
					if ( m_current->y < 0 ) { m_current->y = 0; }
					m_current = 0;
				}
				window()->redraw();
				do_callback();
				return 1;
			} else if ( m_current && m_output_drag >= 0 ) {
				int target_input = -1;
				for ( filters* i = m_filter->m_filters; i; i = i->next ) {
					if ( i == m_current ) {
						continue;
					}
					if ( ( target_input = inside_filter_input( i, Fl::event_x() - x(), Fl::event_y() - y() ) ) >= 0 ) {
						int output_slot = 0;
						while ( m_current->outputs[output_slot] && output_slot < MAX_FILTER_OUT - 1 ) {
							output_slot++;
						}
						filters* old_unrelated_src = i->inputs[target_input];
						if ( old_unrelated_src ) {
							for ( int j = 0; j < MAX_FILTER_OUT; j++ ) {
								if ( old_unrelated_src->outputs[j] == i && old_unrelated_src->target_slots[j] == target_input ) {
									old_unrelated_src->outputs[j] = 0;
									old_unrelated_src->target_slots[j] = 0;
									break;
								}
							}
						}
						i->inputs[target_input] = m_current;
						m_current->outputs[output_slot] = i;
						m_current->target_slots[output_slot] = target_input;
						break;
					}
				}
				m_current = 0;
				m_output_drag = -1;
				do_callback();
				window()->redraw();
				return 1;
			} else if ( m_current && m_input_drag >= 0 ) {
				m_current = 0;
				m_input_drag = -1;
				do_callback();
				window()->redraw();
				return 1;
			}
			break;
	}
	return Fl_Widget::handle( event );
}
void Frei0rGraphEditor::addNode( INodeFilterFactoryPlugin* ffp )
{
	window()->begin();
	m_filter->m_filters = (filters*)sl_push( m_filter->m_filters, filters_create( 10,10, 50, 50, ffp->get_i_node( m_filter->m_w, m_filter->m_h ), ffp->name() ) );
	m_filter->m_filters->node->init_widgets();
	window()->end();
	redraw();
}
void Frei0rGraphEditor::init_all_widgets()
{
	window()->begin();
	for ( filters* i = m_filter->m_filters; i; i = i->next ) {
		i->node->init_widgets();
	}
	window()->end();
	redraw();
}
void Frei0rGraphEditor::setFilter( nle::NodeFilter* filter )
{
	m_filter = filter;
	assert(m_filter);
	init_all_widgets();
}

