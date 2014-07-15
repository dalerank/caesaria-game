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
#include "objects/house.hpp"
#include "city/helper.hpp"
#include "core/logger.hpp"
#include "game/settings.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "city/cityservice_info.hpp"
#include "objects/house_level.hpp"
#include "city/migration.hpp"
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
  typedef enum { dm_census, dm_population, dm_society, dm_count } DrawMode;
  CityChart( Widget* parent, const Rect& rectangle ) : Label( parent, rectangle )
  {

  }

  DrawMode fit( DrawMode mode )
  {
    if( mode < 0 ) { mode = (DrawMode)(CityChart::dm_count-1); }
    else if( mode >= CityChart::dm_count ) { mode = CityChart::dm_census; }

    return (DrawMode)mode;
  }

  void update( PlayerCityPtr city, DrawMode mode )
  {
    _values.clear();
    _mode = fit( mode );
    switch( _mode )
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

      oc3_emit onMaxYChange( _maxValue );
      oc3_emit onMaxXChange( _maxXValue );
    }
    break;

    case dm_population:
    {
      city::InfoPtr info = ptr_cast<city::Info>( city->findService( city::Info::defaultName() ) );
      city::Info::History history = info->history();
      history.push_back( info->lastParams() );

      _values.clear();
      foreach( it, history )
      {
        _values.push_back( (*it).population );
      }
    }
    break;

    case dm_society:
    {
      city::Helper helper( city );
      HouseList houses = helper.find<House>( building::house );

      _values.clear();
      _maxValue = 5;
      std::map< int, int > taxMap;

      foreach( it, houses )
      {
        const HouseSpecification& spec = (*it)->spec();
        taxMap[ spec.taxRate() ]++;
        _maxValue = std::max( spec.taxRate(), _maxValue );
      }

      if( !taxMap.empty() )
      {
        _minXValue = taxMap.begin()->second;
        _maxXValue = taxMap.rbegin()->second;
        foreach( it, taxMap )
        {
          _values.push_back( it->second );
        }
      }
    }

    default: break;
    }

    if( _values.size() <= 20 ) { _picIndex = 75; }
    else if( _values.size() <= 40 ) { _picIndex = 76; }
    else if( _values.size() <= 100 ) { _picIndex = 77; }
    else { _picIndex = 78; }

    _resizeEvent();
  }

  void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    if( !getTextPicture() )
      return;

    Picture pic = *getTextPicture();
    Picture rpic = Picture::load( ResourceGroup::panelBackground, _picIndex );

    pic.fill( 0, Rect() );
    int index=0;
    foreach( it, _values )
    {
      int y = rpic.height() - (*it) * rpic.height() / _maxValue;
      pic.draw( rpic, Rect( 0, y, rpic.width(), rpic.height()), Rect( rpic.width() * index, y, rpic.width() * (index+1), rpic.height()), false );
      index++;
    }
  }

  DrawMode mode() const { return _mode; }

public oc3_signals:
  Signal1<int> onMaxYChange;
  Signal1<int> onMaxXChange;

private:
  std::vector<int> _values;
  DrawMode _mode;
  int _picIndex;
  int _maxValue;
  int _minXValue, _maxXValue;
};

class ChartModeHelper : public EnumsHelper<CityChart::DrawMode>
{
public:
  ChartModeHelper() : EnumsHelper<CityChart::DrawMode>( CityChart::dm_count )
  {
    append( CityChart::dm_census, "census" );
    append( CityChart::dm_population, "population" );
    append( CityChart::dm_society, "society" );
  }
};

class Population::Impl
{
public:
  PlayerCityPtr city;
  CityChart* lbCityChart;
  Label* lbNextChart;
  Label* lbPrevChart;
  Label* lbTitle;
  Label* lbMigrationValue;

public:
  void showNextChart();
  void showPrevChart();
  void updateStates();
  void switch2nextChart( int change );
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

  _d->lbNextChart = findChildA<Label*>( "lbNextChart", true, this );
  _d->lbPrevChart = findChildA<Label*>( "lbPrevChart", true, this );
  _d->lbTitle = findChildA<Label*>( "lbTitle", true, this );
  _d->lbMigrationValue =  findChildA<Label*>( "lbMigrationValue", true, this );

  Label* lbChart = findChildA<Label*>( "lbChart", true, this );
  if( lbChart )
  {
    _d->lbCityChart = new CityChart( lbChart, Rect( 10, 7, 405, 202 ) );

    CONNECT( _d->lbCityChart, onMaxYChange, legendY, CityChartLegend::setMaxValue );
    CONNECT( _d->lbCityChart, onMaxXChange, legendX, CityChartLegend::setMaxValue );

    _d->switch2nextChart( 0 );
  }

  _d->updateStates();

  CONNECT( _d->lbNextChart, onClicked(), _d.data(), Impl::showNextChart );
  CONNECT( _d->lbPrevChart, onClicked(), _d.data(), Impl::showPrevChart );
}

void Population::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Widget::draw( painter );
}

void Population::Impl::showNextChart()
{
  switch2nextChart( 1 );
}

void Population::Impl::switch2nextChart( int change )
{
  if( lbCityChart )
  {
    int mode = lbCityChart->mode();
    lbCityChart->update( city, (CityChart::DrawMode)(mode+change) );

    mode = lbCityChart->mode();
    ChartModeHelper cmHelper;
    std::string modeName = cmHelper.findName( lbCityChart->fit( (CityChart::DrawMode)(mode + 1) ) );
    std::string text = StringHelper::format( 0xff, "##citychart_%s##", modeName.c_str() );
    lbNextChart->setText(  _( text ) );

    modeName = cmHelper.findName( lbCityChart->fit( (CityChart::DrawMode)(mode - 1) ) );
    text = StringHelper::format( 0xff, "##citychart_%s##", modeName.c_str() );
    lbPrevChart->setText( _( text ) );

    modeName = cmHelper.findName( lbCityChart->fit( (CityChart::DrawMode)mode ) );
    text = StringHelper::format( 0xff, "##citychart_%s##", modeName.c_str() );
    //std::string tooltipText =
    lbTitle->setText( _( text ) );
  }
}

void Population::Impl::showPrevChart() {  switch2nextChart( -1 );}

void Population::Impl::updateStates()
{
  if( lbMigrationValue )
  {
    int currentPop = city->population();

    city::InfoPtr info = ptr_cast<city::Info>( city->findService( city::Info::defaultName() ) );
    city::Info::Parameters lastMonth = info->lastParams();

    int migrationValue = currentPop - lastMonth.population;

    if( migrationValue >= 0 )
    {
      std::string suffix = ( migrationValue % 10 == 1 )
                             ? "##newcomer_this_month##"
                             : "##newcomers_this_month##";

      lbMigrationValue->setText( StringHelper::i2str( migrationValue ) + " " + _( suffix ) );
    }
    else
    {
      city::MigrationPtr migration = ptr_cast<city::Migration>( city->findService( city::Migration::defaultName() ) );
      if( migration.isValid() )
      {
        lbMigrationValue->setText( migration->leaveCityReason() );
      }
    }
  }
}

}//end namespace advisorwnd

}//end namespace gui
