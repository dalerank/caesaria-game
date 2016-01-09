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

#include "layers/constants.hpp"
#include "menu.hpp"
#include "texturedbutton.hpp"
#include "gfx/picture.hpp"
#include "core/color.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "buildmenu.hpp"
#include "environment.hpp"
#include "gfx/decorator.hpp"
#include "widgetpositionanimator.hpp"
#include "label.hpp"
#include "gfx/drawstate.hpp"
#include "core/gettext.hpp"
#include "game/minimap_colours.hpp"
#include "gfx/city_renderer.hpp"
#include "gfx/tile.hpp"
#include "gfx/engine.hpp"
#include "city/scribes.hpp"
#include "overlays_menu.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "image.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "city/city.hpp"
#include "extented_date_info.hpp"
#include "core/saveadapter.hpp"
#include "core/osystem.hpp"
#include "game/settings.hpp"
#include "events/playsound.hpp"

using namespace constants;
using namespace gfx;
using namespace city;
using namespace events;

namespace gui
{

enum class AdvToolMode
{
  removeTool = 0xff00 + 1,
  terrainTool,
  maximizeTool,
  undoAction,
  messages,
  disaster,
  toolCount
};

enum { noSubMenu=0, haveSubMenu=1, pushButton=1 };
static Signal0<> invalidAction;

class MessageAnnotation : public Widget
{
public:
  MessageAnnotation( Widget* parent, const Rect& rect, Scribes& messages )
    : Widget( parent, -1, rect )
  {
    active = false;
    pic = Picture( gui::rc.panel, 114 );
    setSubElement( true );
    CONNECT( &messages, onChangeMessageNumber(), this, MessageAnnotation::messagesChanged )
  }

  void messagesChanged(int number)
  {
    active = number > 0;
  }

  virtual void draw(Engine& painter)
  {
    unsigned int time = DateTime::elapsedTime() ;
    if( enabled() && active && (time % 1000 < 500) )
    {
      Rect rect( Point(), pic.size() );
      painter.draw( pic, rect, absoluteRect(),  &absoluteClippingRectRef() );
    }
  }

  bool active;
  Picture pic;
};

struct Menu::Link
{
  typedef enum { buildHouse,  editTerrain,
                 clearLand, editForest,
                 buildRoad,   editWater,
                 buildWater,  editRocks,
                 buildHealth, editMeadow,
                 buildTemple, editPlateau,
                 buildEducation, editRift,
                 buildEntertainment, editRiver,
                 buildGovt, editIndigene,
                 buildEngineering, editRoads,
                 buildSecurity, editBorders,
                 buildCommerce, editAttacks,
                 undoAction,
                 messages,
                 disaster
               } Name;
  typedef enum { inGame=0, inEditor=1 } VisibleMode;
  Point pos;
  int picId;
  int guiId;
  int index;
  int mode;
  int midPicID;
  bool isPushBtn;
  bool submenu;
  std::string sound;
  PushButton* button;
  std::string tooltip;
  VisibleMode visibleMode;
  Rect rect;

  void setEnabled(bool enabled) { if( button ) button->setEnabled(enabled); }
  Signal0<>& action() { return button ? button->onClicked() : invalidAction; }
};

struct Menu::Model
{
  typedef enum { smallMenu=0, bigMenu } MenuType;
  Picture background, bottom;
  int width;
  float scale;
  MenuType mtype;
  bool fitToScreen;  
  std::map<Link::Name, Menu::Link> actions;

  void setEnabled( Link::Name name, bool enabled )
  {
    if( actions[ name ].button )
      actions[ name ].button->setEnabled( enabled );
  }

