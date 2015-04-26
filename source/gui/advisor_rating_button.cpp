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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "advisor_rating_button.hpp"
#include "core/gettext.hpp"
#include "core/utils.hpp"
#include "widget_factory.hpp"

using namespace gfx;

namespace gui
{

namespace advisorwnd
{

REGISTER_CLASS_IN_WIDGETFACTORY(RatingButton)

RatingButton::RatingButton(Widget *parent, Point pos, std::string title, std::string tooltip)
  : PushButton( parent, Rect( pos, Size( 108, 65 ) ), _(title), -1, false, PushButton::whiteBorderUp )
{
  setTextAlignment( align::center, align::upperLeft );
  setTooltipText( _(tooltip) );
  _value = 0;
  _target = 0;
}

RatingButton::RatingButton(Widget *parent)
  : PushButton( parent, Rect( 0, 0, 108, 65 ), "", -1, false, PushButton::whiteBorderUp )
{
  setTextAlignment( align::center, align::upperLeft );
  _value = 0;
  _target = 0;
}

void RatingButton::_updateTextPic()
{
  PushButton::_updateTextPic();

  Font digitFont = Font::create( FONT_4 );
  PictureRef& pic = _textPictureRef();
  if( pic )
  {
    digitFont.draw( *pic, utils::i2str( _value ), width() / 2 - 10, 17, true, false );

    Font targetFont = Font::create( FONT_1 );
    targetFont.draw( *pic, utils::format( 0xff, "%d %s", _target, _("##wndrt_need##") ), 10, height() - 20, true, false );

    pic->update();
  }
}

void RatingButton::setValue(const int value)
{
  _value = value;
  _finalizeResize();
}

void RatingButton::setTarget(const int value)
{
  _target = value;
  _finalizeResize();
}

void advisorwnd::RatingButton::setupUI(const VariantMap &ui)
{
  PushButton::setupUI( ui );
}

}

}//end namespace gui
