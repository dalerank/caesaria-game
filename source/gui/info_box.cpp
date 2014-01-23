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

#include <cstdio>

#include "info_box.hpp"

#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "core/gettext.hpp"
#include "gfx/decorator.hpp"
#include "objects/metadata.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "texturedbutton.hpp"
#include "gui/label.hpp"
#include "city/city.hpp"
#include "objects/market.hpp"
#include "core/stringhelper.hpp"
#include "good/goodhelper.hpp"
#include "objects/farm.hpp"
#include "objects/entertainment.hpp"
#include "objects/house.hpp"
#include "objects/religion.hpp"
#include "game/divinity.hpp"
#include "objects/warehouse.hpp"
#include "gfx/engine.hpp"
#include "gui/special_orders_window.hpp"
#include "good/goodstore.hpp"
#include "environment.hpp"
#include "groupbox.hpp"
#include "walker/walker.hpp"
#include "objects/watersupply.hpp"
#include "objects/senate.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "events/event.hpp"
#include "game/settings.hpp"
#include "objects/well.hpp"
#include "image.hpp"
#include "core/foreach.hpp"
#include "dictionary.hpp"
#include "gameautopause.hpp"

using namespace constants;

namespace gui
{

class InfoBoxSimple::Impl
{
public:
  Label* lbBackground;
  Label* lbBlackFrame;
  Label* lbTitle;
  Label* lbText;
  PushButton* btnExit;
  PushButton* btnHelp;
  bool isAutoPosition;
  GameAutoPause autopause;


  Impl() : lbBackground(0), lbBlackFrame(0), lbTitle(0),
    lbText(0), btnExit(0), btnHelp(0), isAutoPosition(false)
  {

  }
};

InfoBoxSimple::InfoBoxSimple( Widget* parent, const Rect& rect, const Rect& blackArea, int id )
: Widget( parent, id, rect ), _d( new Impl )
{
  _d->autopause.activate();

  // create the title
  Widget::setupUI( GameSettings::rcpath( "/gui/infobox.gui" ) );

  _d->lbTitle = findChild<Label*>( "lbTitle", true );
  _d->btnExit = findChild<TexturedButton*>( "btnExit", true );
  _d->btnHelp = findChild<TexturedButton*>( "btnHelp", true );
  _d->lbBackground = findChild<Label*>( "lbBackground", true );
  _d->lbBlackFrame = findChild<Label*>( "lbBlackFrame", true );
  _d->lbText = findChild<Label*>( "lbText", true );

  if( _d->btnExit ) { _d->btnExit->setPosition( Point( getWidth() - 39, getHeight() - 39 ) ); }
  if( _d->btnHelp ) { _d->btnHelp->setPosition( Point( 14, getHeight() - 39 ) ); }

  CONNECT( _d->btnExit, onClicked(), this, InfoBoxSimple::deleteLater );
  CONNECT( _d->btnHelp, onClicked(), this, InfoBoxSimple::showDescription );

  // black box
  Point lastPos( getWidth() - 32, getHeight() - 48 );
  if( _d->lbBlackFrame )
  {
    _d->lbBlackFrame->setVisible( blackArea.getSize().getArea() > 0 );
    _d->lbBlackFrame->setGeometry( blackArea );
    lastPos.setY( _d->lbBlackFrame->getTop() - 10 );
  }

  if( _d->lbText && blackArea.getWidth() == 0 )
  {
    Rect r = _d->lbText->getRelativeRect();
    r.LowerRightCorner = _d->btnExit->getRightupCorner();
    _d->lbText->setGeometry( r );
  }

  _afterCreate();
}

void InfoBoxSimple::setText( const std::string& text )
{
  if( _d->lbText ) { _d->lbText->setText( text ); }
}

InfoBoxSimple::~InfoBoxSimple()
{
}

void InfoBoxSimple::draw( GfxEngine& engine )
{
  Widget::draw( engine );
}

bool InfoBoxSimple::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return getParent()->getAbsoluteRect().isPointInside( point );
}

bool InfoBoxSimple::onEvent( const NEvent& event)
{
  switch( event.EventType )
  {
  case sEventMouse:
    if( event.mouse.type == mouseRbtnRelease )
    {
      deleteLater();
      return true;
    }
    else if( event.mouse.type == mouseLbtnRelease )
    {
      return true;
    }
  break;

  default:
  break;
  }

  return Widget::onEvent( event );
}