  void initDefaultActions()
  {
    actions[ Link::buildHouse  ] = { {13,277}, gui::button.house, object::house, 0,
                                     object::house, gui::miniature.house, pushButton,
                                     noSubMenu, "housing", nullptr, "housing", Link::inGame };

    actions[ Link::editTerrain ] = { {13,277}, gui::button.terrain, object::terrain, 0,
                                     object::terrain, gui::miniature.clear, pushButton,
                                     noSubMenu, "terrain", nullptr, "terrain", Link::inEditor };

    actions[ Link::clearLand   ] = { {63,277}, gui::button.clear, (int)AdvToolMode::removeTool, 1,
                                     (int)AdvToolMode::removeTool, gui::miniature.clear, pushButton,
                                     noSubMenu, "clear_land", nullptr, "clear", Link::inGame };

    actions[ Link::editForest  ] = { {63,277}, gui::button.forest, object::tree, 0,
                                     object::tree, gui::miniature.clear, pushButton,
                                     noSubMenu, "forest", nullptr, "forest", Link::inEditor };

    actions[ Link::buildRoad   ] = { {113,277}, gui::button.road, object::road,  2,
                                     object::road, gui::miniature.road, pushButton,
                                     noSubMenu, "road", nullptr, "road", Link::inGame };

    actions[ Link::editWater   ] = { {113,277}, gui::button.water, object::water,  2,
                                     object::water, gui::miniature.clear, pushButton,
                                     noSubMenu, "water", nullptr, "water", Link::inEditor};

    actions[ Link::buildWater  ] = { {13,313}, gui::button.waterSupply, development::water,  3,
                                     development::water, gui::miniature.water, pushButton,
                                     haveSubMenu, "water", nullptr, "water", Link::inGame };

    actions[ Link::editRocks ] = { {13,313}, gui::button.rocks, object::rock,  3,
                                     object::rock, gui::miniature.clear, pushButton,
                                     noSubMenu, "smRock", nullptr, "smRock", Link::inEditor};

    actions[ Link::buildHealth  ] = { {63,313}, gui::button.health, development::health,  4,
                                      development::health, gui::miniature.health, pushButton,
                                      haveSubMenu, "health", nullptr, "health", Link::inGame };

    actions[ Link::editMeadow ] = { {63,313}, gui::button.meadow, object::meadow,  4,
                                     object::meadow, gui::miniature.clear, pushButton,
                                     noSubMenu, "meadow", nullptr, "meadow", Link::inEditor};

    actions[ Link::buildTemple  ] = { {113,313}, gui::button.temple, development::religion,  5,
                                      development::religion, gui::miniature.religion, pushButton,
                                      haveSubMenu, "temples", nullptr, "temples", Link::inGame };

    actions[ Link::editPlateau ] = { {113,313}, gui::button.plateau, object::plateau,  5,
                                     object::plateau, gui::miniature.clear, pushButton,
                                     noSubMenu, "plateau", nullptr, "plateau", Link::inEditor};

    actions[ Link::buildEducation]={ {13,349}, gui::button.education, development::education,  6,
                                      development::education, gui::miniature.education, pushButton,
                                      haveSubMenu, "education", nullptr, "education", Link::inGame };

    actions[ Link::editRift ] = { {13,349}, gui::button.rift, object::rift,  6,
                                   object::rift, gui::miniature.clear, pushButton,
                                   noSubMenu, "rift", nullptr, "rift", Link::inEditor};

    actions[ Link::buildEntertainment]={ {63,349}, gui::button.entertainment, development::entertainment,  7,
                                         development::entertainment, gui::miniature.entertainment, pushButton,
                                         haveSubMenu, "entertainment", nullptr, "entertainment", Link::inGame };

    actions[ Link::editRiver ] = { {63,349}, gui::button.river, object::river,  7,
                                  object::river, gui::miniature.clear, pushButton,
                                  noSubMenu, "river", nullptr, "river", Link::inEditor};

    actions[ Link::buildGovt ]={ {113,349}, gui::button.govt, development::administration, 8,
                                 development::administration, gui::miniature.administration, pushButton,
                                 haveSubMenu, "administration", nullptr, "administration", Link::inGame };

    actions[ Link::editIndigene ] = { {113,349}, gui::button.indigene, object::native_hut,  8,
                                  object::native_hut, gui::miniature.clear, pushButton,
                                  noSubMenu, "indigene", nullptr, "indigene", Link::inEditor};

    actions[ Link::buildEngineering ]={ {13,385}, gui::button.engineering, development::engineering, 9,
                                 development::engineering, gui::miniature.engineering, pushButton,
                                 haveSubMenu, "engineering", nullptr, "engineering", Link::inGame };

    actions[ Link::editRoads ] = { {13,385}, gui::button.broad, object::road,  9,
                                  object::road, gui::miniature.clear, pushButton,
                                  noSubMenu, "road", nullptr, "road", Link::inEditor};

    actions[ Link::buildSecurity ]={ {63,385}, gui::button.security, development::security, 10,
                                 development::security, gui::miniature.security, pushButton,
                                 haveSubMenu, "security", nullptr, "security", Link::inGame };

    actions[ Link::editBorders ] = { {63,385}, gui::button.waymark, object::waymark,  10,
                                  object::waymark, gui::miniature.clear, pushButton,
                                  noSubMenu, "waymark", nullptr, "waymark", Link::inEditor};

    actions[ Link::buildCommerce ]={ {113,385}, gui::button.commerce, development::commerce, 11,
                                 development::commerce, gui::miniature.commerce, pushButton,
                                 haveSubMenu, "comerce", nullptr, "comerce", Link::inGame };

    actions[ Link::editAttacks ] = { {113,385}, gui::button.attacks, object::attackTrigger,  11,
                                  object::attackTrigger, gui::miniature.clear, pushButton,
                                  noSubMenu, "attackTrigger", nullptr, "attackTrigger", Link::inEditor};

    actions[ Link::messages   ] = { {63, 421}, gui::button.messages, (int)AdvToolMode::messages, 12,
                                    (int)AdvToolMode::messages, gui::miniature.clear, pushButton,
                                    noSubMenu, "message", nullptr, "message", Link::inGame };

    actions[ Link::disaster   ] = { {113, 421}, gui::button.disaster, (int)AdvToolMode::disaster, 13,
                                    (int)AdvToolMode::disaster, gui::miniature.clear, pushButton,
                                    noSubMenu, "troubles", nullptr, "troubles", Link::inGame };

    actions[ Link::undoAction ] = { {13,421}, gui::button.undo, (int)AdvToolMode::undoAction, 14,
                                    (int)AdvToolMode::undoAction, gui::miniature.clear, pushButton,
                                    noSubMenu, "cancel", nullptr, "cancel", Link::inGame};
  }

