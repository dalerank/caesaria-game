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
  GameAutoPause locker;
  bool initialized;

  void resolveChange( SpinBox* who, int value )
  {
    int type = who->getProperty( "soundType" );
    emit onSoundChangeSignal( audio::SoundType(type), value );
  }

public signals:
  Signal2<audio::SoundType, int> onSoundChangeSignal;
  Signal0<> onCloseSignal, onApplySignal, onRequestSignal;
};

SoundOptions::SoundOptions(Widget* parent)
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->locker.activate();
  _d->initialized = false;
  setupUI( ":/gui/soundoptions.gui" );

  setCenter( parent->center() );

  WidgetEscapeCloser::insertTo( this );

  INIT_WIDGET_FROM_UI( PushButton*, btnOk )
  if( btnOk ) btnOk->setFocus();

  List<SpinBox*> widgets = findChildren<SpinBox*>( true );
  foreach( it, widgets )
    CONNECT( *it, onChangeA(), _d.data(), Impl::resolveChange )
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
      emit _d->onApplySignal();
      deleteLater();
    break;

    case 1002:
    {
      emit _d->onCloseSignal();
      deleteLater();
    }
    break;

    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal2<audio::SoundType, int>& SoundOptions::onChange() { return _d->onSoundChangeSignal; }
Signal0<>& SoundOptions::onClose()                       { return _d->onCloseSignal; }
Signal0<>& SoundOptions::onApply()                       { return _d->onApplySignal; }

void SoundOptions::update(audio::SoundType type, int value)
{
  List<SpinBox*> widgets = findChildren<SpinBox*>( true );

  foreach( it, widgets )
  {
    if( (*it)->getProperty( "soundType" ).toInt() == type )
    {
      (*it)->setValue( value );
      return;
    }
  }
}

}//end namespace dialog

}//end namespace gui
