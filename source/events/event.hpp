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

#ifndef _CAESARIA_SCENARIO_EVENT_H_INCLUDE_
#define _CAESARIA_SCENARIO_EVENT_H_INCLUDE_

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "core/position.hpp"
#include "objects/building.hpp"
#include "predefinitions.hpp"

class Game;

namespace events
{

class GameEvent : public ReferenceCounted
{
public:
  virtual bool isDeleted() const;
  virtual bool tryExec( Game& game, unsigned int time );

  void dispatch();

  virtual VariantMap save() const;
  virtual void load( const VariantMap& );

protected:
  virtual void _exec( Game& game, unsigned int time ) = 0;
  virtual bool _mayExec( Game& game, unsigned int time ) const = 0;

  GameEvent() {}
};

class ClearLandEvent : public GameEvent
{
public:
  static GameEventPtr create( const TilePos& );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;
private:
  TilePos _pos;
};

class ShowInfoboxEvent : public GameEvent
{
public:
  static GameEventPtr create( const std::string& title, const std::string& text );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  std::string _title, _text;
};

class WarningMessageEvent : public GameEvent
{
public:
  static GameEventPtr create( const std::string& text );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;
private:
  std::string _text;
};

class ShowEmpireMapWindow : public GameEvent
{
public:
  static GameEventPtr create( bool show );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  bool _show;
};

class ShowAdvisorWindow : public GameEvent
{
public:
  static GameEventPtr create( bool show, int advisor );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  bool _show;
  int _advisor;
};

class Pause : public GameEvent
{
public:
  typedef enum { toggle, pause, play, hidepause, hideplay } Mode;
  static GameEventPtr create( Mode mode );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  Mode _mode;
};

class ChangeSpeed : public GameEvent
{
public:
  static GameEventPtr create( int value );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec( Game& game, unsigned int ) const;

private:
  int _value;
};

} //end namespace events
#endif //_CAESARIA_SCENARIO_EVENT_H_INCLUDE_