  bool isLinkValid( Link::Name name ) const
  {
    auto it = actions.find( name );

    if( it == actions.end() )
      return false;

    return it->second.button != nullptr;
  }

  void setConstructoMode( bool enabled )
  {
    Link::VisibleMode mode = enabled ? Link::inEditor : Link::inGame;
    for( auto& item : actions )
    {
      if( item.second.button )
        item.second.button->setVisible( item.second.visibleMode == mode );
    }
  }

  Model( Widget* parent, bool fit, const std::string& name, MenuType mode )
   : fitToScreen( fit )
  {
    VariantMap config = config::load( name );

    VARIANT_LOAD_PICTURE(bottom, config)
    VARIANT_LOAD_PICTURE(background, config)

    mtype = mode;

    if( !background.isValid() )
      background.load( gui::rc.panel, mode == smallMenu ? 16 : 17 );

    if( !bottom.isValid() )
      bottom.load( gui::rc.panel, mode == smallMenu ? 21 : 20 );

    scale = 1;
    if( fitToScreen )
      scale = (parent->height() * 0.9f) / (float)background.height();
    width = background.width() * scale;

    initDefaultActions();
  }
};

class Menu::Impl
{
public:
  struct {
    Pictures fallback;
    Rects rects;
    Batch batch;

    void add( Picture pic, const Rect& r )
    {
      fallback.push_back( pic );
      rects.push_back( r );

      batch.destroy();
      batch.load( fallback, rects );
    }

    void update( const Point& move )
    {
      //no move, do nothing
      if( move == Point( 0, 0) )
        return;

      for( auto& r : rects )
        r += move;

      batch.destroy();
      batch.load( fallback, rects );
    }
  } bg;

  Widget* lastPressed;

  struct {
    PushButton* minimize;
  } button;

  PushButton* senateButton;
  PushButton* empireButton;
  PushButton* missionButton;
  PushButton* northButton;
  PushButton* rotateLeftButton;
  PushButton* rotateRightButton;
  PushButton* overlaysButton;

  Image* middleLabel;
  OverlaysMenu* overlaysMenu; 
  float koeff;
  Menu::Side side;
  PlayerCityPtr city;
  ScopedPtr<Menu::Model> model;

