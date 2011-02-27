/*  TimelineView.H
 *
 *  Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
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

#ifndef _TIMELINE_VIEW_H_
#define _TIMELINE_VIEW_H_

#include <stdint.h>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

#include "Rect.H"
#include "global_includes.H"
#include "globals.H"

namespace nle
{
#define TRACK_SPACING 5
#define LEFT_TRACK_SPACING 74

struct clip_node_;
typedef struct clip_node_ clip_node;

class Track;
class Clip;
class Timeline;
class DragHandler;
class FilterFactory;
class PasteSelectionCommand;
class TitleClip;
class FilterBase;
	
class TimelineView : public Fl_Widget
{
	public:
		TimelineView( int x, int y, int w, int h, const char *label = 0 );
		virtual ~TimelineView();
		int handle( int event );
		void resize(int x, int y, int w, int h);
		void add_video( int track, int y, string filename ); /*remove me!*/
		void scroll( int64_t position );
		Track* get_track( int _x, int _y);
		Clip* get_clip( int _x, int _y );
		Rect get_track_rect( int track );
		Rect get_track_rect( Track* track );
		Rect get_clip_rect( Clip* clip, bool clipping = true );
		void move_clip( Clip* clip, int _x, int _y, int offset );
		void trim_clip( Clip* clip, int _x, bool trimRight );
		void split_clip( Clip* clip, int _x );
		void clear_selection();
		void select_clips( int _x1, int _y1, int _x2, int _y2 );
		void select_all_after_cursor();
		void toggle_selection( Clip* clip );
		void add_track( int type );
		void move_cursor_by(int64_t change);
		void move_cursor( int64_t position );
		void stylus( long stylus_pos );
		long stylus();
		void draw_cursor();
		void zoom( float zoom );
		int64_t get_real_position( int p, float stretchFactor = NLE_TIME_BASE );
		int64_t get_screen_position( int64_t p, float stretchFactor = NLE_TIME_BASE );
		int64_t scrollPosition() { return m_scrollPosition; }
		void end() {}
		void adjustScrollbar();
		void addEffect( FilterFactory* effectFactory );
		void titles_text( const char* t );
		void titles_x( float x );
		void titles_y( float y );
		void titles_size( int size );
		void titles_font( int font );
		void titles_color( Fl_Color color );
		int m_trackHeight;
		void cut();
		void copy();
		void paste();
		void remove();
		void setSelectionButtons();
		void help();
		void updateEffectDisplay();
		FilterBase* m_draggedFilter;
	protected:
		void draw();
	private:
		TitleClip* getTitleClip();
		void updateTitlesDisplay();
		DragHandler *m_dragHandler;
		int64_t m_scrollPosition;
		int64_t m_stylusPosition;
		int m_vscroll;
		clip_node* m_selectedClips;
		PasteSelectionCommand* m_pasteCommand;
		bool has_next_clip();

};

extern TimelineView* g_timelineView;

} /* namespace nle */

#endif /* _TIMELINE_VIEW_H_ */
