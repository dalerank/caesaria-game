// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "change_salary_window.hpp"
#include "pushbutton.hpp"
#include "listbox.hpp"
#include "game/settings.hpp"
#include "core/logger.hpp"

namespace gui
{

class ChangeSalaryWindow::Impl
{
public:
  int newSalary;

public oc3_slots:
  void resolveSalaryChange(const ListBoxItem&);
  void setNewSalary();

public oc3_signals:
  Signal1<int> onChangeSalarySignal;
};

ChangeSalaryWindow::ChangeSalaryWindow(Widget* p, int salary)
  : Widget( p, -1, Rect() ), __INIT_IMPL(ChangeSalaryWindow)
{
  setupUI( GameSettings::rcpath( "/gui/changesalary.gui") );
  setCenter( parent()->center() );

  _dfunc()->newSalary = salary;

  ListBox* lbx = findChildA<ListBox*>( "lbxTitles", true, this );
  if( lbx )
  {
    for( int i=0; i < lbx->itemCount(); i++ )
    {
      ListBoxItem& item = lbx->item( i );
      if( item.tag() > salary )
      {
        lbx->setSelected( i - 1 );
        break;
      }
    }
  }

  PushButton* btnCancel = findChildA<PushButton*>( "btnCancel", true, this );
  PushButton* btnOk = findChildA<PushButton*>( "btnOk", true, this );
  CONNECT( btnCancel, onClicked(), this, ChangeSalaryWindow::deleteLater );
  CONNECT( btnOk, onClicked(), _dfunc().data(), Impl::setNewSalary );
  CONNECT( btnOk, onClicked(), this, ChangeSalaryWindow::deleteLater  );
  CONNECT( lbx, onItemSelected(), _dfunc().data(), Impl::resolveSalaryChange );
}

ChangeSalaryWindow::~ChangeSalaryWindow(){  }
Signal1<int>& ChangeSalaryWindow::onChangeSalary(){  return _dfunc()->onChangeSalarySignal; }
void ChangeSalaryWindow::Impl::resolveSalaryChange(const ListBoxItem& item ) { newSalary = item.tag(); }
void ChangeSalaryWindow::Impl::setNewSalary(){  onChangeSalarySignal.emit( newSalary ); }


}//end namespace gui
