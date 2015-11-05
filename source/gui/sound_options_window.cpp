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

#include "sound_options_window.hpp"
#include "gameautopause.hpp"
#include "core/event.hpp"
#include "label.hpp"
#include "widget_helper.hpp"
#include "core/utils.hpp"
#include "sound/constants.hpp"
#include "core/logger.hpp"
#include "spinbox.hpp"
#include "widgetescapecloser.hpp"

namespace gui
{

namespace dialog
{

class SoundOptions::Impl
{
public:
  bool initialized;

  struct {
    Signal2<audio::SoundType,audio::Volume> onSoundChange;
    Signal0<> onClose, onApply, onRequest;
  } signal;

  void resolveChange( SpinBox* who, int value )
  {
    int type = who->getProperty( "soundType" );
    emit signal.onSoundChange( audio::SoundType(type), audio::Volume(value) );
  }
};

SoundOptions::SoundOptions(Widget* parent)
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->initialized = false;
  setupUI( ":/gui/soundoptions.gui" );

  setCenter( parent->center() );

  WidgetEscapeCloser::insertTo( this );
  GameAutoPause::insertTo( this );

  INIT_WIDGET_FROM_UI( PushButton*, btnOk )
  if( btnOk ) btnOk->setFocus();

  auto widgets = findChildren<SpinBox*>( true );
  for( auto wdg : widgets )
    CONNECT( wdg, onChangeA(), _d.data(), Impl::resolveChange )
}

SoundOptions::~SoundOptions( void ) {}

bool SoundOptions::onEvent(const NEvent& event)
{  
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->ID();
    switch( id )
    {
    case 1001:
      emit _d->signal.onApply();
      deleteLater();
    break;

    case 1002:
    {
      emit _d->signal.onClose();
      deleteLater();
    }
    break;

    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal2<audio::SoundType, audio::Volume>& SoundOptions::onChange() { return _d->signal.onSoundChange; }
Signal0<>& SoundOptions::onClose()                       { return _d->signal.onClose; }
Signal0<>& SoundOptions::onApply()                       { return _d->signal.onApply; }

void SoundOptions::update(audio::SoundType type, audio::Volume value)
{
  auto widgets = findChildren<SpinBox*>( true );

  for( auto wdg : widgets )
  {
    if( wdg->getProperty( "soundType" ).toInt() == type )
    {
      wdg->setValue( value );
      return;
    }
  }
}

}//end namespace dialog

}//end namespace gui
