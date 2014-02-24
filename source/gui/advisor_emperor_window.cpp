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

#include "advisor_emperor_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "listbox.hpp"
#include "listboxitem.hpp"
#include "core/logger.hpp"
#include "city/city.hpp"
#include "city/request.hpp"
#include "core/foreach.hpp"
#include "good/goodhelper.hpp"
#include "game/gamedate.hpp"
#include "gameautopause.hpp"
#include "city/statistic.hpp"
#include "city/requestdispatcher.hpp"
#include "game/player.hpp"

namespace gui
{

namespace {
  Point requestButtonOffset = Point( 0, 60 );
  Size requestButtonSize = Size( 560, 40 );
}

class RequestButton : public PushButton
{
public:
  RequestButton( Widget* parent, const Point& pos, int index, CityRequestPtr request )
    : PushButton( parent, Rect( pos + requestButtonOffset * index, requestButtonSize), "", -1, false, PushButton::blackBorderUp )
  {
    _request = request;
    _resizeEvent();

    CONNECT( this, onClicked(), this, RequestButton::_executeRequest );
  }

  virtual void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    PictureRef& pic = _getTextPicture( state );

    Font font = Font::create( FONT_1_WHITE );

    GoodRequestPtr gr = ptr_cast<GoodRequest>(_request);
    if( gr.isValid() )
    {
      font.draw( *pic, title, 20, 2 );
      font.draw( *pic, StringHelper::format( 0xff, "%d", gr->getQty() ), 20, 2 );

      Picture goodPicture = GoodHelper::getPicture( gr->getGoodType() );
      pic->draw( goodPicture, Point( 50, 2 ), false );

      fond.draw( *pic, GoodHelper::getTypeName( gr->getGoodType() ) );

      int month2comply = GameDate::current().getMonthToDate( gr->getFinishedDate() );
      font.draw( *pic, StringHelper::format( 0xff, "%d %s", month2comply, _( "##rqst_month_2_comply##") ), 250, 2 );
      font.draw( *pic, gr->getDescription(), 5, pic->getHeight() - 20 );
    }
  }

public oc3_signals:
  Signal1<CityRequestPtr>& onExecRequest() { return _onExecRequestSignal; }

private:
  void _executeRequest() {  _onExecRequestSignal.emit( _request ); }

  Signal1<CityRequestPtr> _onExecRequestSignal;
  CityRequestPtr _request;
};

class AdvisorEmperorWindow::Impl
{
public:
  PlayerCityPtr city;
  int wantSend;
  PictureRef background;
  gui::Label* lbEmperorFavour;
  gui::Label* lbEmperorFavourDesc;
  gui::Label* lbPost;
  gui::Label* lbPrimaryFunds;
  PushButton* btnSendGift;
  PushButton* btnSend2City;
  PushButton* btnChangeSalary; 
  GameAutoPause autoPause;
  bool isRequestsUpdated;

  void sendMoney()
  {
    onSendMoneySignal.emit( wantSend );
  }

  void resolveRequest( CityRequestPtr request )
  {
    if( request.isValid() )
    {
      request->exec( city );
      isRequestsUpdated = true;
    }
  }

public oc3_signals:
  Signal1<int> onChangeSalarySignal;
  Signal1<int> onSendMoneySignal;
};

