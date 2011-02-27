

#include "fltk_layout_persistance.H"

void fltk_save_layout( Fl_Preferences* p, Fl_Group* g ) {
	p->set( "group_x", g->x() );
	p->set( "group_y", g->y() );
	p->set( "group_w", g->w() );
	p->set( "group_h", g->h() );
	int num = g->children();
	for ( int i = 0; i < num; i++ ) {
		Fl_Group* gg = dynamic_cast<Fl_Group*>(g->child(i));
		if ( gg ) {
			fltk_save_layout( new Fl_Preferences( *p, Fl_Preferences::Name( "group_%d", i ) ), gg );
		} else {
			p->set( Fl_Preferences::Name( "child_%d_x", i ), g->child(i)->x() );
			p->set( Fl_Preferences::Name( "child_%d_y", i ), g->child(i)->y() );
			p->set( Fl_Preferences::Name( "child_%d_w", i ), g->child(i)->w() );
			p->set( Fl_Preferences::Name( "child_%d_h", i ), g->child(i)->h() );
		}
	}
}

void fltk_load_layout( Fl_Preferences* p, Fl_Group* g) {
	int x, y, w, h;
	p->get( "group_x", x, g->x() );
	p->get( "group_y", y, g->y() );
	p->get( "group_w", w, g->w() );
	p->get( "group_h", h, g->h() );
	g->resize( x, y, w, h );
	int num = g->children();
	for ( int i = 0; i < num; i++ ) {
		Fl_Group* gg = dynamic_cast<Fl_Group*>(g->child(i));
		if ( gg ) {
			fltk_load_layout( new Fl_Preferences( *p, Fl_Preferences::Name( "group_%d", i ) ), gg );
		} else {
			p->get( Fl_Preferences::Name( "child_%d_x", i ), x, g->child(i)->x() );
			p->get( Fl_Preferences::Name( "child_%d_y", i ), y, g->child(i)->y() );
			p->get( Fl_Preferences::Name( "child_%d_w", i ), w, g->child(i)->w() );
			p->get( Fl_Preferences::Name( "child_%d_h", i ), h, g->child(i)->h() );
			g->child(i)->resize( x, y, w, h );
		}
	}
}