  struct {
    Signal1<int> onCreateConstruction;
    Signal1<int> onCreateObject;
    Signal0<> onHide;
  } signal;

  void initActionButton(PushButton* btn, const Point& pos,
                        bool pushBtn=true);
  void playSound(Widget* widget);
  void updateBuildingOptions();
};

Signal1<int>& Menu::onCreateConstruction(){  return _d->signal.onCreateConstruction;}
Signal1<int>& Menu::onCreateObject(){  return _d->signal.onCreateObject;}
Signal0<>& Menu::onRemoveTool(){  return _d->model->actions[ Link::clearLand ].action();}

class MenuButton : public TexturedButton
{
public:
  MenuButton( Widget* parent, const Point& pos, int id, int midIconId, int startPic, bool pushBtn )
    : TexturedButton( parent, pos, Size( 39, 26 ), id, startPic )
  {
    _midIconId = midIconId;
    setIsPushButton( pushBtn );
  }

  int miniatureIndex() const { return _midIconId; }
  void setMidPicId( int id ) { _midIconId = id; }
  void setSound( const std::string& name ) { addProperty( "sound", name ); }
private:
  int _midIconId;
};

Menu::Menu(Widget* parent, int id, const Rect& rectangle , PlayerCityPtr city)
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  setupUI( ":/gui/shortmenu.gui" );
  _d->city = city;
  _d->side = rightSide;
  _d->lastPressed = 0;
  _d->overlaysMenu = 0;
}

void Menu::_updateButtons()
{
  _d->button.minimize = _addButton( gui::button.maximize, false, 0, (int)AdvToolMode::maximizeTool,
                                    noSubMenu, gui::miniature.empty, "show_bigpanel",
                                    Rect( Point( 6, 4 ), Size( 31, 20 ) ) );

  _createLink( _d->model->actions[ Link::buildHouse ] );
  _createLink( _d->model->actions[ Link::clearLand  ] );
  _createLink( _d->model->actions[ Link::buildRoad  ] );
  _createLink( _d->model->actions[ Link::buildWater ] );
  _createLink( _d->model->actions[ Link::buildHealth ]);
  _createLink( _d->model->actions[ Link::buildTemple ]);
  _createLink( _d->model->actions[ Link::buildEducation ]);
  _createLink( _d->model->actions[ Link::buildEntertainment ]);
  _createLink( _d->model->actions[ Link::buildGovt ]);
  _createLink( _d->model->actions[ Link::buildEngineering ]);
  _createLink( _d->model->actions[ Link::buildSecurity]);
  _createLink( _d->model->actions[ Link::buildCommerce]);
  _createLink( _d->model->actions[ Link::undoAction ] );
  _createLink( _d->model->actions[ Link::messages ] );
  _createLink( _d->model->actions[ Link::disaster ] );

  Rect rect( 2, 2, 23, 20 );
  auto messagesButton = _d->model->actions[ Link::messages ].button;
  if( !_d->city->getOption( PlayerCity::c3gameplay ) )
    rect = Rect( Point(0,0), messagesButton->size() );

  messagesButton->add<MessageAnnotation>( rect, _d->city->scribes() );

  CONNECT( _d->button.minimize, onClicked(), this, Menu::minimize );

  _d->model->actions[ Link::undoAction ].setEnabled( false );
  _d->model->actions[ Link::disaster ].setEnabled( false );
}

void Menu::_setModel( Model* model )
{
  Impl& d = *_d.data();
  d.model.reset( model );
  d.koeff = model->scale;
  d.bg.add( model->background, Rect( Point(0, 0), model->background.size() * model->scale ) );

  unsigned int y = model->background.height() * model->scale;
  while( y < parent()->height() )
  {
    d.bg.add( model->bottom, Rect( 0, y, model->width, y + model->bottom.height() * model->scale ) );
    y += model->bottom.height() * model->scale - 5;
  }
}

void Menu::_setChildGeometry( Widget* w, const Rect& r)
{
  if( !w )
    return;

  w->setGeometry( r * _d->koeff );
}

void Menu::_updateBuildOptions()
{
  _d->updateBuildingOptions();
}

