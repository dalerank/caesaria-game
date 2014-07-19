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
  CityChartLegend( Widget* parent, const Rect& rectangle, bool horizontal, int stepCount );

public oc3_slots:
  void setMaxValue( int value );

protected:
  void _updateTexture( gfx::Engine& painter );

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
    _isSmall = false;
  }

  DrawMode fit( DrawMode mode );

  void update( PlayerCityPtr city, DrawMode mode );

  void _updateTexture( gfx::Engine& painter );

  DrawMode mode() const { return _mode; }
  void setIsSmall( bool value ) { _isSmall = value; }

public oc3_signals:
  Signal1<int> onMaxYChange;
  Signal1<int> onMaxXChange;

private:
  std::vector<int> _values;
  DrawMode _mode;
  int _picIndex;
  unsigned int _maxValue;
  int _minXValue, _maxXValue;
  bool _isSmall;
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
  CityChart* chartCurrent;
  CityChart* chartNext;
  CityChart* chartPrev;
  Label* lbNextChart;
  Label* lbPrevChart;
  Label* lbTitle;
  Label* lbMigrationValue;

public oc3_slots:
  void showNextChart();
  void showPrevChart();

public:
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

  _d->chartCurrent = 0;
  CityChartLegend* legendY = new CityChartLegend( this, Rect( 8, 60, 56, 280 ), false, 2 );
  CityChartLegend* legendX = new CityChartLegend( this, Rect( 54, 270, 480, 290 ), true, 10 );

  _d->lbNextChart = findChildA<Label*>( "lbNextChart", true, this );
  _d->lbPrevChart = findChildA<Label*>( "lbPrevChart", true, this );
  _d->lbTitle = findChildA<Label*>( "lbTitle", true, this );
  _d->lbMigrationValue =  findChildA<Label*>( "lbMigrationValue", true, this );

  Label* lbNextChartArea = findChildA<Label*>( "lbNextChartArea", true, this );
  if( lbNextChartArea )
  {
    _d->chartNext = new CityChart( lbNextChartArea, Rect( 0, 0, 100, 50 ) );
    _d->chartNext->setIsSmall( true );
    CONNECT( lbNextChartArea, onClicked(), _d.data(), Impl::showNextChart );
  }

  Label* lbPrevChartArea = findChildA<Label*>( "lbPrevChartArea", true, this );
  if( lbPrevChartArea )
  {
    _d->chartPrev = new CityChart( lbPrevChartArea, Rect( 0, 0, 100, 50 ) );
    _d->chartPrev->setIsSmall( true );
    CONNECT( lbPrevChartArea, onClicked(), _d.data(), Impl::showPrevChart );
  }

  Label* lbChart = findChildA<Label*>( "lbChart", true, this );
  if( lbChart )
  {
    _d->chartCurrent = new CityChart( lbChart, Rect( 10, 7, 405, 202 ) );

    CONNECT( _d->chartCurrent, onMaxYChange, legendY, CityChartLegend::setMaxValue );
    CONNECT( _d->chartCurrent, onMaxXChange, legendX, CityChartLegend::setMaxValue );

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


void Population::Impl::switch2nextChart( int change )
{
  if( chartCurrent )
  {
    ChartModeHelper cmHelper;

    chartCurrent->update( city, (CityChart::DrawMode)(chartCurrent->mode()+change) );
    int mode = chartCurrent->mode();
    std::string modeName = cmHelper.findName( (CityChart::DrawMode)mode );
    std::string text = StringHelper::format( 0xff, "##citychart_%s##", modeName.c_str() );
    lbTitle->setText( _( text ) );

    mode = chartCurrent->fit( (CityChart::DrawMode)(chartCurrent->mode() + 1) );
    modeName = cmHelper.findName( (CityChart::DrawMode)mode );
    text = StringHelper::format( 0xff, "##citychart_%s##", modeName.c_str() );
    lbNextChart->setText(  _( text ) );
    chartNext->update( city, (CityChart::DrawMode)mode );

    mode = chartCurrent->fit( (CityChart::DrawMode)(chartCurrent->mode() - 1) );
    modeName = cmHelper.findName( (CityChart::DrawMode)mode );
    text = StringHelper::format( 0xff, "##citychart_%s##", modeName.c_str() );
    lbPrevChart->setText( _( text ) );
    chartPrev->update( city, (CityChart::DrawMode)mode );
  }
}

void Population::Impl::showPrevChart() { switch2nextChart( -1 );}
void Population::Impl::showNextChart() { switch2nextChart( 1 ); }

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

CityChartLegend::CityChartLegend(Widget *parent, const Rect &rectangle, bool horizontal, int stepCount)
  : Label( parent, rectangle )
{
  setFont( Font::create( FONT_1 ) );
  _stepCount = stepCount;
  _horizontal = horizontal;
}

void CityChartLegend::setMaxValue(int value)
{
  _maxValue = value;
  _resizeEvent();
}

void CityChartLegend::_updateTexture(Engine &painter)
{
  Label::_updateTexture( painter );

  if( !_textPictureRef() )
    return;

  Picture pic = *_textPictureRef();

  pic.fill( 0, Rect() );
  for( int k=0; k < _stepCount+1; k++ )
  {
    std::string text = StringHelper::i2str( k * _maxValue / _stepCount );
    Point offset  = _horizontal
        ? Point( k * width() / _stepCount - (k == 0 ? 0 : 20), 3 )
        : Point( 8, height() - k * height() / _stepCount - (k == _stepCount ? 0 : 23) );

    font().draw( pic, text, offset );
  }
}

CityChart::DrawMode CityChart::fit(CityChart::DrawMode mode)
{
  if( mode < 0 ) { mode = (DrawMode)(CityChart::dm_count-1); }
  else if( mode >= CityChart::dm_count ) { mode = CityChart::dm_census; }

  return (DrawMode)mode;
}

void CityChart::update(PlayerCityPtr city, CityChart::DrawMode mode)
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
        _maxValue = std::max<unsigned int>( _maxValue, (*it).population );
      }

      _maxValue = ( _maxValue * 1.5 / 100 ) * 100;
      _maxXValue = _values.size();
      oc3_emit onMaxYChange( _maxValue );
      oc3_emit onMaxXChange( _maxXValue );
    }
  break;

  case dm_society:
    {
      city::Helper helper( city );
      HouseList houses = helper.find<House>( building::house );

      _values.clear();
      _maxValue = 5;
      std::map< unsigned int, unsigned int> levelPopulations;
      for( int k=0; k < HouseLevel::count; k++ )
      {
        levelPopulations[ k ] = 0;
      }

      foreach( it, houses )
      {
        const HouseSpecification& spec = (*it)->spec();
        levelPopulations[ spec.level() ] += (*it)->habitants().count();
        _maxValue = std::max( levelPopulations[ spec.level() ], _maxValue );
      }

      if( !levelPopulations.empty() )
      {
        _minXValue = levelPopulations.begin()->second;
        _maxXValue = levelPopulations.rbegin()->second;
        foreach( it, levelPopulations )
        {
          _values.push_back( it->second );
        }
      }

      _maxValue = ( _maxValue * 1.5 / 100 ) * 100;
      _maxXValue = HouseLevel::count;
      oc3_emit onMaxYChange( _maxValue );
      oc3_emit onMaxXChange( _maxXValue );
    }
  break;

  default: break;
  }

  if( _isSmall )
  {
    _picIndex = 78;
  }
  else
  {
    if( _values.size() <= 20 ) { _picIndex = 75; }
    else if( _values.size() <= 40 ) { _picIndex = 76; }
    else if( _values.size() <= 100 ) { _picIndex = 77; }
    else { _picIndex = 78; }
  }

  _resizeEvent();
}

void CityChart::_updateTexture(Engine &painter)
{
  Label::_updateTexture( painter );

  if( !_textPictureRef() || _maxValue == 0 )
    return;

  Picture& pic = *_textPictureRef();
  Picture& rpic = Picture::load( ResourceGroup::panelBackground, _picIndex );

  pic.fill( 0, Rect() );
  int index=0;
  unsigned int maxHeight = std::min( rpic.height(), pic.height() );
  foreach( it, _values )
  {
    int y = maxHeight - (*it) * maxHeight / _maxValue;
    pic.draw( rpic, Rect( 0, y, rpic.width(), maxHeight), Rect( rpic.width() * index, y, rpic.width() * (index+1), maxHeight), false );
    index++;
  }
}

}//end namespace advisorwnd

}//end namespace gui
