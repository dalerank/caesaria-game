#include "extented_date_info.hpp"
#include "core/event.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(ExtentedDateInfo)

ExtentedDateInfo::ExtentedDateInfo(Widget *parent, const Rect& rect, int id) :
  Label( parent, rect, "", false, bgSimpleWhite, id )
{
  setGeometry( Rect( parent->width() - 50, 24, parent->width(), 100) );
}

ExtentedDateInfo::~ExtentedDateInfo() {}

bool ExtentedDateInfo::onEvent(const NEvent& e)
{
  if( e.EventType == sEventGui && e.gui.type == guiEditboxChanged )
  {

  }

  return Label::onEvent( e );
}

}//end namespace gui
