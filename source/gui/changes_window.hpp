#ifndef __CAESARIA_CHANGES_WINDOW_H_INCLUDED__
#define __CAESARIA_CHANGES_WINDOW_H_INCLUDED__

#include "window.hpp"

namespace gui
{

class ChangesWindow : public SimpleWindow
{
public:
  //
  ChangesWindow( Widget *parent, const Rect& rect, const std::string& title, int number);

  void update();

  void hideAlways();

  virtual ~ChangesWindow();

  virtual bool onEvent(const NEvent& e);
};

}//end namespace gui

#endif //__CAESARIA_CHANGES_WINDOW_H_INCLUDED__