void InfoBoxSimple::setTitle( const std::string& title )
{
  if( _d->lbTitle ) { _d->lbTitle->setText( title ); }
}

bool InfoBoxSimple::isAutoPosition() const{  return _d->isAutoPosition;}
void InfoBoxSimple::setAutoPosition( bool value ){  _d->isAutoPosition = value;}

void InfoBoxSimple::setupUI(const VariantMap& ui)
{
  Widget::setupUI( ui );

  _d->isAutoPosition = ui.get( "autoPosition", true );
}

Label* InfoBoxSimple::_getTitle(){  return _d->lbTitle;}

Label*InfoBoxSimple::_getInfo(){ return _d->lbText; }
Label* InfoBoxSimple::_getBlackFrame(){  return _d->lbBlackFrame; }
PushButton*InfoBoxSimple::_getBtnExit() { return _d->btnExit; }

void InfoBoxSimple::_updateWorkersLabel(const Point &pos, int picId, int need, int have )
{
  _d->lbBlackFrame->setVisible( need > 0 );
  if( !_d->lbBlackFrame || 0 == need)
    return;

  // number of workers
  std::string text = StringHelper::format( 0xff, "%d %s (%d %s)",
                                           have, _("##employers##"),
                                           need, _("##requierd##") );
  _d->lbBlackFrame->setIcon( Picture::load( ResourceGroup::panelBackground, picId ), pos );
  _d->lbBlackFrame->setText( text );
  _d->lbBlackFrame->setTextOffset( Point( pos.x() + 30, 0 ) );
}

InfoBoxWorkingBuilding::InfoBoxWorkingBuilding( Widget* parent, WorkingBuildingPtr building)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  _working = building;

  std::string title = MetaDataHolder::getPrettyName( _working->getType() );
  setTitle( _(title) );

  _updateWorkersLabel( Point( 32, 150 ), 542, _working->getMaxWorkers(), _working->getWorkersCount() );

  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                           (int)_working->getState( Construction::damage ),
                                           (int)_working->getState( Construction::fire ));

  new Label( this, Rect( 50, getHeight() - 30, getWidth() - 50, getHeight() - 10 ), text );
  new Label( this, Rect( 16, 50, getWidth() - 16, 130 ), "", false, Label::bgNone, lbHelpId );
}

void InfoBoxWorkingBuilding::setText(const std::string& text)
{  
  if( Widget* lb = findChild( lbHelpId ) )
  {
    StringArray messages;
    messages.push_back( text );

    if( _working->getMaxWorkers() > 0 )
    {
      std::string type = MetaDataHolder::getTypename( _working->getType() );
      const char* stateName[] = { "nowork", "poor", "half", "good", "awesome" };
      int workPercent = _working->getWorkersCount() * 4 / _working->getMaxWorkers();

      messages.push_back( StringHelper::format( 0xff, "##%s_%s##", type.c_str(), stateName[ workPercent == 0 ? 0 : (workPercent + 1) ]));
    }

    lb->setText( messages.at( rand() % messages.size() ) );
  }
}

void InfoBoxWorkingBuilding::showDescription()
{
  DictionaryWindow::show( getEnvironment()->getRootWidget(), _working->getType() );
}

InfoBoxTemple::InfoBoxTemple( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 56, 510 - 16, 56 + 62) )
{
  TemplePtr temple = ptr_cast<Temple>( tile.getOverlay() );
  RomeDivinityPtr divn = temple->getDivinity();

  bool bigTemple = temple->getSize().getWidth() > 2;
  std::string desc = _(divn->getShortDescription());
  std::string text = StringHelper::format( 0xff, "##%s_%s_temple##",
                                                 bigTemple ? "big" : "small",
                                                 divn->getDebugName().c_str() );
  setTitle( _(text) + desc );

  _updateWorkersLabel( Point( 32, 56 + 12), 542, temple->getMaxWorkers(), temple->getWorkersCount() );

  new Image( this, Point( 192, 140 ), temple->getDivinity()->getPicture() );
}

InfoBoxTemple::~InfoBoxTemple()
{
}

InfoBoxBuilding::InfoBoxBuilding( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 450, 220 ), Rect( 16, 60, 450 - 16, 60 + 50) )
{
  BuildingPtr building = ptr_cast<Building>( tile.getOverlay() );
  setTitle( MetaDataHolder::getPrettyName( building->getType() ) );
}

