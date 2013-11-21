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
#include "building/metadata.hpp"
#include "game/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "texturedbutton.hpp"
#include "gui/label.hpp"
#include "game/city.hpp"
#include "building/market.hpp"
#include "building/granary.hpp"
#include "core/stringhelper.hpp"
#include "game/goodhelper.hpp"
#include "building/farm.hpp"
#include "building/entertainment.hpp"
#include "building/house.hpp"
#include "building/religion.hpp"
#include "game/divinity.hpp"
#include "building/warehouse.hpp"
#include "gfx/engine.hpp"
#include "gui/special_orders_window.hpp"
#include "game/goodstore.hpp"
#include "groupbox.hpp"
#include "walker/walker.hpp"
#include "building/watersupply.hpp"
#include "building/senate.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"
#include "events/event.hpp"
#include "game/settings.hpp"
#include "image.hpp"

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

  Impl() : lbBackground(0), lbBlackFrame(0), lbTitle(0),
    lbText(0), btnExit(0), btnHelp(0), isAutoPosition(false)
  {

  }
};

InfoBoxSimple::InfoBoxSimple( Widget* parent, const Rect& rect, const Rect& blackArea, int id )
: Widget( parent, id, rect ), _d( new Impl )
{
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

  CONNECT( _d->btnExit, onClicked(), this, InfoBoxLand::deleteLater );

  _d->lbText = new Label( this, Rect( 32, 64, 510 - 32, 300 - 48 ) );

  // black box
  Point lastPos( getWidth() - 32, getHeight() - 48 );
  if( _d->lbBlackFrame )
  {
    _d->lbBlackFrame->setVisible( blackArea.getSize().getArea() > 0 );
    _d->lbBlackFrame->setGeometry( blackArea );
    lastPos.setY( _d->lbBlackFrame->getTop() - 10 );
  }

  if( _d->lbText )
  {
    Rect r = _d->lbText->getRelativeRect();
    r.LowerRightCorner = lastPos;
    _d->lbText->setGeometry( r );
  }

  _afterCreate();

  events::GameEventPtr e = events::Pause::create( events::Pause::hidepause );
  e->dispatch();
}

void InfoBoxSimple::setText( const std::string& text )
{
  if( _d->lbText ) { _d->lbText->setText( text ); }
}

InfoBoxSimple::~InfoBoxSimple()
{
  events::GameEventPtr e = events::Pause::create( events::Pause::hideplay );
  e->dispatch();
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
Label* InfoBoxSimple::_getBlackFrame(){  return _d->lbBlackFrame; }

void InfoBoxSimple::_updateWorkersLabel(const Point &pos, int picId, int need, int have )
{
  if( !_d->lbBlackFrame || 0 == need)
    return;

  _d->lbBlackFrame->setVisible( need > 0 );

  // number of workers
  std::string text = StringHelper::format( 0xff, "%d %s (%d %s)",
                                           have, _("##employers##"),
                                           need, _("##requierd##") );
  _d->lbBlackFrame->setIcon( Picture::load( ResourceGroup::panelBackground, picId ), pos );
  _d->lbBlackFrame->setText( text );
  _d->lbBlackFrame->setTextOffset( Point( pos.getX() + 30, 0 ) );
}

InfoBoxWorkingBuilding::InfoBoxWorkingBuilding( Widget* parent, WorkingBuildingPtr building)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  setTitle( MetaDataHolder::instance().getData( building->getType() ).getPrettyName() );  

  _updateWorkersLabel( Point( 32, 150 ), 542, building->getMaxWorkers(), building->getWorkersCount() );

  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                           (int)building->getState( Construction::damage ),
                                           (int)building->getState( Construction::fire ));

  new Label( this, Rect( 50, getHeight() - 50, getWidth() - 50, getHeight() - 16 ), text );
  new Label( this, Rect( 16, 50, getWidth() - 16, 130 ), "", false, Label::bgNone, lbHelpId );
}

void InfoBoxWorkingBuilding::setText(const std::string& text)
{
  if( Widget* lb = findChild( lbHelpId ) )
      lb->setText( text );
}

