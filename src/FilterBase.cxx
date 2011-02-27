#include "FilterBase.H"
#include "EffectDialogButtonWidget.H"

namespace nle
{
IEffectWidget* FilterBase::widget()
{
	if ( hasDialog() ) {
		return new EffectDialogButtonWidget( this );
	}
	return 0;
}

} /* namespace nle */