InfoBoxLand::InfoBoxLand( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 60, 510 - 16, 60 + 180) )
{ 
  Label* lbText = new Label( this, Rect( 38, 239, 470, 338 ), "", true, Label::bgNone, lbTextId );
  lbText->setFont( Font::create( FONT_2 ) );
  lbText->setWordwrap( true );

  if( tile.getFlag( Tile::tlTree ) )
  {
    setTitle( _("##trees_and_forest_caption##") );
    lbText->setText( _("##trees_and_forest_text##"));
  } 
  else if( tile.getFlag( Tile::tlWater ) )
  {
    setTitle( _("##water_caption##") );
    lbText->setText( _("##water_text##"));
  }
  else if( tile.getFlag( Tile::tlRock ) )
  {
    setTitle( _("##rock_caption##") );
    lbText->setText( _("##rock_text##"));
  }
  else if( tile.getFlag( Tile::tlRoad ) )
  {
    if( tile.getOverlay()->getType() == construction::plaza )
    {
      setTitle( _("##plaza_caption##") );
      lbText->setText( _("##plaza_text##"));
    }
    else 
    {
      setTitle( _("##road_caption##") );
      lbText->setText( _("##road_text##"));
    }
  }
  else if( tile.getFlag( Tile::tlMeadow ) )
  {
    setTitle( _("##meadow_caption##") );
    lbText->setText( _("##meadow_text##"));
  }
  else 
  {
    setTitle( _("##clear_land_caption##") );
    lbText->setText( _("##clear_land_text##"));
  }
  
  //int index = (size - tile.getJ() - 1 + border_size) * 162 + tile.getI() + border_size;

  std::string text = StringHelper::format( 0xff, "Tile at: (%d,%d) ID:%04X",
                                           tile.getI(), tile.getJ(),  
                                          ((short int) tile.getOriginalImgId() ) );
  
  lbText->setText( text );
}

void InfoBoxLand::setText( const std::string& text )
{
  if( Widget* lb = findChild( lbTextId ) )
    lb->setText( text );
}

InfoBoxFreeHouse::InfoBoxFreeHouse( Widget* parent, const Tile& tile )
    : InfoBoxLand( parent, tile )
{
    setTitle( _("##freehouse_caption##") );

    ConstructionPtr cnst = ptr_cast<Construction>( tile.getOverlay() );
    if( cnst.isValid() && cnst->getAccessRoads().size() == 0 )
    {
      setText( _("##freehouse_text_noroad##") );
    }
    else
    {
      setText( _("##freehouse_text##") );
    }
}   

InfoBoxColosseum::InfoBoxColosseum(Widget *parent, const Tile &tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 470, 300), Rect( 16, 145, 470 - 16, 145 + 100 ) )
{
  CollosseumPtr colloseum = ptr_cast<Collosseum>(tile.getOverlay());
  setTitle( MetaDataHolder::getPrettyName( building::colloseum ) );

  _updateWorkersLabel( Point( 40, 150), 542, colloseum->getMaxWorkers(), colloseum->getWorkersCount() );
  
  if( colloseum->isNeedGladiators() )
  {
    new Label( this, Rect( 35, 190, getWidth() - 35, 190 + 20 ), _("##colloseum_haveno_gladiatorpit##") );
  }
  else
  {
    std::string text = StringHelper::format( 0xff, "Animal contest runs for another %d days", 0 );
    new Label( this, Rect( 35, 190, getWidth() - 35, 190 + 20 ), text );

    text = StringHelper::format( 0xff, "Gladiator bouts runs for another %d days", 0 );
    new Label( this, Rect( 35, 210, getWidth() - 35, 210 + 20 ), text );
  }
}

InfoBoxColosseum::~InfoBoxColosseum()
{

}

InfoBoxText::InfoBoxText(Widget* parent, const std::string& title, const std::string& message)
  : InfoBoxSimple( parent, Rect( 0, 0, 480, 320 ), Rect( 18, 40, 480 - 18, 320 - 50 ) )
{
  setTitle( title );
  _d->isAutoPosition = false;

  setPosition( Point( parent->getWidth() - getWidth(), parent->getHeight() - getHeight() ) / 2 );

  _d->lbText->setGeometry( Rect( 25, 45, getWidth() - 25, getHeight() - 55 ) );
  _d->lbText->setWordwrap( true );
  _d->lbText->setText( message );
}

InfoBoxText::~InfoBoxText()
{

}

}//end namespace gui
