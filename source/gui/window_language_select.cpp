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

#include "window_language_select.hpp"
#include "game/settings.hpp"
#include "core/locale.hpp"
#include "walker/name_generator.hpp"
#include "sound/engine.hpp"

CAESARIA_LITERALCONST(talks)
CAESARIA_LITERALCONST(font)

namespace gui
{

namespace dialog
{

LanguageSelect::LanguageSelect(gui::Widget* parent, Size size)
{
  Label( parent, Rect( Point(), windowSize ), "", false, gui::Label::bgWhiteFrame );
  ListBox* lbx = new ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  PushButton* btn = new PushButton( frame, Rect( 0, 0, 1, 1), _("##apply##") );

  WidgetEscapeCloser::insertTo( frame );
  frame->setCenter( parent->center() );
  lbx->setFocus();
  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.95 ) );

  VariantMap languages = config::load( SETTINGS_RC_PATH( langModel ) );
  std::string currentLang = SETTINGS_VALUE( language ).toString();
  int currentIndex = -1;
  for( auto it : languages )
  {
    lbx->addItem( it.first );
    std::string ext = it.second.toMap().get( literals::ext ).toString();
    if( ext == currentLang )
      currentIndex = lbx->itemsCount() - 1;
  }

  lbx->setSelected( currentIndex );

}


}//end namespace dialog

}//end namespace gui
