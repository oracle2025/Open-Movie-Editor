#ifndef _I_EFFECT_WIDGET_H_
#define _I_EFFECT_WIDGET_H_

#include <FL/Fl.H>
#include <FL/Fl_Group.H>

namespace nle
{

class IEffectWidget : public Fl_Group {
	public:
		virtual ~IEffectWidget() {}
	protected:
		IEffectWidget(int X, int Y, int W, int H, const char *L = 0)
			: Fl_Group( X, Y, W, H, L )
		{}
};

} /* namespace nle */

#endif /* _I_EFFECT_WIDGET_H_ */