InfoBoxSenate::InfoBoxSenate( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 290 ), Rect( 16, 126, 510 - 16, 126 + 62 ) )
{
  SenatePtr senate = tile.getOverlay().as<Senate>();
  setTitle( MetaDataHolder::instance().getData( building::senate ).getPrettyName() );

  // number of workers
  _updateWorkersLabel( Point( 32, 136), 542, senate->getMaxWorkers(), senate->getWorkersCount() );

  std::string denariesStr = StringHelper::format( 0xff, "%s %d", _("##senate_save##"), senate->getFunds() );

  Label* lb = new Label( this, Rect( 60, 35, getWidth() - 16, 35 + 30 ), denariesStr );
  lb->setIcon( GoodHelper::getPicture( Good::denaries ) );
  lb->setText( denariesStr );

  new Label( this, Rect( 60, 215, 60 + 300, 215 + 24 ), _("##open_rating_adviser##") );
  new TexturedButton( this, Point( 350, 215 ), Size(28), -1, 289 );
  new Label( this, Rect( 16, 70, getWidth() - 16, 70 + 60 ), _("##senate_help_text##") );
}

InfoBoxSenate::~InfoBoxSenate()
{
}

GuiInfoFactory::GuiInfoFactory( Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 147, 510 - 16, 147 + 62) )
{
  FactoryPtr building = tile.getOverlay().as<Factory>();
  setTitle( MetaDataHolder::getPrettyName( building->getType() ) );

  // paint progress
  std::string text = StringHelper::format( 0xff, "%s %d%%", _("##production_ready_at##"), building->getProgress() );
  new Label( this, Rect( _d->lbTitle->getLeftdownCorner() + Point( 10, 0 ), Size( getWidth() - 32, 25 ) ), text );

  if( building->getOutGoodType() != Good::none )
  {
    new Image( this, Point( 10, 10), GoodHelper::getPicture( building->getOutGoodType() ) );
  }

  // paint picture of in good
  if( building->getInGood().type() != Good::none )
  {
    Label* lb = new Label( this, Rect( _d->lbTitle->getLeftdownCorner() + Point( 0, 25 ), Size( getWidth() - 32, 25 ) ) );
    lb->setIcon( GoodHelper::getPicture( building->getInGood().type() ) );

    std::string text = StringHelper::format( 0xff, "%s %s: %d %s",
                                             GoodHelper::getName( building->getInGood().type() ).c_str(),
                                             _("##factory_stock##"),
                                             building->getInGood()._currentQty / 100,
                                             _("##factory_units##") );

    lb->setText( text );
    lb->setTextOffset( Point( 30, 0 ) );
  }

  _updateWorkersLabel( Point( 32, 157 ), 542, building->getMaxWorkers(), building->getWorkersCount() );
}

std::string GuiInfoFactory::getInfoText()
{
  return "";
  /*std::string textKey = GoodHelper::getName( _building->getOutGood().type() );
  if (_building->isActive() == false)
  {
    textKey+= "- Production arretee sous ordre du gouverneur";
  }
  else if (_building->getInGood().type() != Good::G_NONE && _building->getInGood()._currentQty == 0)
  {
    textKey+= "- Production arretee par manque de matiere premiere";
  }
  else
  {
    int nbWorkers = _building->getWorkers();
    if (nbWorkers == 0)
    {
       textKey+= "- Production arretee par manque de personnel";
    }
    else if (nbWorkers < 5)
    {
       textKey+= "- Production quasimment a l'arret par manque de main-d'oeuvre";
    }
    else if (nbWorkers < 7)
    {
       textKey+= "- Production tres ralentie par manque d'employes";
    }
    else if (nbWorkers < 9)
    {
       textKey+= "- Production limitee par manque de personnel";
    }
    else if (nbWorkers < 10)
    {
       textKey+= "- Production legerement ralentie par manque d'employes";
    }
    else
    {
       textKey+= "- Production a son maximum";
    }
  }

  return _(textKey.c_str());*/
}

InfoBoxGranary::InfoBoxGranary( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 280 ), Rect( 16, 130, 510 - 16, 130 + 62) )
{
  _granary = tile.getOverlay().as<Granary>();
  Size btnOrdersSize( 350, 20 );
  PushButton* btnOrders = new PushButton( this, Rect( Point( (getWidth() - btnOrdersSize.getWidth())/ 2, getHeight() - 34 ), btnOrdersSize),
                                         _("##special_orders##"), -1, false, PushButton::whiteBorderUp );
  CONNECT( btnOrders, onClicked(), this, InfoBoxGranary::showSpecialOrdersWindow );

  setTitle( MetaDataHolder::getPrettyName( _granary->getType() ) );

  // summary: total stock, free capacity
  std::string desc = StringHelper::format( 0xff, "%d %s %d",
                                           _granary->getGoodStore().getCurrentQty(),
                                           _("##units_in_stock_freespace_for##"),
                                           _granary->getGoodStore().getFreeQty() );

  Label* lbUnits = new Label( this, Rect( _d->lbTitle->getLeftdownCorner(), Size( getWidth() - 16, 40 )), desc );

  drawGood(Good::wheat, 0, lbUnits->getBottom() );
  drawGood(Good::meat, 0, lbUnits->getBottom() + 25);
  drawGood(Good::fruit, 1, lbUnits->getBottom() );
  drawGood(Good::vegetable, 1, lbUnits->getBottom() + 25);

  _updateWorkersLabel( Point( 32, lbUnits->getBottom() + 60 ), 542, _granary->getMaxWorkers(), _granary->getWorkersCount() );
}