void AdvisorEmperorWindow::_showChangeSalaryWindow()
{
	Point startPos( (getWidth() - 510) / 2, (getHeight() -400)/2 );
  Rect wdgRect( startPos, Size( 510, 400 ) );
  GroupBox* gb = new GroupBox( this, wdgRect, -1, GroupBox::whiteFrame );

  new gui::Label( gb, Rect( 15, 15, gb->getWidth() - 15, 35), _("##set_mayor_salary##"),  false, gui::Label::bgNone );
  ListBox* lbx = new ListBox( gb, Rect( 16, 50, gb->getWidth() - 16, gb->getHeight() - 100 ) );
  lbx->setItemHeight( 22 );
  lbx->setTextAlignment( alignCenter, alignCenter );
  lbx->setItemFont( Font::create( FONT_2_WHITE ) );

  ListBoxItem* item = &lbx->addItem( _("##citizen_salary##") ); item->setTag( 0 );
  item = &lbx->addItem( _("##clerk_salary##") ); item->setTag( 2 );
  item = &lbx->addItem( _("##engineer_salary##") ); item->setTag( 5 );
  item = &lbx->addItem( _("##architect_salary##") ); item->setTag( 8 );
  item = &lbx->addItem( _("##questor_salary##") ); item->setTag( 12 );
  item = &lbx->addItem( _("##procurate_salary##") ); item->setTag( 20 );
  item = &lbx->addItem( _("##edil_salary##") ); item->setTag( 30 );
  item = &lbx->addItem( _("##pretor_salary##") ); item->setTag( 40 );
  item = &lbx->addItem( _("##consoul_salary##") ); item->setTag( 60 );
  item = &lbx->addItem( _("##proconsoul_salary##") ); item->setTag( 80 );
  item = &lbx->addItem( _("##caesar_salary##") ); item->setTag( 100 );

  PushButton* btn = new PushButton( gb, Rect( Point( 176, gb->getHeight() - 32 ), Size( 160, 20) ), _("##cancel##"), -1, false, PushButton::whiteBorderUp );
  CONNECT( btn, onClicked(), gb, GroupBox::deleteLater );
}

void AdvisorEmperorWindow::_showSend2CityWindow()
{
  _d->wantSend = 0;
  GroupBox* gb = new GroupBox( this, Rect( Point( 50, 175 ), Size( 510, 160 )), -1, GroupBox::whiteFrame );

  gui::Label* lbTitle = new gui::Label( gb, Rect( 85, 15, gb->getWidth() - 85, 40), _("##send_money_to_city##"),  false, gui::Label::bgNone );
  lbTitle->setTextAlignment( alignCenter, alignCenter );
  lbTitle->setFont( Font::create( FONT_3 ) );
  new gui::Label( gb, Rect( Point( 50, 50), Size( 415, 60 ) ), "", false, gui::Label::bgBlack );

  Point start( 65, 55 );
  Point offset( 80, 0 );
  Size btnSize( 65, 15 );
  new PushButton( gb, Rect( start, btnSize ), "0", 0x0f00, false, PushButton::blackBorderUp ); start += offset;
  new PushButton( gb, Rect( start, btnSize ), "500", 0x0f01, false,PushButton::blackBorderUp ); start += offset;
  new PushButton( gb, Rect( start, btnSize ), "2000", 0x0f04, false, PushButton::blackBorderUp ); start += offset;
  new PushButton( gb, Rect( start, btnSize ), "5000", 0x0f0a, false, PushButton::blackBorderUp ); start += offset;
  new PushButton( gb, Rect( start, btnSize ), _("##send_all##"), 0xf0ff, false,PushButton::blackBorderUp );

  Rect btnRect = Rect( 80, gb->getHeight() - 35, 80 + 160, gb->getHeight() - 15 );
  PushButton* btnSend = new PushButton( gb, btnRect, _("##send_money##"), -1, false, PushButton::whiteBorderUp );
  PushButton* btnCancel = new PushButton( gb, btnRect + Point( 190, 0), _("##cancel##"), -1, false, PushButton::whiteBorderUp );

  CONNECT( btnSend, onClicked(), _d.data(), Impl::sendMoney );
  CONNECT( btnSend, onClicked(), gb, GroupBox::deleteLater );
  CONNECT( btnCancel, onClicked(), gb, GroupBox::deleteLater );
}

