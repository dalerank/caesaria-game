#ifndef __CAESARIA_EXTENTED_DATE_INFO_H_INCLUDED__
#define __CAESARIA_EXTENTED_DATE_INFO_H_INCLUDED__

#include "label.hpp"

namespace gui
{

class ExtentedDateInfo : public Label
{
public:
  //
  ExtentedDateInfo( Widget *parent, const Rect& rect=Rect(), int id=-1);

  virtual ~ExtentedDateInfo();

  virtual bool onEvent(const NEvent& e);
};

}//end namespace gui

#endif //__CAESARIA_EXTENTED_DATE_INFO_H_INCLUDED__
