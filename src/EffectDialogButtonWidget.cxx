#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include "IEffectDialog.H"
#include "FilterBase.H"
#include "EffectDialogButtonWidget.H"


namespace nle
{
void EffectDialogButtonWidget::cb_Edit_i( Fl_Button* o )
{
	IEffectDialog* dialog = m_filter->dialog();
	if ( dialog ) {
		dialog->show();
	}
}
void EffectDialogButtonWidget::cb_Edit( Fl_Button* o, void* v )
{
	EffectDialogButtonWidget* edbw = (EffectDialogButtonWidget*)v;
	edbw->cb_Edit_i( o );
}
EffectDialogButtonWidget::EffectDialogButtonWidget( FilterBase* filter )
: IEffectWidget( 0, 0, 340, 20 ), m_filter( filter )
{
	 { Fl_Button* o = new Fl_Button(10, 0, 320, 20, "Edit");
	o->callback((Fl_Callback*)cb_Edit, this);
	o->labelsize( 12 );
  }
}
EffectDialogButtonWidget::~EffectDialogButtonWidget()
{
}

} /* namespace nle */