void AdvisorEmperorWindow::_updateRequests()
{
  Rect reqsRect( Point( 32, 91 ), Size( 570, 220 ) );
  PictureDecorator::draw( *_d->background, reqsRect, PictureDecorator::blackFrame );

  List<RequestButton*> btns = findChildren<RequestButton*>();
  foreach( btn, btns )
  {
    (*btn)->deleteLater();
  }

  CityRequestList reqs;
  CityRequestDispatcherPtr dispatcher = ptr_cast<CityRequestDispatcher>( _d->city->findService( CityRequestDispatcher::getDefaultName() ) );

  if( dispatcher.isValid() )
  {
    reqs = dispatcher->getRequests();
  }

  if( reqs.empty() )
  {
    Label* lb = new Label( this, reqsRect, _("##have_no_requests##") );
    lb->setTextAlignment( alignCenter, alignCenter );
  }
  else
  {
    foreach( request, reqs )
    {
      if( !(*request)->isDeleted() )
      {
        bool mayExec = (*request)->mayExec( _d->city );
        RequestButton* btn = new RequestButton( this, reqsRect.UpperLeftCorner + Point( 5, 5 ), std::distance( request, reqs.begin() ), *request );
        btn->setTooltipText( _("##request_btn_tooltip##") );
        btn->setEnabled( mayExec );
        CONNECT(btn, onExecRequest(), _d.data(), Impl::resolveRequest );
      }
    }
  }
  _d->isRequestsUpdated = false;
}

bool AdvisorEmperorWindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui  )
  {
    if( event.gui.type == guiListboxChanged )
    {
      if( ListBox* lstBox = safety_cast< ListBox* >( event.gui.caller ) )
      {
        _d->onChangeSalarySignal.emit( lstBox->getSelectedItem().getTag() );
      }
    }
    else if( event.gui.type == guiButtonClicked )
    {
      int id = event.gui.caller->getID();
      if( id > 0 && ((id & 0x0f00) == 0x0f00) )
      {
        int multiplier = id & 0xff;
        int money = _d->city->getPlayer()->getMoney();
        _d->wantSend = math::clamp( (multiplier == 0xff ? money : (multiplier * 500)), 0, money);
      }
    }
  }

  return Widget::onEvent( event );
}

Signal1<int>&AdvisorEmperorWindow::onChangeSalary() {  return _d->onChangeSalarySignal;}
Signal1<int>&AdvisorEmperorWindow::onSendMoney(){  return _d->onSendMoneySignal;}

AdvisorEmperorWindow::AdvisorEmperorWindow( PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->autoPause.activate();
  _d->city = city;
  _d->isRequestsUpdated = true;

  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  gui::Label* title = new gui::Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( city->getPlayer()->getName() );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  //buttons _d->_d->background  
  PictureDecorator::draw( *_d->background, Rect( 66, 325, 66 + 510, 325 + 94 ), PictureDecorator::blackFrame );
  
  _d->lbEmperorFavour = new gui::Label( this, Rect( Point( 58, 44 ), Size( 550, 20 ) ), "Favour of the emperor 50" );
  _d->lbEmperorFavourDesc = new gui::Label( this, _d->lbEmperorFavour->getRelativeRect() + Point( 0, 20 ), "The emperor has mixed feelings to you" );

  _d->lbPost = new gui::Label( this, Rect( Point( 70, 336 ), Size( 240, 26 ) ), "Post");
  _d->lbPrimaryFunds = new gui::Label( this, Rect( Point( 70, 370 ), Size( 240, 20 ) ), "PrimaryFunds 0" );

  _d->btnSendGift = new PushButton( this, Rect( Point( 322, 343), Size( 250, 20 ) ), "Send gift", -1, false, PushButton::blackBorderUp );
  _d->btnSend2City = new PushButton( this, Rect( Point( 322, 370), Size( 250, 20 ) ), "Send to city", -1, false, PushButton::blackBorderUp );
  _d->btnChangeSalary = new PushButton( this, Rect( Point( 70, 395), Size( 500, 20 ) ), "Change salary", -1, false, PushButton::blackBorderUp );  
  CONNECT( _d->btnChangeSalary, onClicked(), this, AdvisorEmperorWindow::_showChangeSalaryWindow );
  CONNECT( _d->btnSend2City, onClicked(), this, AdvisorEmperorWindow::_showSend2CityWindow );
}

void AdvisorEmperorWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );
  if( _d->isRequestsUpdated )
  {
    _updateRequests();
  }

  Widget::draw( painter );
}

}//end namespace gui
