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

#include "advisor_population_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "city/helper.hpp"
#include "core/logger.hpp"
#include "game/settings.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "label.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

class CityChartLegend : public Label
{
public:
  CityChartLegend( Widget* parent, const Rect& rectangle, bool horizontal, int stepCount )
    : Label( parent, rectangle )
  {
    setFont( Font::create( FONT_1 ) );
    _stepCount = stepCount;
    _horizontal = horizontal;
  }

public oc3_slots:
  void setMaxValue( int value )
  {
    _maxValue = value;
    _resizeEvent();
  }

  void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    if( !getTextPicture() )
      return;

    Picture pic = *getTextPicture();

    pic.fill( 0, Rect() );
    for( int k=0; k < _stepCount+1; k++ )
    {
      std::string text = StringHelper::i2str( k * _maxValue / _stepCount );
      Point offset  = _horizontal
                         ? Point( k * width() / _stepCount - (k == 0 ? 0 : 20), 3 )
                         : Point( 8, height() - k * height() / _stepCount - (k == _stepCount ? 0 : 23) );

      getFont().draw( pic, text, offset );
    }
  }

private:
  bool _horizontal;
  int _maxValue;
  int _stepCount;
};

class CityChart : public Label
{
public:
  typedef enum { dm_census } DrawMode;
  CityChart( Widget* parent, const Rect& rectangle ) : Label( parent, rectangle )
  {

  }

  void update( PlayerCityPtr city, DrawMode mode )
  {
    _values.clear();
    switch( mode )
    {
    case dm_census:
    {
      CitizenGroup population = city::Statistic::getPopulation( city );

      _maxValue = 100;
      for( int age=CitizenGroup::newborn; age <= CitizenGroup::longliver; age++ )
      {
        _values.push_back( population[ age ] );
        _maxValue = std::max<int>( _maxValue, population[ age ] );
      }

      _minXValue = 0;
      _maxXValue = _values.size();
      _maxValue = ( _maxValue * 1.5 / 100 ) * 100;

      onMaxYChange.emit( _maxValue );
      onMaxXChange.emit( _maxXValue );
    }
    break;
    }
  }

  void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    if( !getTextPicture() )
      return;

    Picture pic = *getTextPicture();
    Picture rpic = Picture::load( ResourceGroup::panelBackground, 77 );

    pic.fill( 0, Rect() );
    int index=0;
    foreach( it, _values )
    {
      int y = rpic.height() - (*it) * rpic.height() / _maxValue;
      pic.draw( rpic, Rect( 0, y, rpic.width(), rpic.height()), Rect( rpic.width() * index, y, rpic.width() * (index+1), rpic.height()), false );
      index++;
    }
  }

public oc3_signals:
  Signal1<int> onMaxYChange;
  Signal1<int> onMaxXChange;

private:
  std::vector<int> _values;
  int _maxValue;
  int _minXValue, _maxXValue;
};

class Population::Impl
{
public:
  PlayerCityPtr city;
  CityChart* lbCityChart;
};

Population::Population(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ),
  __INIT_IMPL(Population)
{
  setupUI( GameSettings::rcpath( "/gui/populationadv.gui" ) );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  __D_IMPL(_d,Population)
  _d->city = city;

  _d->lbCityChart = 0;
  CityChartLegend* legendY = new CityChartLegend( this, Rect( 8, 60, 56, 280 ), false, 2 );
  CityChartLegend* legendX = new CityChartLegend( this, Rect( 54, 270, 480, 290 ), true, 10 );

  Label* lbChart = findChildA<Label*>( "lbChart", true, this );
  if( lbChart )
  {
    _d->lbCityChart = new CityChart( lbChart, Rect( 10, 7, 405, 202 ) );

    CONNECT( _d->lbCityChart, onMaxYChange, legendY, CityChartLegend::setMaxValue );
    CONNECT( _d->lbCityChart, onMaxXChange, legendX, CityChartLegend::setMaxValue );

    _d->lbCityChart->update( city, CityChart::dm_census );
  }
}

void Population::draw( gfx::Engine& painter )
{
  if( !isVisible() )
    return;

  Widget::draw( painter );
}

}//end namespace advisorwnd

} //end namespace gui
