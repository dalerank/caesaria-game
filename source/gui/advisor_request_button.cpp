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

#include "advisor_request_button.hpp"
#include "core/gettext.hpp"
#include "dialogbox.hpp"
#include "good/helper.hpp"
#include "gfx/engine.hpp"
#include "city/city.hpp"
#include "core/utils.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

class RequestButton::Impl
{
public:
  Signal1<RequestPtr> onExecRequestSignal;

  RequestPtr request;
  Picture goodPic;
};

namespace {
  Point requestButtonOffset = Point( 0, 55 );
  Size requestButtonSize = Size( 560, 40 );

  enum { favourLimiter=20, maxFavourValue=100 };
}

RequestButton::RequestButton(Widget* parent, const Point& pos, int index, RequestPtr request)
  : PushButton( parent, Rect( pos + requestButtonOffset * index, requestButtonSize), "", -1, false, PushButton::blackBorderUp ),
    __INIT_IMPL(RequestButton)
{
  __D_IMPL(d,RequestButton)
  d->request = request;

  auto goodRequest = d->request.as<GoodRequest>();
  if( goodRequest.isValid() )
    d->goodPic = good::Helper::picture( goodRequest->goodType() );

  _finalizeResize();

  CONNECT_LOCAL( this, onClicked(), RequestButton::_executeRequest );
}

void RequestButton::_updateTexture()
{
  PushButton::_updateTexture();

  Font font = Font::create( FONT_1_WHITE );

  auto goodRequest = _dfunc()->request.as<GoodRequest>();
  if( goodRequest.isValid() )
  {
    int month2comply = game::Date::current().monthsTo( goodRequest->finishedDate() );

    canvasDraw( utils::i2str( goodRequest->qty() ), Point( 2, 2 ), font );
    canvasDraw( good::Helper::getTypeName( goodRequest->goodType() ), Point( 60, 2 ), font );
    canvasDraw( fmt::format( "{} {}", month2comply, _( "##rqst_month_2_comply##") ), Point( 250, 2 ), font );
    canvasDraw( goodRequest->description(), Point( 5, height() - 20 ), font );
  }
}

Signal1<RequestPtr>&RequestButton::onExecRequest() { return _dfunc()->onExecRequestSignal; }

void RequestButton::_acceptRequest()  { emit _dfunc()->onExecRequestSignal( _dfunc()->request );  }

void RequestButton::_executeRequest()
{
 dialog::Confirmation( ui(),  "", "##dispatch_emperor_request_question##",
                       makeDelegate( this, &RequestButton::_acceptRequest ) );
}

void RequestButton::draw(Engine& painter)
{
  PushButton::draw( painter );
  painter.draw( _dfunc()->goodPic, absoluteRect().lefttop() + Point( 40, 2 ), &absoluteClippingRectRef() );
}

}//end namespace advisorwnd

}//end namespace gui
