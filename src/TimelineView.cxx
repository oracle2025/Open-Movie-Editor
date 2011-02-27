/*  TimelineView.cxx
 *
 *  Copyright (C) 2005-2008 Richard Spindler <richard.spindler AT gmail.com>
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

#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/filename.H>

#include "sl/sl.h"

#include "nle.h"
#include "TimelineView.H"
#include "Timeline.H"
#include "SwitchBoard.H"
#include "VideoClip.H"
#include "VideoTrack.H"
#include "MoveDragHandler.H"
#include "SelectDragHandler.H"
#include "TrimDragHandler.H"
#include "Rect.H"
#include "FilmStrip.H"
#include "helper.H"
#include "ErrorDialog/IErrorHandler.H"
#include "AudioClip.H"
#include "AutomationDragHandler.H"
#include "IClipArtist.H"
#include "IPlaybackCore.H"
#include "DocManager.H"
#include "Commands.H"
#include "IVideoEffect.H"
#include "IEffectDialog.H"
#include "PasteSelectionCommand.H"
#include "TitleClip.H"
#include "AutoTrack.H"
#include "AutoDragHandler.H"
#include "XmlClipData.H"
#include "IVideoFile.H"
#include "VideoFileFactory.H"
#include "ProgressDialog/ProgressDialog.h"
#include "FilterFactory.H"

#include "audio.xpm"
#include "video.xpm"
#include "Flmm_Cursor_Shape.H"
#include "../cursors/clip_a_cursor.crsr"
#include "../cursors/clip_b_cursor.crsr"
#include "fps_helper.H"

namespace nle
{
int get_track_top( Track* track );
bool USING_AUDIO = 0;
	
TimelineView* g_timelineView = 0;

static Fl_Cursor current_cursor;
TimelineView::TimelineView( int x, int y, int w, int, const char *label )
	: Fl_Widget( x, y, w, 200, label )
{
	g_timelineView = this;
	m_dragHandler = NULL;
	m_pasteCommand = 0;

	m_scrollPosition = 0;
	m_stylusPosition = 0;
	current_cursor = FL_CURSOR_DEFAULT;
	m_vscroll = 0;
	m_selectedClips = 0;
	m_draggedFilter = 0;
}
TimelineView::~TimelineView()
{
}
struct action_menu_struct {
	Clip* clip;
	int index;
};
void action_menu_callback( Fl_Widget*, void* data ) {
	struct action_menu_struct* tupel = (struct action_menu_struct*)data;
	tupel->clip->doAction( tupel->index );
}
#define MENU_ITEM_INIT 0, 0, 0, 0, 0, 0, 0, 0
int TimelineView::handle( int event )
{
	Clip* cl;
	if ( g_playbackCore->active() ) {
		return Fl_Widget::handle( event );
	}
	int _x = Fl::event_x();
	int _y = Fl::event_y() - y();
	switch ( event ) {
		case FL_MOUSEWHEEL:
			if ( Fl::event_shift() ) {
				float new_zoom = GetZoom() - ( Fl::event_dy() * GetZoom() / 2 );
				if ( new_zoom < 1.0 ) {
					new_zoom = 1.0;
				}
				int64_t new_scroll_position = llrint( ( ( float(_x - LEFT_TRACK_SPACING - x() ) / GetZoom() ) + m_scrollPosition ) ) - llrint( ( ( float(_x - LEFT_TRACK_SPACING - x() ) / new_zoom ) ) );
				SetZoom( new_zoom );
				m_scrollPosition = new_scroll_position;
			} else if ( Fl::event_ctrl() ) {
				move_cursor_by(Fl::event_dy() * 500000000 / GetZoom());
			} else {
				m_scrollPosition += Fl::event_dy();
				if ( m_scrollPosition < 0 ) {
					m_scrollPosition = 0;
				}
			}
			adjustScrollbar();
			redraw();
			return 1;
		case FL_PASTE:
			{
				Track* t = get_track( _x, _y );
				char *fn,*filename=strdup(Fl::event_text());
				int i=strlen(filename);
				while (i>0 && (iscntrl(filename[i]) || isspace(filename[i])) ) filename[i--]=0;
				if ( !strncmp( filename, "filter:", 7 ) || !strncmp( filename, "effect:", 7 ) ) {
					cl = get_clip( _x, _y );
					FilterClip* fc = dynamic_cast<FilterClip*>(cl);
					if ( fc ) {
						XmlClipData* xml = 0;
						if ( m_draggedFilter ) {
							xml = new XmlClipData;
							xml->m_xml = new TiXmlElement( "filter" );
							m_draggedFilter->writeXML( xml->m_xml );
						}
						clear_selection();
						Command* cmd = new FilterAddCommand( fc, filename, xml );
						submit( cmd );
						g_videoView->redraw();
						toggle_selection( fc );
						//TODO: Display Dialog
					}
					free(filename);
					return 1;
				}
				if (!strncmp(filename,"file://",7)) {
					fn=&(filename[7]); 
				} else {
					fn=filename;
				}
				if (t && !fl_filename_isdir(fn)) {
					ProgressDialog pDlg( "Loading File" );
					
					int64_t rp = get_real_position( _x, t->stretchFactor() );
					
					g_video_file_factory_progress = &pDlg;
					pDlg.start();

					Command* cmd = new AddCommand( fn, t, rp );
					g_docManager->submit( cmd );

					pDlg.end();
					g_video_file_factory_progress = 0;
					
					adjustScrollbar();
					redraw();
					g_videoView->seek( m_stylusPosition );
					g_timeline->changing();
				}
				free(filename);
			}
			return 1;
		case FL_DND_DRAG:
		case FL_DND_RELEASE:
		case FL_DND_ENTER:
		case FL_DND_LEAVE:
		case FL_ENTER:
			return 1;
	/*	case FL_ENTER:
			window()->cursor( FL_CURSOR_WE );
			return 1;*/
		case FL_LEAVE:
			window()->cursor( FL_CURSOR_DEFAULT );
			return 1;
		case FL_MOVE:
			if ( g_ui->automationsMode() != 0 ) {
				return 1;
			}
			cl = get_clip( _x, _y );
			if ( cl && ( _x < get_screen_position( cl->position(), cl->track()->stretchFactor() ) + 8 ) ) {
				if ( current_cursor != FL_CURSOR_WE ) {
					flmm_cursor( window(), &Flmm_Cursor_Shape(clip_a_cursor_crsr) );
					//window()->cursor( FL_CURSOR_WE, fl_rgb_color(254,254,254), fl_rgb_color(1,1,1) );
					current_cursor = FL_CURSOR_WE;
				}
			} else if ( cl &&  ( _x > get_screen_position( cl->position() + (cl->length()+1), cl->track()->stretchFactor() ) - 8 ) ) {
				if ( current_cursor != FL_CURSOR_NE ) {
					flmm_cursor( window(), &Flmm_Cursor_Shape(clip_b_cursor_crsr) );
					current_cursor = FL_CURSOR_NE;
				}
			} else {
				if ( current_cursor != FL_CURSOR_DEFAULT ) {
					window()->cursor( FL_CURSOR_DEFAULT );
					current_cursor = FL_CURSOR_DEFAULT;
				}
			}
			return 1;
		case FL_PUSH: {
				if ( _x > TRACK_SPACING + x() && _x < TRACK_SPACING + x() + 64 ) {
					int track_count = -1;
					for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
						track_count++;
						int yy = get_track_top( i->track );
						if ( _y > yy && _y < yy + i->track->h() ) {
							Fl_Menu_Item menuitem[] = { { "Remove Track", MENU_ITEM_INIT }, { "Move Up", MENU_ITEM_INIT }, { "Move Down", MENU_ITEM_INIT }, { "Rename", 0,0,0,FL_MENU_DIVIDER,0,0,0,0 }, { "1x", MENU_ITEM_INIT }, { "2x", MENU_ITEM_INIT }, { "4x", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
							Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( TRACK_SPACING + x(), yy + i->track->h() + y() + 1 );
							if ( r == &menuitem[0] ) {
								clear_selection();
								Command* cmd = new RemoveTrackCommand( i->track );
								submit( cmd );
							} else if ( r == &menuitem[1] ) {
								g_timeline->trackUp( i->track );
								redraw();
							} else if ( r == &menuitem[2] ) {
								g_timeline->trackDown( i->track );
								redraw();
							} else if ( r == &menuitem[3] ) {
								const char* name = fl_input( "Please enter the track name.", i->track->name().c_str() );
								if ( name ) {
									i->track->name( name );
									redraw();
								}
							} else if ( r == &menuitem[4] ) {
								i->track->h( 30 );
								redraw();
							} else if ( i->track->type() == TRACK_TYPE_AUDIO && r == &menuitem[5] ) {
								i->track->h( 60 );
								redraw();
							} else if ( r == &menuitem[6] ) {
								i->track->h( 120 );
								redraw();
							} else if ( i->track->type() == TRACK_TYPE_VIDEO && r == &menuitem[5] ) {
								i->track->h( 65 );
								redraw();
							}
							return 1;
						}
					}
				}
				/*
				If Mouse in AutoTrack, return an DragHandler for
				the Automations.
				if ( _x > TRACK_SPACING + x() && _x < TRACK_SPACING + x() + 64 )
				*/
				Track *tr = get_track( _x, _y );
				AutoTrack* auttr = 0;
				if ( tr ) {
					auttr = dynamic_cast<AutoTrack*>(tr);
				}
				if ( auttr && ( Fl::event_button() == FL_LEFT_MOUSE ) ) {
					Rect rect = get_track_rect( tr );
					m_dragHandler = new AutoDragHandler( auttr, rect, _x, _y );
					return 1;
				}
				cl = get_clip( _x, _y );
				VideoClip* vcl = 0;
				if ( cl ) {
					vcl = dynamic_cast<VideoClip*>(cl);
				}
				if ( cl && Fl::event_ctrl() ) {
					toggle_selection( cl );
					return 1;
				}
				if ( cl && g_ui->automationsMode() == 2 ) {
					return 1;
				}
				if ( cl && vcl && vcl->hasAudio() && ( Fl::event_button() == FL_RIGHT_MOUSE ) ) {
					if ( vcl->m_mute ) {
						Fl_Menu_Item menuitem[] = { { "Unmute Original Sound", MENU_ITEM_INIT },
							{ "Select all Clips after Cursor", MENU_ITEM_INIT }, 
							{ 0L, MENU_ITEM_INIT } };
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vcl->m_mute = false;
							redraw();
							g_timeline->changing();
						} else if ( r == &menuitem[1] ) {
							select_all_after_cursor();	
						}
					} else {
						Fl_Menu_Item menuitem[] = { { "Mute Original Sound", MENU_ITEM_INIT }, 
							{ "Select all Clips after Cursor", MENU_ITEM_INIT }, 
							{ 0L, MENU_ITEM_INIT } };
						Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
						if ( r == &menuitem[0] ) {
							vcl->m_mute = true;
							redraw();
							g_timeline->changing();
						} else if ( r == &menuitem[1] ) {
							select_all_after_cursor();	
						}
					}
					return 1;
				}
				VideoEffectClip* vec;
				if ( ( vec = dynamic_cast<VideoEffectClip*>(cl) ) && ( Fl::event_button() == FL_RIGHT_MOUSE ) ) {
					struct action_menu_struct action_menu_tuples[cl->getActionCount()];
					Fl_Menu_Item action_menu[cl->getActionCount()+1];
					//Fl_Menu_ action_menu(0,0,0,0);
					for ( int i = 0; i < cl->getActionCount(); i++ ) {
						action_menu_tuples[i].clip = cl;
						action_menu_tuples[i].index = i;
						action_menu[i].text = cl->getActionName( i );
						action_menu[i].shortcut_ = 0;
						action_menu[i].callback_ = (Fl_Callback*)action_menu_callback;
						action_menu[i].user_data_ = &(action_menu_tuples[i]);
						action_menu[i].flags = 0;
						action_menu[i].labeltype_ = FL_NORMAL_LABEL;
						action_menu[i].labelfont_ = FL_HELVETICA;
						action_menu[i].labelsize_ = 14;
						action_menu[i].labelcolor_ = FL_BLACK;
					}
					action_menu[cl->getActionCount()].text = 0;

					Fl_Menu_Item menuitem[] = { { "Actions", 0,0,action_menu,FL_SUBMENU_POINTER,0,0,0,0},
						{ "Select all Clips after Cursor", MENU_ITEM_INIT }, 
						{ 0L, MENU_ITEM_INIT } };
					Fl_Menu_Item* r = (Fl_Menu_Item*)menuitem->popup( Fl::event_x(), Fl::event_y() );
					if ( r && r->callback() ) {
						 Fl_Callback* cb = r->callback();
						 cb( 0, r->user_data() );
					}
					if ( r == &menuitem[1] ) {
						select_all_after_cursor();	
					}
					return 1;
				}
				if ( Fl::event_button() == FL_RIGHT_MOUSE ) {
					Fl_Menu_Item menuitem[] = { { "Select all Clips after Cursor", MENU_ITEM_INIT }, { 0L, MENU_ITEM_INIT } };
					if ( menuitem->popup( Fl::event_x(), Fl::event_y() ) ) {
						select_all_after_cursor();
					}
					return 1;
				}
				AudioClip* audioClip = dynamic_cast<AudioClip*>(cl);
				if ( cl && g_ui->automationsMode() == 1 && audioClip ) {
					Rect r = get_clip_rect( cl, true );
					m_dragHandler = audioClip->onMouseDown( r, _x, _y, FL_SHIFT & Fl::event_state() );
					return 1;
				} else if ( cl ) {
					if ( g_lock ) {
						clear_selection();
						toggle_selection( cl );
					} else {
						if ( _x < get_screen_position( cl->position(), cl->track()->stretchFactor() ) + 8 ) {
							m_dragHandler = new TrimDragHandler(
									this, cl, cl->track()->num(),
									0, 0, false );
						} else if ( _x > get_screen_position( cl->position() + (cl->length()+1), cl->track()->stretchFactor() ) - 8) {
							m_dragHandler = new TrimDragHandler(
									this, cl, cl->track()->num(),
									0, 0, true );
						} else {
							m_dragHandler = new MoveDragHandler(
									this, cl, _x, _y, get_clip_rect( cl, false )
									);
						}
					}
					return 1;
				}
			}
			m_dragHandler = new SelectDragHandler( _x, _y );
			return 1;
		case FL_DRAG:
			if ( m_dragHandler ) {
				m_dragHandler->OnDrag( _x, _y );
				return 1;
			}
			return Fl_Widget::handle( event );
		case FL_RELEASE:
			if ( m_dragHandler ) {
				m_dragHandler->OnDrop( _x, _y );
				delete m_dragHandler;
				m_dragHandler = NULL;
				redraw();
				g_videoView->seek( m_stylusPosition );
				g_timeline->changing();
				return 1;
			}
			if ( g_ui->automationsMode() == 2 ) {
				cl = get_clip( _x, _y );
				if ( cl ) {
					split_clip( cl, _x );
					redraw();
					g_timeline->changing();
					return 1;
				}
			}
		case FL_SHORTCUT: //patch by raaf from OME Forums
			{
				//Take m_playback_fps into account here!!
				int key = Fl::event_key();
				int64_t framelen = single_frame_length( g_timeline->m_playback_fps );
				if( framelen > 0 && key == FL_KP + '4')
					move_cursor_by((-1) * framelen);
				else if(framelen > 0 && key ==  FL_KP + '6')
					move_cursor_by(framelen);
				else if(framelen > 0 && key == FL_KP + '2')
					move_cursor_by((-5)*framelen);
				else if(framelen > 0 && key == FL_KP + '8')
					move_cursor_by(5*framelen);
				else if(key == 's') //patch by dreamlx from OME Forums
				{
					cl = get_clip(get_screen_position(m_stylusPosition), _y);
					if (cl)
					{
						split_clip(cl, get_screen_position(m_stylusPosition));
						redraw();
						g_timeline->changing();
					}

				}

				else
					return Fl_Widget::handle( event );
			}
			return 1;
		default:
			return Fl_Widget::handle( event );	
	}
	
}
void TimelineView::resize(int x, int y, int w, int h)
{
	int h_t = 2 * TRACK_SPACING;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		h_t += i->track->h() + TRACK_SPACING;
	}
	int h_r = h_t > h ? h_t : h;
	Fl_Widget::resize( x, y, w, h_r );
	int a = g_v_scrollbar->value();
	int b = h_r - parent()->h();
	int v = a <= b ? a : b;
	if ( v < 0 ) { v = 0; }
	g_v_scrollbar->value( v, parent()->h(), 0, h_r );
	adjustScrollbar();
}
void TimelineView::draw()
{
	fl_overlay_clear();
	fl_push_clip( x(), y(), w(), h() );


//     - Draw Background
	fl_draw_box( FL_FLAT_BOX, x(), y(), w(), h(), FL_BACKGROUND_COLOR );
// END - Draw Background

	int track_count = -1;
	int y_coord = y() + TRACK_SPACING;
	for ( track_node* i = g_timeline->getTracks(); i; i = i->next ) {
		Track* track = i->track;
		track_count++;
		int x_coord = x() + LEFT_TRACK_SPACING;
		int w_size = w() - TRACK_SPACING - LEFT_TRACK_SPACING;
		
		USING_AUDIO = track->type() == TRACK_TYPE_AUDIO;
		
	//     - Draw Button
		fl_draw_box( FL_UP_BOX, x() + TRACK_SPACING, y_coord, 64, track->h() + 1, FL_BACKGROUND_COLOR );
		fl_color( FL_BLACK );
		fl_font( FL_HELVETICA, 11 );
		if ( USING_AUDIO ) {
			fl_draw( track->name().c_str(), x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( audio_xpm, x() + TRACK_SPACING + 3, y_coord + 5 );
		} else {
			fl_draw( track->name().c_str(), x() + TRACK_SPACING + 23, y_coord + 18);
			fl_draw_pixmap( video_xpm, x() + TRACK_SPACING + 6, y_coord + 8 );
		}
	// END - Draw Button

	//     - Draw Track Background
		fl_draw_box( FL_BORDER_BOX, x_coord, y_coord, w_size, track->h(), FL_DARK2 );
	// END - Draw Track Background

		fl_push_clip( x_coord, y_coord, w_size, track->h() );
		
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int64_t scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int64_t scr_clip_y = y_coord;
			int64_t scr_clip_w = llrint( (clip->length() + 1) * GetZoom() / track->stretchFactor() );
			int64_t scr_clip_h = track->h();

			if ( scr_clip_x + scr_clip_w < 0 )
				continue;
			if ( scr_clip_x > (int64_t)w() + (int64_t)x() )
				continue;

			if ( scr_clip_x < (int64_t)x() ) {
				scr_clip_w = scr_clip_w + scr_clip_x - x() + 5;
				scr_clip_x = x() - 5;
			}

			if ( scr_clip_x + scr_clip_w > (int64_t)w() + (int64_t)x() ) {
				scr_clip_w = (int64_t)(x() + w()) - scr_clip_x;
			}

			
			
			fl_draw_box( FL_BORDER_BOX , scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_DARK3 );
			
			Rect r( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );
			IClipArtist* artist = clip->getArtist();
			if ( artist ) {
				artist->render( r, get_real_position( scr_clip_x, track->stretchFactor() ),
						get_real_position( scr_clip_x + scr_clip_w, track->stretchFactor() ) );
			}
			if ( clip->selected() ) {
				fl_draw_box( FL_BORDER_FRAME, scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_RED );
			} else {
				fl_draw_box( FL_BORDER_FRAME, scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h, FL_DARK3 );
			}
		}
		if ( dynamic_cast<VideoTrack*>(track) ) {
			for ( clip_node* j = track->getClips(); j; j = j->next ) {
				Clip* clip = j->clip;
				for ( clip_node* k = j->next; k; k = k->next ) {
					Clip* cclip = k->clip;
					if ( clip->A() < cclip->A() && clip->B() > cclip->A() ) {
						int x = get_screen_position( cclip->A(), track->stretchFactor() );
						int w = get_screen_position( clip->B()+1, track->stretchFactor() ) - x;
						fl_draw_box( FL_FLAT_BOX, x, y_coord, w, track->h(), FL_DARK_BLUE );
						fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, track->h(), FL_RED );
						fl_color( FL_RED );
						fl_line( x, y_coord, x + w, y_coord + track->h());
						fl_line( x + w, y_coord, x, y_coord + track->h());
					} else if ( cclip->A() < clip->A() && cclip->B() > clip->A() ) {
						int x = get_screen_position( clip->A(), track->stretchFactor() );
						int w = get_screen_position( cclip->B()+1, track->stretchFactor() ) - x;
						fl_draw_box( FL_FLAT_BOX, x, y_coord, w, track->h(), FL_DARK_BLUE );
						fl_draw_box( FL_BORDER_FRAME, x, y_coord, w, track->h(), FL_RED );
						fl_color( FL_RED );
						fl_line( x, y_coord, x + w, y_coord + track->h());
						fl_line( x + w, y_coord, x, y_coord + track->h());
					}

				}
			}
		}
		for ( clip_node* j = track->getClips(); j; j = j->next ) {
			Clip* clip = j->clip;
			int scr_clip_x = get_screen_position( clip->position(), track->stretchFactor() );
			int scr_clip_y = y_coord;
			int scr_clip_w = (int)( (clip->length()+1) * GetZoom() / track->stretchFactor() );
			for ( int k = 0; k < 2; k++ ) {
				if ( !k ) {
					fl_color( FL_BLACK );
					fl_begin_polygon();
				} else {
					fl_color( FL_DARK2 );
					fl_begin_loop();
				}
				fl_vertex( scr_clip_x, scr_clip_y + track->h()/ 2 );
				fl_vertex( scr_clip_x + 8, scr_clip_y + track->h()/ 2 - 5 );
				fl_vertex( scr_clip_x + 8, scr_clip_y + track->h()/ 2 + 5 );
				if ( !k ) {
					fl_end_polygon();
					fl_begin_polygon();
				} else {
					fl_end_loop();
					fl_begin_loop();
				}
				fl_vertex( scr_clip_x + scr_clip_w, scr_clip_y + track->h()/ 2 );
				fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + track->h()/ 2 - 5 );
				fl_vertex( scr_clip_x + scr_clip_w - 8, scr_clip_y + track->h()/ 2 + 5 );
				if ( !k ) {
					fl_end_polygon();
				} else {
					fl_end_loop();
				}
			}
		}

		if ( AutoTrack* autt = dynamic_cast<AutoTrack*>(track) ) {
			int64_t scr_clip_x = x_coord;
			int64_t scr_clip_y = y_coord;
			int64_t scr_clip_w = w_size;
			int64_t scr_clip_h = track->h();
			Rect rect( scr_clip_x, scr_clip_y, scr_clip_w, scr_clip_h );
			auto_node* nodes = autt->getAutomationPoints();

			float stretchF = (float)NLE_TIME_BASE;
			fl_color( FL_RED );
			if ( nodes ) {
				int y =(int)( y_coord + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) + 5 );
				fl_line( x_coord, y, g_timelineView->get_screen_position( nodes->x, stretchF ), y );
			}
			for ( ; nodes && nodes->next; nodes = nodes->next ) {
				int y = (int)( rect.y + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) + 5 );
				int y_next = (int)( rect.y + ( ( track->h() - 10 ) * ( 1.0 - nodes->next->y ) ) + 5 );
				fl_line( g_timelineView->get_screen_position( nodes->x, stretchF ),
						y,
						g_timelineView->get_screen_position( nodes->next->x, stretchF ),
						y_next );
			}
			if ( nodes ) {
				int y =(int)( y_coord + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) + 5 );
				fl_line( g_timelineView->get_screen_position( nodes->x, stretchF ), y, x_coord + w_size, y );
			}
			nodes = autt->getAutomationPoints();
			for ( ; nodes; nodes = nodes->next ) {
				//consider Trimming
				int x;
				int y = (int)( rect.y + ( ( track->h() - 10 ) * ( 1.0 - nodes->y ) ) );
				x = g_timelineView->get_screen_position( nodes->x, stretchF );
				fl_draw_box( FL_UP_BOX, x, y, 10, 10, FL_RED );
			}

		}

		fl_pop_clip();
		y_coord += TRACK_SPACING + i->track->h();
	}
	fl_pop_clip();
	draw_cursor();       
}
int64_t TimelineView::get_real_position( int p, float stretchFactor )
{
	return llrint( ( ( float(p - LEFT_TRACK_SPACING - x() ) / GetZoom() ) + m_scrollPosition ) * stretchFactor );
}
int64_t TimelineView::get_screen_position( int64_t p, float stretchFactor )
{
	return llrint( float( float(p) - ( m_scrollPosition * stretchFactor ) ) * GetZoom() / stretchFactor ) + LEFT_TRACK_SPACING + x();

}
void TimelineView::scroll( int64_t position )
{
	m_scrollPosition = position;
}
void TimelineView::zoom( float zoom )
{
  bool insane = isinf(zoom) || isnan(zoom) || zoom >= 300.0;
  if (!insane) {SetZoom( zoom );}
  redraw();
  g_ruler->stylus( get_screen_position(m_stylusPosition) );
}
Track* TimelineView::get_track( int _x, int _y )
{
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		if ( !get_track_rect( o->track ).inside( _x, _y ) ) {
			continue;
		}
		return o->track;
	}
	return NULL;
}
Clip* TimelineView::get_clip( int _x, int _y )
{
	Track *tr = get_track( _x, _y );
	if ( !tr ) {
		return NULL;
	}
	for ( clip_node* p = tr->getClips(); p; p = p->next ) {
		Rect tmp = get_clip_rect( p->clip, true );
		if ( !tmp.inside( _x, _y ) )
			continue;
		return p->clip;
	}
	return NULL;
}
int get_track_top( Track* track )
{
	int top = TRACK_SPACING;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		if ( o->track == track ) {
			return top;
		}
		top += o->track->h() + TRACK_SPACING;
	}
	return top;	
}
int get_track_top( int track )
{
	int top = TRACK_SPACING;
	int t = 0;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		if ( t == track ) {
			return top;
		}
		top += o->track->h() + TRACK_SPACING;
		t++;
	}
	return top;	
}
Rect TimelineView::get_track_rect( Track* track )
{
	Rect tmp(
			x() + LEFT_TRACK_SPACING,
			get_track_top( track ),
			w() - ( TRACK_SPACING + LEFT_TRACK_SPACING ),
			track->h()
		);
	return tmp;
}
Rect TimelineView::get_clip_rect( Clip* clip, bool clipping )
{
	Rect tmp(
			get_screen_position( clip->position(), clip->track()->stretchFactor() ),
			get_track_top( clip->track() ),
			int( (clip->length()+1) * GetZoom() / clip->track()->stretchFactor() ),
			clip->track()->h()
		);
	if ( clipping ) {
		if ( tmp.x < LEFT_TRACK_SPACING + x() ) {
			tmp.w += tmp.x - ( LEFT_TRACK_SPACING + x() );
			tmp.x = LEFT_TRACK_SPACING + x();
		}
		if ( tmp.w + tmp.x > w() - TRACK_SPACING + x() ) {
			tmp.w = ( w() - TRACK_SPACING + x() ) - tmp.x;
		}
	}
	return tmp;
}

