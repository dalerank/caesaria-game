#include "changes_window.hpp"
#include "core/format.hpp"
#include "game/settings.hpp"
#include "core/logger.hpp"

namespace gui
{

ChangesWindow::ChangesWindow(Widget * parent, const Rect & rect, int number)
  : SimpleWindow( parent, rect, "", ":/gui/changes.gui" )
{
  setupUI(fmt::format(":/changes/{}.changes", number));

  auto& btn = add<PushButton>(Rect(13, width() - 36, 13 + 300, width() - 12), "", 0xff, false, PushButton::whiteBorderUp);
  CONNECT(&btn, onClicked(), this, ChangesWindow::hideAlways)
  update();
}

void ChangesWindow::update()
{
  auto* button = findChild<PushButton>(0xff);
  if (button)
  {
    bool showChanges = KILLSWITCH(showLastChanges);
    button->setText(showChanges ? "Don't show this window" : "Show this window on start");
  }
}

void ChangesWindow::hideAlways() 
{ 
  bool showChanges = KILLSWITCH(showLastChanges);
  SETTINGS_SET_VALUE(showLastChanges, !showChanges);
  update();
}

ChangesWindow::~ChangesWindow()
{
}

bool ChangesWindow::onEvent(const NEvent & e)
{
  return false;
}

}//end namespace gui

