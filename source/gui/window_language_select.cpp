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

CAESARIA_LITERALCONST(talks)
CAESARIA_LITERALCONST(font)
CAESARIA_LITERALCONST(ext)

namespace gui
{

namespace dialog
{

class LanguageSelect::Impl
{
public:
  vfs::Path model;
};

LanguageSelect::LanguageSelect(gui::Widget* parent, vfs::Path model, const std::string& current, Size size)
  : Label( parent, Rect( Point(), size ), "", false, gui::Label::bgWhiteFrame ),
    _d( new Impl )
{
  auto&& listbox = add<ListBox>( Rect(), -1, true, true );
  auto&& btnSelect = add<PushButton>( Rect(), _("##continue##") );
  listbox.setGeometry( 0.05, 0.05, 0.95, 0.85 );
  btnSelect.setGeometry( 0.1, 0.88, 0.9, 0.95 );

  WidgetEscapeCloser::insertTo( this );  
  moveTo( Widget::parentCenter );

  _d->model = model;
  VariantMap languages = config::load( _d->model );

  for( auto& it : languages )
  {
    std::string languageHash = it.second.toMap().get( literals::ext ).toString();
    auto&& item = listbox.addItem( it.first );
    item.setTag( languageHash );
  }

  listbox.setSelectedTag( current );

  CONNECT( &listbox,   onItemSelected(), this, LanguageSelect::_changeLanguage )
  CONNECT( &btnSelect, onClicked(),      this, LanguageSelect::_apply          )

  listbox.setFocus();
}

LanguageSelect::~LanguageSelect() {}

void LanguageSelect::setDefaultFont(const std::string& fontname)
{
  _defaultFont = fontname;
}

void LanguageSelect::_changeLanguage(const gui::ListBoxItem& item)
{
  VariantMap languages = config::load( _d->model );
  for( auto& it : languages )
  {
    if( item.text() == it.first )
    {
      VariantMap vm = it.second.toMap();
      std::string lang = vm.get( literals::ext ).toString();
      std::string talksArchive = vm.get( literals::talks ).toString();
      std::string newFont  = vm.get( literals::font ).toString();

      if( newFont.empty() )
        newFont = _defaultFont;

      emit onChange( lang, newFont, talksArchive );
      return;
    }
  }
}

void LanguageSelect::_apply()
{
  deleteLater();
  emit onContinue();
}

}//end namespace dialog

}//end namespace gui