bool inside_widget( Fl_Widget* widget, int x, int y )
{
	int w_x = widget->x();
	int w_y = widget->y();
	int w_w = widget->w();
	int w_h = widget->h();
	return ( x >= w_x && x <= w_x + w_w && y >= w_y && y <= w_y + w_h );
}

void TimelineView::move_clip( Clip* clip, int _x, int _y, int offset )
{
	Track *new_tr = get_track( _x, _y );
	Track *old_tr = clip->track();
	bool clear = true;
	for ( clip_node* n = m_selectedClips; n; n = n->next ) {
		if ( n->clip == clip ) {
			clear = false;
			break;
		}
	}
	if ( clear ) {
		clear_selection();
	}
	if ( inside_widget( g_trashCan, _x, y() + _y ) ) {
		Command* cmd;
		if ( m_selectedClips ) {
			cmd = new RemoveSelectionCommand( m_selectedClips );
			submit( cmd );
			adjustScrollbar();
			clear_selection();
			return;
		} else {
			cmd = new RemoveCommand( clip );
			g_docManager->submit( cmd );
			adjustScrollbar();
			return;
		}
	}
	if (!new_tr || new_tr->type() != old_tr->type() ) {
		return;
	}
	int64_t new_position = get_real_position( _x - offset, clip->track()->stretchFactor() );
	if ( g_snap ) {
		new_position = new_tr->getSnapA( clip, new_position );
		new_position = new_tr->getSnapB( clip, new_position );
	}
	if ( new_position < 0 ) {
		new_position = 0;
	}
	Command* cmd;
	/* -- BEGIN CHECK NUMBER OF SELECTED TRACKS -- */
	int track_count = 0;
	int last_track = -1;
	for ( clip_node* p = m_selectedClips; p; p = p->next ) {
		if ( last_track != p->clip->track()->num() ) {
		       track_count++;
		}
 		last_track = p->clip->track()->num();
		if ( track_count > 1 ) {
			break;
		}
	}
	/* -- END CHECK NUMBER OF SELECTED TRACKS -- */
	if ( m_selectedClips && ( new_tr == old_tr || track_count == 1 ) ) {
		if ( track_count == 1 ) {
			cmd = new MoveSelectionCommand( clip, new_tr, new_position, m_selectedClips );
		} else {
			cmd = new MoveSelectionCommand( clip, 0, new_position, m_selectedClips );
		}
	} else {
		clear_selection();
		cmd = new MoveCommand( clip, new_tr, new_position );
		toggle_selection( clip );
	}
	submit( cmd );
	adjustScrollbar();
}
void TimelineView::add_track( int type )
{
	if ( g_playbackCore->active() ) {
		return;
	}
	Command* cmd = new AddTrackCommand( type );
	submit( cmd );
}
void TimelineView::adjustScrollbar()
{
  long scale = long( w() / GetZoom() );
  g_scrollBar->value( m_scrollPosition, scale,0, g_timeline->length() + scale );
}
void TimelineView::split_clip( Clip* clip, int _x )
{
	int64_t split_position = get_real_position(_x, clip->track()->stretchFactor() );
	Command* cmd = new SplitCommand( clip, split_position );
	g_docManager->submit( cmd );
}
void TimelineView::clear_selection()
{
	clip_node* node;
	while ( ( node = (clip_node*)sl_pop( &m_selectedClips ) ) ) {
		node->clip->selected( false );
		delete node;
	}
	updateEffectDisplay();
	setSelectionButtons();
}
void TimelineView::select_clips( int _x1, int _y1, int _x2, int _y2 )
{
	clear_selection();
	if ( _x1 > _x2 ) { int k; k = _x1; _x1 = _x2; _x2 = k; }
	if ( _y1 > _y2 ) { int k; k = _y1; _y1 = _y2; _y2 = k; }
	Rect r;
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		r = get_track_rect( o->track );
		if ( !( _y1 <= y() + r.y && _y2 >= y() + r.y + r.h ) ) {
			continue;
		}
		for ( clip_node* c = o->track->getClips(); c; c = c->next ) {
			r = get_clip_rect( c->clip, false );
			if ( !(_x1 <= r.x && _x2 >= r.x + r.w) ) {
				continue;
			}
			// Add Clip to Selection and mark them
			clip_node* n = new clip_node;
			n->next = 0;
			n->clip = c->clip;
			c->clip->selected( true );
			m_selectedClips = (clip_node*)sl_push( m_selectedClips, n );
		}
	}
	updateEffectDisplay();
	setSelectionButtons();
}
void TimelineView::select_all_after_cursor()
{
	clear_selection();
	for ( track_node* o = g_timeline->getTracks(); o; o = o->next ) {
		for ( clip_node* c = o->track->getClips(); c; c = c->next ) {
			if ( c->clip->position() / c->clip->track()->stretchFactor() * NLE_TIME_BASE >= m_stylusPosition ) {
				clip_node* n = new clip_node;
				n->next = 0;
				n->clip = c->clip;
				c->clip->selected( true );
				m_selectedClips = (clip_node*)sl_push( m_selectedClips, n );
			}
		}
	}
	updateEffectDisplay();
	setSelectionButtons();
	redraw();
}
static int remove_clip_helper( void* p, void* data )
{
	Clip* clip = (Clip*)data;
	clip_node* node = (clip_node*)p;
	if ( node->clip == clip ) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * Determine if there is a next clip, or not
 */
bool TimelineView::has_next_clip()
{
  /* relies on "short-curcuiting" */
  return (!m_selectedClips || m_selectedClips->next);
}

void TimelineView::updateTitlesDisplay()
{
  if(has_next_clip()) {
    g_ui->deactivate_titles();
    return;
  }
	TitleClip* tc = dynamic_cast<TitleClip*>( m_selectedClips->clip );
	if ( !tc ) {
		g_ui->deactivate_titles();
		return;
	}
	g_ui->activate_titles( tc->font(), tc->size(), tc->text(), tc->x(), tc->y(), tc->color() );
}
void TimelineView::titles_text( const char* t )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->text( t );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_x( float x )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->x( x );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_y( float y )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->y( y );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_size( int size )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->size( size );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_font( int font )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->font( font );
	tc->touch();
	g_videoView->redraw();
}
void TimelineView::titles_color( Fl_Color color )
{
	TitleClip* tc = getTitleClip();
	if ( !tc ) {
		return;
	}
	tc->color( color );
	tc->touch();
	g_videoView->redraw();
}
TitleClip* TimelineView::getTitleClip()
{
  if(has_next_clip()) { return 0; }
  return dynamic_cast<TitleClip*>( m_selectedClips->clip );
}