void Menu::_createLink( Link& link )
{
  auto btn = _addButton( link.picId,   link.isPushBtn, link.index,
                         link.guiId,   link.submenu,   link.midPicID,
                         link.tooltip, link.rect );

  if( _d->model->mtype == Model::bigMenu )
  {
    _d->initActionButton( btn, link.pos, link.isPushBtn );
  }

  link.button = btn;
}

PushButton* Menu::_addButton( int startPic, bool pushBtn, int yMul, 
                              int id, bool haveSubmenu, int midPic,
                              const std::string& ident, const Rect& rect )
{
  Point offset( 1, 32 );
  int dy = 35;

  MenuButton& ret = add<MenuButton>( Point( 0, 0 ), -1, -1, startPic, pushBtn );
  ret.setID( id | ( haveSubmenu ? BuildMenu::subMenuCreateIdHigh : 0 ) );
  Point temp = offset + Point( 0, dy * yMul );
  if( _d->koeff != 1 )
  {
    temp.setX( ceil( temp.x() * _d->koeff) );
    temp.setY( temp.y() * _d->koeff );
    ret.setWidth( ceil( ret.width() * _d->koeff ) );
    ret.setHeight( ceil( ret.height() * _d->koeff ) );
  }
  ret.setPosition( temp );
  ret.setTooltipText( _( "##extm_"+ident+"_tlp##" ) );
  ret.setSound( "extm_" + ident );
  ret.setMidPicId( midPic );

  if( rect.width() > 0 )
    _setChildGeometry( &ret, rect );

  return &ret;
}

/* here will be helper functions for minimap generation */
void Menu::draw(gfx::Engine& painter)
{
  if( !visible() )
    return;

  DrawState pipe( painter, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  pipe.draw( _d->bg.batch )
      .fallback( _d->bg.fallback );
    
  Widget::draw( painter );
}

void Menu::setPosition(const Point& relativePosition)
{
  Point oldPos = lefttop();
  Widget::setPosition( relativePosition );
  _d->bg.update( relativePosition - oldPos );
}

bool Menu::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    if( !event.gui.caller )
        return false;

    int id = event.gui.caller->ID();
    if( id == object::house || id == object::road )
    {
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit _d->signal.onCreateConstruction( id );
    }
    else if( id == object::terrain || id == object::tree
             || id == object::water || id == object::rock
             || id == object::meadow || id == object::plateau
             || id == object::river )
    {
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit _d->signal.onCreateObject( id );
    }
    else if( id == (int)AdvToolMode::removeTool )
    {
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit onRemoveTool();
    }
    else if( id == (int)AdvToolMode::undoAction )
    {
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit onUndo();
    }
    else if( id == (int)AdvToolMode::messages )
    {
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit onMessagesShow();
    }
    else
    {
      if( _d->lastPressed != event.gui.caller )
      {
        if( event.gui.caller->parent() == this )
            _d->lastPressed = event.gui.caller;

        if( PushButton* btn = safety_cast< PushButton* >( event.gui.caller ) )
        {
          int id = btn->ID();
          if( id & BuildMenu::subMenuCreateIdHigh )
          {
            _createBuildMenu( id & 0xff, event.gui.caller );
          }
          else
          {
            emit _d->signal.onCreateConstruction( id );
            _createBuildMenu( -1, this );
            setFocus();
          }
        }
      }
    }

    unselectAll();
    if( PushButton* btn = safety_cast< PushButton* >( _d->lastPressed ) )
    {
      btn->setPressed( true && btn->isPushButton() );
    }
    return true;
  }

  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type )
    {
    case mouseRbtnRelease:
      _createBuildMenu( -1, this );
      cancel();
    return true;

    case mouseLbtnPressed:
    case mouseLbtnRelease:
    {
      //lock movement for tilemap
      if( findChildren<BuildMenu*>().size() > 0 )
        return true;
    }
    break;

    default: break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    if( event.keyboard.key == KEY_ESCAPE )
    {
      auto menus = findChildren<BuildMenu*>();
      for( auto m : menus ) m->deleteLater();
    }

    return true;
  }

  return Widget::onEvent( event );
}