InfoBoxGranary::~InfoBoxGranary()
{
}

void InfoBoxGranary::showSpecialOrdersWindow()
{
  Point pos;
  if( getTop() > (int)getParent()->getHeight() / 2 )
  {
    pos = Point( getScreenLeft(), getScreenBottom() - 450 );   
  }
  else
  {
    pos = getAbsoluteRect().UpperLeftCorner;
  }

  new GranarySpecialOrdersWindow( getParent(), pos, _granary );
}

void InfoBoxGranary::drawGood(const Good::Type &goodType, int col, int paintY)
{
  std::string goodName = GoodHelper::getName( goodType );
  int qty = _granary->getGoodStore().getCurrentQty(goodType);
  std::string outText = StringHelper::format( 0xff, "%d %s", qty, goodName.c_str() );

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  Label* lb = new Label( this, Rect( Point( (col == 0 ? 31 : 250), paintY), Size( 24, 50 )) );
  lb->setIcon( pic );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setText( outText );
}

InfoBoxTemple::InfoBoxTemple( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 56, 510 - 16, 56 + 62) )
{
  TemplePtr temple = tile.getOverlay().as<Temple>();
  RomeDivinityPtr divn = temple->getDivinity();

  std::string text = StringHelper::format( 0xff, "##Temple of ##%s (%s)", 
                                                 divn->getName().c_str(), 
                                                 divn->getShortDescription().c_str() );
  setTitle( text );

  _updateWorkersLabel( Point( 32, 56 + 12), 542, temple->getMaxWorkers(), temple->getWorkersCount() );

  new Image( this, Point( 192, 140 ), temple->getDivinity()->getPicture() );
}

InfoBoxTemple::~InfoBoxTemple()
{
}

InfoBoxBuilding::InfoBoxBuilding( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 450, 220 ), Rect( 16, 60, 450 - 16, 60 + 50) )
{
  BuildingPtr building = tile.getOverlay().as<Building>();
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

    if( tile.getOverlay().as<Construction>()->getAccessRoads().size() == 0 )
    {
      setText( _("##freehouse_text_noroad##") );
    }
    else
    {
      setText( _("##freehouse_text##") );
    }
}   

InfoBoxRawMaterial::InfoBoxRawMaterial( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 146, 510 - 16, 146 + 64 ) )
{
  FactoryPtr rawmb = tile.getOverlay().as<Factory>();

  if( rawmb->getOutGoodType() != Good::none )
  {
    Picture pic = GoodHelper::getPicture( rawmb->getOutGoodType() );
    new Image( this, Point( 10, 10 ), pic );
  }

  _updateWorkersLabel( Point( 32, 160 ), 542, rawmb->getMaxWorkers(), rawmb->getWorkersCount() );

  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                          (int)rawmb->getState( Construction::damage ),
                                          (int)rawmb->getState( Construction::fire ) );
  new Label( this, Rect( 50, getHeight() - 50, getWidth() - 50, getHeight() - 16 ), text );

  text = StringHelper::format( 0xff, "%s %d", _("##rawm_production_complete##"), rawmb->getProgress() );
  Label* lbProgress = new Label( this, Rect( 32, 50, getWidth() - 16, 50 + 32 ), text );
  Label* lbAbout = new Label( this, Rect( 32, lbProgress->getBottom() + 6, getWidth() - 16, 130 ) );

  std::string desc, name;
  //GoodType goodType = G_NONE;
  switch( rawmb->getType() )
  {
  case building::wheatFarm:
    desc.assign( _("##farm_description_wheat##") );
    name.assign( _("##farm_title_wheat##") );
  break;

  case building::fruitFarm:
    desc.assign( _("##farm_description_fruit##") );
    name.assign( _("##farm_title_fruit##") );
  break;

  case building::oliveFarm:
    desc.assign( _("##farm_description_olive##") );
    name.assign( _("##farm_title_olive##") );
  break;

  case building::grapeFarm:
    desc.assign( _("##farm_description_vine##") );
    name.assign( _("##farm_title_vine##") );
  break;

  case building::pigFarm:
    desc.assign( _("##farm_description_meat##") );
    name.assign( _("##farm_title_meat##") );
  break;

  case building::vegetableFarm:
    desc.assign( _("##farm_description_vegetable##") );
    name.assign( _("##farm_title_vegetable##") );
  break;

  default:
  break;
  }

  new Label( this, Rect( 32, 236, getWidth() - 50, getHeight() - 50 ), desc );

  setTitle( name );

  text = _("##farm_working_normally##");
  if( rawmb->getWorkersCount() == 0 )
  {
    text = _("##farm_have_no_workers##");
  }
  else if( rawmb->getWorkersCount() <= rawmb->getMaxWorkers() / 2 )
  {
    text = _("##farm_working_bad##");
  }

  lbAbout->setText( text );
}