void TimelineView::updateEffectDisplay()
{
	updateTitlesDisplay();
	if (has_next_clip()) {
		g_ui->m_effectMenu->deactivate();
		return;
	}
	/*
	VideoClip* vidclip = dynamic_cast<VideoClip*>( m_selectedClips->clip );
	if ( vidclip ) {
		string interlacing = "none";
		if ( vidclip->interlacing() == INTERLACE_PROGRESSIVE ) {
			interlacing = "progressive";
		} else if ( vidclip->interlacing() == INTERLACE_TOP_FIELD_FIRST ) {
			interlacing = "top field first";
		} else if ( vidclip->interlacing() == INTERLACE_BOTTOM_FIELD_FIRST ) {
			interlacing = "bottom field first";
		}
		string aspect;
		frame_struct frame;
		guess_aspect( vidclip->w(), vidclip->h(), &frame );
		stringstream aspectstream;
		aspectstream << frame.pixel_aspect_ratio;
		aspect = aspectstream.str();
		string decoder = vidclip->file()->decoder();
		string framerate;
		stringstream frameratestream;
		
		frameratestream << ( (float)NLE_TIME_BASE / (float)vidclip->file()->ticksPerFrame() );
		framerate = frameratestream.str();
		g_ui->activate_clip( fl_filename_name(vidclip->filename().c_str()), vidclip->filename().c_str(), decoder.c_str(), framerate.c_str(), aspect.c_str(), interlacing.c_str() );
	}
	*/
	FilterClip* vc = dynamic_cast<FilterClip*>( m_selectedClips->clip );
	g_ui->filter_scroll->setClip( vc );
	if ( !vc ) {
		g_ui->m_effectMenu->deactivate();
		return;
	}
	g_ui->m_effectMenu->activate();
}
void TimelineView::toggle_selection( Clip* clip )
{
	if ( clip->selected() ) {
		clip->selected( false );
		clip_node* n = (clip_node*)sl_remove( &m_selectedClips, remove_clip_helper, clip );
		if ( n ) {
			delete n;
		}
	} else {
		clip->selected( true );
		clip_node* n = new clip_node;
		n->next = 0;
		n->clip = clip;
		clip->selected( true );
		m_selectedClips = (clip_node*)sl_push( m_selectedClips, n );
	}
	updateEffectDisplay();
	setSelectionButtons();
	redraw();
}
void TimelineView::setSelectionButtons()
{
	if ( m_selectedClips ) {
		g_ui->cut_item->activate();
		g_ui->copy_item->activate();
		g_ui->delete_item->activate();
	} else {
		g_ui->cut_item->deactivate();
		g_ui->copy_item->deactivate();
		g_ui->delete_item->deactivate();
	}
	if ( m_pasteCommand ) {
		g_ui->paste_item->activate();
	} else {
		g_ui->paste_item->deactivate();
	}

}


