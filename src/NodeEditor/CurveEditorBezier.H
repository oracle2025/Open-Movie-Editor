#ifndef _CURVE_EDITOR_BEZIER_H_
#define _CURVE_EDITOR_BEZIER_H_

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

typedef struct _point_list {
	struct _point_list* next;
	float x;
	float y;
	float pre_x;
	float pre_y;
	float post_x;
	float post_y;
} point_list;

class CurveEditorBezier : public Fl_Widget
{
	public:
		CurveEditorBezier( int x, int y, int w, int h, const char* label = 0 );
		~CurveEditorBezier();
		void draw();
		int handle( int event );
		void setValues( unsigned char* val ); // Size of array: 256
		point_list *m_points;
	private:
		void calc_lut();
		void calc_curve_lut();
		point_list *m_grabbed_point;
		point_list *m_grabbed_pre;
		point_list *m_grabbed_post;
		float m_drag_lim_left;
		float m_drag_lim_right;
		unsigned char* m_values;
		void display_to_point( int x, int y, float& out_x, float& out_y );
		void point_to_display( float x, float y, int& out_x, int& out_y );
		bool m_deleted_point;
};

#endif /* _CURVE_EDITOR_BEZIER_H_ */
