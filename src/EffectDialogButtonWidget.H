#ifndef _EFFECT_DIALOG_BUTTON_WIDGET_H_
#define _EFFECT_DIALOG_BUTTON_WIDGET_H_

#include "IEffectWidget.H"

class Fl_Button;

namespace nle
{

class FilterBase;

class EffectDialogButtonWidget : public IEffectWidget
{
	public:
		EffectDialogButtonWidget( FilterBase* filter );
		~EffectDialogButtonWidget();
		void cb_Edit_i( Fl_Button* o );
		static void cb_Edit( Fl_Button* o, void* v );
	private:
		FilterBase* m_filter;
};

} /* namespace nle */

#endif /* _EFFECT_DIALOG_BUTTON_WIDGET_H_ */