void TimelineView::addEffect( FilterFactory* effectFactory )
{
	if ( g_playbackCore->active() ) {
		return;
	}
  if(has_next_clip()) {return;}

	FilterClip* fc = dynamic_cast<FilterClip*>( m_selectedClips->clip );
	if ( !fc ) {
		return;
	}
	FilterAddCommand* cmd = new FilterAddCommand( fc, effectFactory->identifier() );
	submit( cmd );
	
	FilterBase* fe = cmd->m_filter;
	assert(fe);
	updateEffectDisplay();
	g_videoView->redraw();
/*	IEffectDialog* dialog = fe->dialog();
	//TODO: VideoEffects: move Stylus to start of clip if it is not inside the clip.

	IVideoEffect* ive = dynamic_cast<IVideoEffect*>(fe);
	if ( ive && ive->numParams() && dialog ) {
		dialog->show();
	}*/
}

void TimelineView::trim_clip( Clip* clip, int _x, bool trimRight )
{
	Command* cmd = new TrimCommand( clip, get_real_position( _x, clip->track()->stretchFactor() ), trimRight );
	g_docManager->submit( cmd );
	adjustScrollbar();
}

/**
 * move the cursor by a CHANGE amount
 */
void TimelineView::move_cursor_by(int64_t change)
{
  move_cursor(m_stylusPosition+change);
}