InfoBoxRawMaterial::~InfoBoxRawMaterial()
{
}

InfoBoxCitizen::InfoBoxCitizen(Widget* parent, const WalkerList& walkers )
  : InfoBoxSimple( parent, Rect( 0, 0, 460, 350 ), Rect( 18, 40, 460 - 18, 350 - 120 ) )
{
  new Label( this, Rect( 25, 100, getWidth() - 25, getHeight() - 130), "", false, Label::bgWhiteBorderA );
  //mini screenshot from citizen pos need here
  new Label( this, Rect( 25, 45, 25 + 52, 45 + 52), "", false, Label::bgBlack );

  Label* lbName = new Label( this, Rect( 90, 108, getWidth() - 30, 108 + 35) );
  lbName->setFont( Font::create( FONT_3 ));
  Label* lbType = new Label( this, Rect( 90, 145, getWidth() - 30, 145 + 25) );
  Label* lbThinks = new Label( this, Rect( 90, 170, getWidth() - 30, getHeight() - 140), "Citizen's thoughts will be placed here" );
  Label* lbCitizenPic = new Label( this, Rect( 30, 112, 30 + 55, 112 + 80) );

  if( !walkers.empty() )
  {
    WalkerPtr walker = walkers.front();
    lbName->setText( walker->getName() );
    lbType->setText( WalkerHelper::getPrettyTypeName( walker->getType() ) );
    lbThinks->setText( walker->getThinks() );
    lbCitizenPic->setBackgroundPicture( WalkerHelper::getBigPicture( walker->getType() ) );
  }
}

InfoBoxCitizen::~InfoBoxCitizen() {
}


InfoBoxColosseum::InfoBoxColosseum(Widget *parent, const Tile &tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 470, 300), Rect( 16, 145, 470 - 16, 145 + 100 ) )
{
  CollosseumPtr colloseum = tile.getOverlay().as<Collosseum>();
  setTitle( MetaDataHolder::getPrettyName( building::colloseum ) );

  _updateWorkersLabel( Point( 40, 150), 542, colloseum->getMaxWorkers(), colloseum->getWorkersCount() );

  std::string text = StringHelper::format( 0xff, "Animal contest runs for another %d days", 0 );
  new Label( this, Rect( 35, 190, getWidth() - 35, 190 + 20 ), text );

  text = StringHelper::format( 0xff, "Gladiator bouts runs for another %d days", 0 );
  new Label( this, Rect( 35, 210, getWidth() - 35, 210 + 20 ), text );
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


InfoBoxFontain::InfoBoxFontain(Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 480, 320 ), Rect( 0, 0, 1, 1 ) )
{
  setTitle( "##fontaun_title##" );

  _d->lbText->setGeometry( Rect( 25, 45, getWidth() - 25, getHeight() - 55 ) );
  _d->lbText->setWordwrap( true );

  FountainPtr fountain = tile.getOverlay().as<Fountain>();
  std::string text;
  if( fountain != 0 )
  {
    if( fountain->isActive() )
    {
      text = _("##fountain_text##");
    }
    else
    {
      text = fountain->haveReservoirAccess()
               ? _("##need_full_reservoir_for_work##")
               : _("##need_reservoir_for_work##");
    }
  }

  _d->lbText->setText( text );
}

InfoBoxFontain::~InfoBoxFontain()
{

}

}//end namespace gui
