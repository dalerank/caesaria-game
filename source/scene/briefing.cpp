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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "briefing.hpp"
#include "gui/texturedbutton.hpp"
#include "gui/image.hpp"
#include "gui/label.hpp"
#include "core/saveadapter.hpp"
#include "gui/environment.hpp"
#include "game/game.hpp"
#include "core/event.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "core/gettext.hpp"
#include "core/variant_map.hpp"

using namespace gfx;

namespace gui
{

class MissionButton : public TexturedButton
{
public:
  MissionButton( Widget* parent, Point pos, std::string title, std::string mission )
   : TexturedButton( parent, pos, Size( 46 ), -1, "europe02", TexturedButton::States( 5, 6, 4, 4 ) )
  {
    setIsPushButton( true );
    _mission = mission;
    _title = title;
  }

  Signal2<std::string, std::string> onMissionSelect;

protected:
  virtual void _btnClicked()
  {
    auto buttons = parent()->children().select<MissionButton>();
    for( auto button : buttons)
    {
      if( button != this )
        button->setPressed( false );
    }

    emit onMissionSelect( _mission, _title );
  }

  std::string _mission,
              _title;
};

}//end namespace gui

namespace scene
{

class Briefing::Impl
{
public:
  static const int currentVesion=1;
  bool isStopped;
  gui::Label* cityCaption;
  gui::TexturedButton* btnContinue;
  Game* game;
  gfx::Engine* engine;
  std::string fileMap;
  std::string filename;
  Briefing::Result result;

  void resolvePlayMission()
  {
    result = Briefing::loadMission;
    isStopped = true;
  }

  void resolveSelecMission( std::string mission, std::string title )
  {
    btnContinue->setEnabled( true );
    cityCaption->setText( _(title) );
    fileMap = mission;
  }
};

Briefing::Briefing( Game& game, Engine& engine, std::string filename ) : _d( new Impl )
{
  _d->isStopped = false;
  _d->game = &game;
  _d->engine = &engine;
  _d->filename = filename;
}

Briefing::~Briefing() {}

void Briefing::draw()
{
  _d->game->gui()->beforeDraw();
  _d->game->gui()->draw();
}

void Briefing::handleEvent( NEvent& event )
{
  _d->game->gui()->handleEvent( event );
}

void Briefing::initialize()
{
  _d->game->gui()->clear();;

  Logger::warning( "Briefing: initialize start");

  VariantMap vm = config::load( _d->filename );

  if( Impl::currentVesion == vm[ "version" ].toInt() )
  {
    Picture mapBackImage( "mapback", 1 );
    std::string briefingCaption = vm[ "title" ].toString();
    if( briefingCaption.empty() )
      briefingCaption = "##briefing_select_next_mission##";
    gui::Image& mapback = _d->game->gui()->add<gui::Image>( Point(), mapBackImage );
    if( !mapBackImage.isValid() )
    {
      mapback.setGeometry( Rect( 0, 0, 640, 480 ) );
    }
    mapback.setCenter( _d->game->gui()->rootWidget()->center() );

    std::string mapToLoad = vm[ "image" ].toString();

    mapBackImage.load( mapToLoad )
                .withFallback( "europe01", 2 );

    Point startImgPos( 192, 144 );
    const unsigned int textYOffset = 400;
    mapback.add<gui::Image>( startImgPos, mapBackImage );

    VariantMap items = vm[ "items" ].toMap();
    for( const auto& it : items )
    {
      VariantMap miss_vm = it.second.toMap();
      std::string title = miss_vm.get( "title" ).toString();
      std::string mission = miss_vm.get( "mission" ).toString();
      Point location = miss_vm.get( "location" ).toPoint() + startImgPos;

      auto& missionButton = mapback.add<gui::MissionButton>( location, title, mission );
      CONNECT( &missionButton, onMissionSelect, _d.data(), Impl::resolveSelecMission );
    }

    std::string missionTt = vm.get( "title" ).toString();
    auto& missionTitle = mapback.add<gui::Label>( Rect( 200, 550, 200 + textYOffset, 600 ), missionTt );
    missionTitle.setFont( Font::create( FONT_5 ));
    _d->cityCaption = &mapback.add<gui::Label>( Rect( 200, 600, 200 + textYOffset, 630 ) );
    _d->cityCaption->setFont( Font::create( FONT_2 ) );
    _d->cityCaption->setText( _(briefingCaption) );

    _d->btnContinue = &mapback.add<gui::TexturedButton>( Point( 780, 560 ), Size( 27 ), -1, 179 );
    _d->btnContinue->setEnabled( false );
    CONNECT( _d->btnContinue, onClicked(), _d.data(), Impl::resolvePlayMission );
  }
  else
  {
    _d->isStopped = true;
    _d->result = Briefing::mainMenu;
  }
}

int Briefing::result() const{  return _d->result; }
bool Briefing::isStopped() const{  return _d->isStopped;}
std::string Briefing::getMapName() const{ return _d->fileMap; }

}//end namespace scene