void TimelineView::move_cursor( int64_t position )
{
	m_stylusPosition = position;
	if ( m_stylusPosition < 0 ) {
		m_stylusPosition = 0;
	}
	int64_t framelen = single_frame_length( g_timeline->m_playback_fps );
	if ( framelen > 0 ) {
		m_stylusPosition = m_stylusPosition - ( m_stylusPosition % framelen );
	}
	window()->make_current();
	long screen_pos = get_screen_position(m_stylusPosition);
	if ( screen_pos > w() + x() - 30 ) {
		m_scrollPosition += (int64_t)( ( 30 - x() - w() + screen_pos ) / GetZoom() );
		adjustScrollbar();
	} else if ( screen_pos < x() + LEFT_TRACK_SPACING + 20 && m_scrollPosition > 0 ) {
		m_scrollPosition -= (int64_t)( ( 20 - ( screen_pos - x() - LEFT_TRACK_SPACING ) ) / GetZoom() );
		if ( m_scrollPosition < 0 ) { m_scrollPosition = 0; }
		adjustScrollbar();
	}
	g_ruler->stylus( get_screen_position(m_stylusPosition) );	
	g_videoView->seek( position );
	g_timeline->seek( position );
	redraw();
	

	// signal histogram view
/*	if ( m_selectedClips && !m_selectedClips->next ) {
		VideoEffectClip* vc = dynamic_cast<VideoEffectClip*>( m_selectedClips->clip );
		Clip* clip = m_selectedClips->clip;
		if ( vc && m_stylusPosition >= clip->A() && m_stylusPosition <= clip->B() ) {
			g_histogram->setVideoClip( vc, m_stylusPosition );
		}

	}*/
}
void TimelineView::stylus( long stylus_pos )
{
	move_cursor( get_real_position( stylus_pos ) );
	g_ui->m_timecode_box->label( timestamp_to_string( m_stylusPosition ) );
}