Menu* Menu::create(Widget* parent, int id, PlayerCityPtr city, bool fitToScreen )
{
  auto model = new Model( parent, fitToScreen, ":/menu.model", Model::smallMenu );

  Menu& ret = parent->add<Menu>( id, Rect( 0, 0, model->width, parent->height() ), city );

  ret._setModel( model );
  ret._updateButtons();
  ret._updateBuildOptions();

  CONNECT( city, onChangeBuildingOptions(), &ret, Menu::_updateBuildOptions );

  return &ret;
}

void Menu::minimize()
{
  _d->lastPressed = 0;
  _createBuildMenu( -1, this );
  Point stopPos = lefttop() + Point( width(), 0 ) * (_d->side == Menu::leftSide ? -1 : 1 );
  auto& animator = add<PositionAnimator>( WidgetAnimator::removeSelf, stopPos, 300 );
  CONNECT( &animator, onFinish(), &_d->signal.onHide, Signal0<>::_emit );

  events::dispatch<PlaySound>( "panel", 3, 100 );
}

void Menu::maximize()
{
  Point stopPos = lefttop() - Point( width(), 0 ) * (_d->side == Menu::leftSide ? -1 : 1 );
  show();
  add<PositionAnimator>( WidgetAnimator::showParent | WidgetAnimator::removeSelf, stopPos, 300 );

  events::dispatch<PlaySound>( "panel", 3, 100 );
}

void Menu::cancel()
{
  unselectAll();
  _d->lastPressed = 0;
}

void Menu::setSide(Menu::Side side, const Point& offset)
{
  _d->side = side;
  switch( _d->side )
  {
  case leftSide:
    setPosition( {static_cast<int>(offset.x() - (visible()?0:1) * width()), offset.y()} );
  break;

  case rightSide:
    setPosition( {static_cast<int>(offset.x() - (visible()?1:0) * width()), offset.y()} );
  break;
  }
}

bool Menu::unselectAll()
{
  bool anyPressed = false;
  auto buttons = children().select<PushButton>();
  for( auto btn : buttons )
  {
    anyPressed |= btn->isPressed();
    btn->setPressed( false );
  }

  return anyPressed;
}

void Menu::_createBuildMenu( int type, Widget* parent )
{
   auto menus = findChildren<BuildMenu*>();
   for( auto m : menus )
     m->deleteLater();

   BuildMenu* buildMenu = BuildMenu::create( (development::Branch)type, this,
                                             _d->city->getOption( PlayerCity::c3gameplay ) );

   if( buildMenu != NULL )
   {
     buildMenu->setNotClipped( true );
     buildMenu->setBuildOptions( _d->city->buildOptions() );
     buildMenu->setModel( SETTINGS_RC_PATH( buildMenuModel ).toString() );
     buildMenu->initialize();

     int y = math::clamp< int >( parent->screenTop() - screenTop(), 0, ui()->rootWidget()->height() - buildMenu->height() );
     buildMenu->setPosition( Point( -(int)buildMenu->width() - 5, y ) );
   }
}

Signal0<>& Menu::onHide() { return _d->signal.onHide; }
Signal0<>& Menu::onMessagesShow()  { return _d->model->actions[ Link::messages ].action(); }
Signal0<>& Menu::onUndo() { return _d->model->actions[ Link::undoAction ].action(); }
Signal0<>& Menu::onSwitchAlarm(){  return _d->model->actions[ Link::disaster].action(); }

void Menu::Impl::initActionButton(PushButton* btn, const Point& pos, bool pushBtn )
{
  btn->setPosition( pos * koeff );
  btn->setIsPushButton( pushBtn );
  CONNECT( btn, onClickedEx(), this, Impl::playSound );
}

void Menu::Impl::playSound( Widget* widget )
{
  std::string sound = widget->getProperty( "sound" ).toString();
  int index = 1;
  if( sound.empty() )
  {
    sound = "panel";
    index = math::random( 2 ) + 1;
  }

  events::dispatch<PlaySound>( sound, index, 100 );
}

