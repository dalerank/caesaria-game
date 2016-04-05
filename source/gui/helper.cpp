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
// Copyright 2012-2016 Dalerank, dalerankn8@gmail.com

#include "helper.hpp"
#include "sound/engine.hpp"
#include "gfx/engine.hpp"
#include "walker/walker.hpp"
#include "widget_factory.hpp"

using namespace audio;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(SoundMuter)
REGISTER_CLASS_IN_WIDGETFACTORY(SoundEmitter)
REGISTER_CLASS_IN_WIDGETFACTORY(WalkerImage)

SoundMuter::SoundMuter(Widget* parent, int value)
  : Widget(parent,-1,Rect())
{
  if (value>=0)
    setVolume(value);
}

void SoundMuter::setVolume(int value)
{
  Engine& ae = Engine::instance();
  if (_states.empty())
  {
    _states[ambient] = ae.volume(ambient);
    _states[theme] = ae.volume(theme);
  }

  ae.setVolume(audio::ambient, value);
  ae.setVolume(audio::theme, value);
}

SoundMuter::~SoundMuter()
{
  Engine& ae = Engine::instance();
  for (const auto& it : _states)
    ae.setVolume( (SoundType)it.first, it.second );
}

SoundEmitter::SoundEmitter(Widget* parent, const std::string& sound, int volume, int type)
  : Widget(parent, -1, Rect())
{
  assign(sound, volume, type);
}

SoundEmitter::~SoundEmitter()
{
  if (!_sample.empty())
    Engine::instance().stop(_sample);
}

void SoundEmitter::assign(const std::string& sampleName, int volume, int type)
{
  if (!_sample.empty())
  {
    Engine::instance().stop(_sample);
  }

  if (!sampleName.empty())
  {
    _sample = sampleName;
    Engine::instance().play(_sample, volume, (SoundType)type);
  }
}

WalkerImage::WalkerImage(Widget* parent, Rect rectangle, SmartPtr<Walker> wlk)
  : Label(parent, rectangle, "", false, Label::bgBlackFrame)
{
  _wlkPicture = gfx::Picture(rectangle.size() - Size(6, 6), 0, true);
  _wlkPicture.fill(ColorList::clear, Rect());
  _walker = wlk;
}

void WalkerImage::_handleClick()
{
  emit _onClickedSignal(_walker);
}

void WalkerImage::draw(gfx::Engine &painter)
{
  if (!visible())
    return;

  Label::draw(painter);

  if (_wlkPicture.isValid())
  {
    Rect clipRect = absoluteClippingRect();
    clipRect._lefttop += Point(3, 3);
    clipRect._bottomright -= Point(3, 3);
    painter.draw(_wlkPicture, absoluteRect().lefttop() + Point(3, 3), &clipRect);
    gfx::Pictures pics;
    _walker->getPictures(pics);
    painter.draw(pics, absoluteRect().lefttop() + Point(30, 30), &clipRect);
  }
}

}//end namespace gui
