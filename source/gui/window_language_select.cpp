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
#include "listbox.hpp"
#include "pushbutton.hpp"
#include "core/gettext.hpp"
#include "widgetescapecloser.hpp"
#include "core/saveadapter.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"

GAME_LITERALCONST(talks)
GAME_LITERALCONST(font)
GAME_LITERALCONST(ext)

namespace gui
{

namespace dialog
{

LanguageSelect::LanguageSelect(Widget* parent, vfs::Path model, const std::string& current, const Size& size)
  : Label( parent, Rect( Point(), size ), "", false, gui::Label::bgWhiteFrame )
{
  auto& listbox = add<ListBox>( RectF(0.05, 0.05, 0.95, 0.85), -1, true, true );
  auto& btnSelect = add<PushButton>( RectF(0.1, 0.88, 0.9, 0.95), _("##continue##") );

  _loadLanguages( listbox, model );

  listbox.setSelectedWithData( literals::ext, Variant( current ) );

  CONNECT_LOCAL( &listbox,   onItemSelected(), LanguageSelect::_changeLanguage )
  CONNECT_LOCAL( &btnSelect, onClicked(),      LanguageSelect::deleteLater     )
  CONNECT      ( &btnSelect, onClicked(),      &onContinue, Signal0<>::_emit   )

  listbox.setFocus();
  moveToCenter();
  WidgetClose::insertTo( this, KEY_RBUTTON );
}

LanguageSelect::~LanguageSelect() {}

void LanguageSelect::setDefaultFont(const std::string& fontname)
{
  _defaultFont = fontname;
}

void LanguageSelect::_loadLanguages(ListBox& listbox, const vfs::Path& filename )
{
  VariantMap languages = config::load( filename );

  for( const auto& it : languages )
  {
    auto& item = listbox.addItem( it.first );
    item.setData( it.second.toMap() );
  }
}

void LanguageSelect::_changeLanguage(const ListBoxItem& item)
{
  std::string lang         = item.data( literals::ext );
  std::string talksArchive = item.data( literals::talks );
  std::string newFont      = item.data( literals::font );

  if( newFont.empty() )
    newFont = _defaultFont;

  emit onChange( lang, newFont, talksArchive );
}

}//end namespace dialog

}//end namespace gui