/**
 * draw the cursor (vertical line) onto the timelineview
 */
void TimelineView::draw_cursor()
{
  fl_overlay_rect( get_screen_position( m_stylusPosition ), 
		   parent()->y(), 1, parent()->h() );
}
void TimelineView::cut()
{
	if ( g_playbackCore->active() ) {
		return;
	}
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_pasteCommand ) {
		delete m_pasteCommand;
		m_pasteCommand = 0;
	}
	m_pasteCommand = new PasteSelectionCommand( m_selectedClips );
	Command* cmd = new RemoveSelectionCommand( m_selectedClips );
	submit( cmd );
	adjustScrollbar();
	clear_selection();
	setSelectionButtons();
}
void TimelineView::copy()
{
	if ( !m_selectedClips ) {
		return;
	}
	if ( m_pasteCommand ) {
		delete m_pasteCommand;
		m_pasteCommand = 0;
	}
	m_pasteCommand = new PasteSelectionCommand( m_selectedClips );
	setSelectionButtons();
}
void TimelineView::paste()
{
	if ( g_playbackCore->active() ) {
		return;
	}
	if ( !m_pasteCommand ) {
		return;
	}
	PasteSelectionCommand* cmd = m_pasteCommand;
	m_pasteCommand = new PasteSelectionCommand( cmd );
	cmd->position( m_stylusPosition );
	submit( cmd );
}
void TimelineView::remove()
{
	if ( g_playbackCore->active() ) {
		return;
	}
	if ( !m_selectedClips ) {
		return;
	}
	Command* cmd = new RemoveSelectionCommand( m_selectedClips );
	submit( cmd );
	adjustScrollbar();
	clear_selection();
	setSelectionButtons();
}
void TimelineView::help()
{
// Create a HelpDialog and show the Help
	static Fl_Help_Dialog* help = 0;
	if ( !help ) {
		help = new Fl_Help_Dialog;
	}
	struct stat buf;
	if ( stat( INSTALL_PREFIX "/share/doc/openmovieeditor/tutorial.html", &buf ) == 0 ) {
		help->load( INSTALL_PREFIX "/share/doc/openmovieeditor/tutorial.html" );
		help->show();
	} else if ( stat( "../doc/tutorial.html", &buf ) == 0 ) {
		help->load( "../doc/tutorial.html" );
		help->show();
	} else {
		fl_alert( "Help File was not found." );
	}
}

} /* namespace nle */
