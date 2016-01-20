#include "changes_window.hpp"
#include "core/format.hpp"

namespace gui
{

ChangesWindow::ChangesWindow(Widget * parent, const Rect & rect, const std::string & title, int number)
  : SimpleWindow( parent, rect, title, ":/gui/changes.gui" )
{
  setupUI(fmt::format(":/changes/{}.changes", number));
}

ChangesWindow::~ChangesWindow()
{
}

bool ChangesWindow::onEvent(const NEvent & e)
{
  return false;
}

}//end namespace gui

