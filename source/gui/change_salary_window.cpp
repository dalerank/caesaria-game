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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "change_salary_window.hpp"
#include "pushbutton.hpp"
#include "listbox.hpp"
#include "core/logger.hpp"
#include "world/empire.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/utils.hpp"

namespace gui
{

namespace dialog
{

class ChangeSalary::Impl
{
public:
  int newSalary;
  int currentSalary;

public slots:
  void resolveSalaryChange(const ListBoxItem&);
  void setNewSalary();

public signals:
  Signal1<int> onChangeSalarySignal;
};

ChangeSalary::ChangeSalary(Widget* p, unsigned int salary)
  : Window( p, Rect(), "" ), __INIT_IMPL(ChangeSalary)
{
  __D_IMPL(d,ChangeSalary)
  setupUI( ":/gui/changesalary.gui");
  setCenter( parent()->center() );

  d->newSalary = salary;
  d->currentSalary = salary;

  LINK_WIDGET_ACTION( PushButton*, btnCancel, onClicked(), this, ChangeSalary::deleteLater );
  LINK_WIDGET_ACTION( PushButton*, btnOk, onClicked(), d.data(), Impl::setNewSalary );
  LINK_WIDGET_ACTION( PushButton*, btnOk, onClicked(), this, ChangeSalary::deleteLater  );
  LINK_WIDGET_ACTION( ListBox*, lbxTitles, onItemSelected(), d.data(), Impl::resolveSalaryChange );
}

void ChangeSalary::setRanks(world::GovernorRanks ranks)
{
  INIT_WIDGET_FROM_UI( ListBox*, lbxTitles )
  if( lbxTitles )
  {
    for( auto& rank : ranks )
    {
      std::string salaryStr = _( "##" + rank.rankName + "_salary##" );
      ListBoxItem& item = lbxTitles->addItem( salaryStr + "   " + utils::i2str( rank.salary ) );
      item.setTag( rank.salary );
    }

    lbxTitles->setSelectedTag( _dfunc()->currentSalary );
  }
}

ChangeSalary::~ChangeSalary(){}
Signal1<int>& ChangeSalary::onChangeSalary(){ return _dfunc()->onChangeSalarySignal; }
void ChangeSalary::Impl::resolveSalaryChange( const ListBoxItem& item ) { newSalary = item.tag(); }
void ChangeSalary::Impl::setNewSalary(){ emit onChangeSalarySignal( newSalary ); }

}//end namespace dialog

}//end namespace gui