void Menu::Impl::updateBuildingOptions()
{
  const development::Options& options = city->buildOptions();
  model->setEnabled( Link::buildWater, options.isGroupAvailable( development::water ));
  model->setEnabled( Link::buildGovt, options.isGroupAvailable( development::administration ));
  model->setEnabled( Link::buildEntertainment, options.isGroupAvailable( development::entertainment ));
  model->setEnabled( Link::buildEducation, options.isGroupAvailable( development::education ));
  model->setEnabled( Link::buildTemple, options.isGroupAvailable( development::religion ));
  model->setEnabled( Link::buildCommerce, options.isGroupAvailable( development::commerce ));
  model->setEnabled( Link::buildSecurity, options.isGroupAvailable( development::security ));
  model->setEnabled( Link::buildHealth, options.isGroupAvailable( development::health ));
  model->setEnabled( Link::buildEngineering, options.isGroupAvailable( development::engineering ));
}

ExtentMenu* ExtentMenu::create(Widget* parent, int id, PlayerCityPtr city , bool fitToScreen)
{
  auto model = new Model( parent, fitToScreen, ":/extmenu.model", Model::bigMenu );

  ExtentMenu& ret = parent->add<ExtentMenu>( id, Rect( 0, 0, model->width, parent->height() ), city );
  ret.setID( Hash( TEXT(ExtentMenu)) );
  ret._setModel( model );
  ret._updateButtons();
  ret._updateBuildOptions();

  CONNECT( city, onChangeBuildingOptions(), &ret, ExtentMenu::_updateBuildOptions );

  return &ret;
}

ExtentMenu::ExtentMenu(Widget* p, int id, const Rect& rectangle, PlayerCityPtr city )
    : Menu( p, id, rectangle, city )
{
  setupUI( ":/gui/fullmenu.gui" );
  _d->city = city;
}

void ExtentMenu::_updateButtons()
{
  Menu::_updateButtons();

  _d->button.minimize->deleteLater();
  _d->button.minimize = _addButton( 97, false, 0, (int)AdvToolMode::maximizeTool, false, gui::miniature.empty,
                                   "hide_bigpanel" );

  _setChildGeometry( _d->button.minimize, Rect( Point( 127, 5 ), Size( 31, 20 ) ) );

  //header
  _d->senateButton = _addButton( 79, false, 0, -1, false, -1, "senate" );
  _setChildGeometry( _d->senateButton, Rect( Point( 7, 155 ), Size( 71, 23 ) ) );

  _d->empireButton = _addButton( 82, false, 0, -1, false, -1, "empire" );
  _setChildGeometry( _d->empireButton, Rect( Point( 84, 155 ), Size( 71, 23 ) ) );

  _d->missionButton = _addButton( 85, false, 0, -1, false, -1, "mission" );
  _setChildGeometry( _d->missionButton, Rect( Point( 7, 184 ), Size( 33, 22 ) ) );

  _d->northButton = _addButton( 88, false, 0, -1, false, -1, "reorient_map_to_north" );
  _setChildGeometry( _d->northButton, Rect( Point( 46, 184 ), Size( 33, 22 ) ) );

  _d->rotateLeftButton = _addButton( 91, false, 0, -1, false, -1, "rotate_map_counter_clockwise" );
  _setChildGeometry( _d->rotateLeftButton, Rect( Point( 84, 184 ), Size( 33, 22 ) ) );

  _d->rotateRightButton = _addButton( 94, false, 0, -1, false, -1, "rotate_map_clockwise" ) ;
  _setChildGeometry( _d->rotateRightButton, Rect( Point( 123, 184 ), Size( 33, 22 ) ) );   

  _d->middleLabel = &add<Image>( Rect( 0, 0, 1, 1 ), Picture(), Image::fit );
  _setChildGeometry( _d->middleLabel, Rect( Point( 7, 216 ), Size( 148, 52 )) );
  _d->middleLabel->setPicture( gui::miniature.rc, gui::miniature.empty );

  _d->overlaysMenu = &parent()->add<OverlaysMenu>( Rect( 0, 0, 160, 1 ), -1 );
  _d->overlaysMenu->hide();

  _d->overlaysButton = &add<PushButton>( Rect( 0, 0, 1, 1 ), _("##ovrm_text##"), -1, false, PushButton::greyBorderLineFit );
  _setChildGeometry( _d->overlaysButton, Rect( 4, 3, 122, 28 ) );
  _d->overlaysButton->setTooltipText( _("##select_city_layer##") );

  CONNECT( _d->button.minimize, onClicked(), this, ExtentMenu::minimize );
  CONNECT( _d->overlaysButton, onClicked(), this, ExtentMenu::toggleOverlayMenuVisible );
  CONNECT( _d->overlaysMenu, onSelectOverlayType(), this, ExtentMenu::changeOverlay );
}

