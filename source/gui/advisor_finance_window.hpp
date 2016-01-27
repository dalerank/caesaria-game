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

#ifndef __CAESARIA_ADVISOR_FINANCE_WINDOW_H_INCLUDED__
#define __CAESARIA_ADVISOR_FINANCE_WINDOW_H_INCLUDED__

#include "advisor_base_window.hpp"

namespace gui
{

namespace advisorwnd
{

/**
 * @brief Finance advisor window class
 */
class Finance : public Base
{
public:
  Finance( PlayerCityPtr city, Widget* parent, int id );

  virtual void draw( gfx::Engine& painter );
  virtual bool onEvent(const NEvent &event);

private:
  void _updateTaxRateNowLabel();

  /**
   * @brief Decrease/Increase city tax rate
   */
  void _decreaseTax();
  void _increaseTax();

  void _initReportRows();
  void _initTaxManager();

  /**
   * @brief Update text for registered tax payers value
   */
  void _updateRegisteredPayers();
  void _addRow(const Rect& rect, int index, bool line = false );
  void _showIssueDetails( Widget* widget );

  /**
   * @brief Update text for city treasure value
   */
  void _updateCityTreasure();
  int  _calculateTaxValue();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end advisorwnd

}//end namespace gui
#endif //__CAESARIA_ADVISOR_FINANCE_WINDOW_H_INCLUDED__
