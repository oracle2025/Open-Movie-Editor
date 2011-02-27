#ifndef _FREI0R_WIDGET_H_
#define _FREI0R_WIDGET_H_

#include "IEffectWidget.H"

namespace nle
{

class Frei0rEffect;
class Frei0rWidget;

typedef struct _widget_callback_info {
	Frei0rWidget* widget;
	int number;
	double x, y;
} widget_callback_info;

class Frei0rWidget : public IEffectWidget
{
	public:
		Frei0rWidget( Frei0rEffect* effect );
		~Frei0rWidget();
		void setDouble( int num, double val );
		void setBool( int num, bool val );
		void setPositionX( int num, double val );
		void setPositionY( int num, double val );
		void setString( int num, const char* val );
	private:
		Frei0rEffect* m_effect;
		widget_callback_info* m_infostack;
};

} /* namespace nle */

#endif /* _FREI0R_WIDGET_H_ */