bool ExtentMenu::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    MenuButton* btn = safety_cast< MenuButton* >( event.gui.caller );
    if( btn )
    {
      int picId = btn->miniatureIndex() > 0 ? btn->miniatureIndex() : gui::miniature.empty;
      _d->middleLabel->setPicture( gui::miniature.rc, picId );
    }
  }

  return Menu::onEvent( event );
}

void ExtentMenu::draw(Engine& painter )
{
  if( !visible() )
    return;

  Menu::draw( painter );
}

void ExtentMenu::toggleOverlayMenuVisible()
{
  _d->overlaysMenu->setPosition( Point( screenLeft() - 170, 74 ) );
  _d->overlaysMenu->setVisible( !_d->overlaysMenu->visible() );
}

void ExtentMenu::setConstructorMode(bool enabled)
{
  PushButton* btns1[] = { _d->senateButton, _d->empireButton,
                          _d->missionButton, _d->northButton,
                          _d->rotateLeftButton, _d->rotateRightButton,
                          _d->model->actions[ Link::messages ].button,
                          _d->model->actions[ Link::disaster ].button };

  for( auto btn : btns1 )
    btn->setEnabled( !enabled );

  if( !_d->model->isLinkValid( Link::editTerrain ) )
  {
    _createLink( _d->model->actions[ Link::editTerrain ] );
    _createLink( _d->model->actions[ Link::editForest  ] );
    _createLink( _d->model->actions[ Link::editWater   ] );
    _createLink( _d->model->actions[ Link::editRocks    ] );
    _createLink( _d->model->actions[ Link::editMeadow  ] );
    _createLink( _d->model->actions[ Link::editPlateau ] );
    _createLink( _d->model->actions[ Link::editRift    ] );
    _createLink( _d->model->actions[ Link::editRiver   ] );
    _createLink( _d->model->actions[ Link::editIndigene] );
    _createLink( _d->model->actions[ Link::editRoads   ] );
    _createLink( _d->model->actions[ Link::editBorders ] );
    _createLink( _d->model->actions[ Link::editAttacks ] );
  }

  _d->model->setConstructoMode( enabled );
}

void ExtentMenu::resolveUndoChange(bool enabled) { _d->model->actions[ Link::undoAction ].setEnabled( enabled ); }

void ExtentMenu::changeOverlay(int ovType)
{
  std::string layerName = citylayer::Helper::prettyName( (citylayer::Type)ovType );
  _d->overlaysButton->setText( _( layerName ) );
}

void ExtentMenu::showInfo(int type)
{
  int hash = Hash( TEXT(ExtentedDateInfo));
  ExtentedDateInfo* window = safety_cast<ExtentedDateInfo*>( findChild( hash ) );
  if( !window )
  {
    window = &add<ExtentedDateInfo>( Rect( Point(), size() ), hash );
  }
  else
  {
    window->deleteLater();
  }
}

void ExtentMenu::setAlarmEnabled( bool enabled )
{
  if( enabled )
  {
    events::dispatch<PlaySound>( "extm_alarm", 1, 100, audio::effects );
  }

  _d->model->actions[ Link::disaster ].setEnabled( enabled );
}

Rect ExtentMenu::getMinimapRect() const
{
  Rect r( Point( 8, 35), Size( 144, 110 ) );
  return r * _d->koeff;
}

Signal1<int>& ExtentMenu::onSelectOverlayType() {  return _d->overlaysMenu->onSelectOverlayType(); }
Signal0<>& ExtentMenu::onEmpireMapShow(){  return _d->empireButton->onClicked(); }
Signal0<>& ExtentMenu::onAdvisorsWindowShow(){  return _d->senateButton->onClicked(); }
Signal0<>& ExtentMenu::onRotateRight() { return _d->rotateRightButton->onClicked(); }
Signal0<>& ExtentMenu::onRotateLeft() { return _d->rotateLeftButton->onClicked(); }
Signal0<>& ExtentMenu::onMissionTargetsWindowShow(){  return _d->missionButton->onClicked(); }

}//end namespace gui
