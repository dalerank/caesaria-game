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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "console.hpp"
#include "console_commands.hpp"
#include "environment.hpp"
#include <GameCore>
#include <GameVfs>
#include <GameCore>
#include <GameLogger>
#include <GameGfx>

using namespace gfx;

namespace gui
{

class Console::Impl
{
public:
  Picture bg;
  bool dirty = true;
  Size commandTextSize;
  Font font;
  unsigned int curIndex = 0;

  int commandCursorWidth;
};

class ConsoleLogger : public LogWriter
{
public:
  Console* console = nullptr;

  virtual void write(const std::string& message, bool newLine)
  {
    console->appendMessage(message);
  }

  virtual bool isActive() const { return true; }
};

//! constructor
Console::Console( Widget* parent, int id, const Rect& rectangle )
      : Widget( parent, id, rectangle ),
        consoleHistoryIndex_(0),
        toggle_visible_(NONE), _d(new Impl)
{
  calculateConsoleRect(ui()->rootWidget()->size());										//calculate the console rectangle

  hide();
  cursorPos_ = 1;

  setFont( Font::create("default",12).withColor(ColorList::yellow) );
  _opacity = 0;
  _logger = SmartPtr<ConsoleLogger>(new ConsoleLogger());
  _logger->drop();
  _logger->console = this;
  _d->bg = Picture(size(),0,true);

  Logger::registerWriter(TEXT(ConsoleLogger), _logger.object() );
  Widget::setVisible(false);

  resizeMessages();
  registerDefaultCommands_();

  appendMessage( "Console initialized" );								//append a message
  _updateCommandRect();
}

void Console::SaveCommands_()
{
  vfs::Path path( ":/commands.model" );

  VariantMap commands;
  for( const auto& it : console_history_ )
    commands[it] = Variant("");

  config::save( commands, path );
}

Font Console::font() const
{
  return _d->font;
}

void Console::LoadSaveCommands_()
{
  vfs::Path path( ":/commands.model" );

  VariantMap commands = config::load( path );
  for( const auto& it : commands )
    console_history_.push_back( it.first );
}

Console::~Console()													//! destructor
{
  SaveCommands_();
}

void Console::registerDefaultCommands_()			//! loads a few default commands into the console
{
  LoadSaveCommands_();

  RegisterCommand(new IC_Command_ECHO() );
  RegisterCommand(new IC_Command_HELP() );
  RegisterCommand(new IC_Command_LIST() );
  RegisterCommand(new IC_Command_CLS() );
  RegisterCommand(new IC_Command_SCRIPT() );
}

void Console::setFont(const Font& font)
{
  _d->font = font;
  resizeMessages();
}

void Console::resizeMessages()											//! resize the message count
{
  unsigned int lineHeight = 0;
  int fontHeight = 0;
  unsigned int maxLines;
  bool isOk = calculateLimits(maxLines, lineHeight, fontHeight);
  console_messages_.resize(isOk ? maxLines : 10);
}

void Console::toggleVisible()											//! toggle the visibility of the console
{
   setVisible( (toggle_visible_ == NONE && visible())
                ? false
                : toggle_visible_ != UPLIGTH );
}

void Console::setVisible( bool vis )											//! toggle the visibility of the console
{
  toggle_visible_ = vis ? UPLIGTH : DOWNLIGTH;
  Widget::setVisible( true );
}

void Console::appendMessage( const std::string& message )
{
  auto messages = utils::split(message, "\n");
  for (const auto& i : messages)
  {
    _d->curIndex = (_d->curIndex + 1) % console_messages_.size();
    console_messages_[_d->curIndex] = i;
  }
  _d->dirty = true;
}

void Console::clearMessages()											//! clear all the messages in the sink
{
  console_messages_.clear();
  _d->dirty = true;
}

void Console::draw( gfx::Engine& painter )
{
  if( !font().isValid() )
  {
    Widget::draw( painter );
    return;
  }

  if( visible() )															// render only if the console is visible
  {
    if( toggle_visible_ != NONE )
    {
      if( toggle_visible_ == DOWNLIGTH )
      {
        if (_opacity > 5) _opacity -= 9;
        else setVisible(false);
        _d->dirty = true;
      }
      else
      {
        if (_opacity < 0xff)	_opacity += 3;
        else toggle_visible_ = NONE;
        _d->dirty = true;
      }
    }

    Rect textRect, shellRect;										//we calculate where the message log shall be printed and where the prompt shall be printed
    calculatePrintRects(textRect,shellRect);

    if(_d->dirty)
    {
      Decorator::drawLines(_d->bg, ColorList::red, relativeRect().lines());
      _d->bg.fill(ColorList::blue);

      unsigned int maxLines, lineHeight;											//now, render the messages
      int fontHeight=0;
      if (!calculateLimits(maxLines,lineHeight,fontHeight))
      {
        return;
      }

      Rect lineRect( textRect.left(),						//calculate the line rectangle
                     textRect.top(),
                     textRect.right(),
                     textRect.bottom() + lineHeight);

      for (unsigned int index = 0; index < console_messages_.size(); index++)
      {
        unsigned int rindex = (_d->curIndex + index) % console_messages_.size();
        const std::string& line = console_messages_[rindex];
        font().draw(_d->bg, line, lineRect.lefttop(), false, false);
        lineRect += Point(0, lineHeight);						//update line rectangle
      }

      std::string shellText = "$>" + currentCommand_;

      font().draw( _d->bg, shellText, shellRect.lefttop(), false, false);	//draw the prompt string

      _d->dirty = false;
      _d->bg.update();
      _d->bg.setAlpha(_opacity/3*2);
    }

    painter.draw( _d->bg, absoluteRect().lefttop() );

    if( DateTime::elapsedTime() % 700 < 350 )
    {
      NColor color = ColorList::white;
      color.setAlpha(_opacity/2);
      painter.fillRect( color, Rect(0, 0, _d->commandCursorWidth,shellRect.height()*0.8)
                        +absoluteRect().leftbottom() + Point(_d->commandTextSize.width(), -shellRect.height()) );
    }
  }

  Widget::draw( painter );
}

void Console::resolveCommand_()											//  Enter
{
  addToHistory( currentCommand_ );
  handleCommandString( currentCommand_ );
  currentCommand_ = "";
  consoleHistoryIndex_ = 0;
  cursorPos_ = 1;
}

void Console::setPrevCommand_()
{
  if(console_history_.size() > 0)
  {
    int index = console_history_.size() - 1 - consoleHistoryIndex_;
    if(index >= 0 && index < int( console_history_.size() ) )
    {
      consoleHistoryIndex_++;
      currentCommand_ = console_history_[index].c_str();
      cursorPos_ = currentCommand_.size() + 1;
    }
    else
    {
      consoleHistoryIndex_ = 0;
    }
  }
  else
  {
    consoleHistoryIndex_ = 0;
  }
}

void Console::setNextCommand_()
{
  if(console_history_.size() > 0)
  {

    int index = console_history_.size() - consoleHistoryIndex_;
    if(index >= 0 && index < static_cast< int >( console_history_.size() ) )
    {
      consoleHistoryIndex_--;
      currentCommand_ = console_history_[index].c_str();
      cursorPos_ = currentCommand_.size() + 1;
    }
    else
    {
      consoleHistoryIndex_ = console_history_.size() - 1;
    }
  }
  else
  {
    consoleHistoryIndex_ = 0;
  }
}

void Console::inputChar_( unsigned int key_char, bool shift_down )
{
  if (key_char)
  {
    char buf[2];
    buf[0] = key_char;
    buf[1] = 0;
    std::string astr = buf;

    currentCommand_ = currentCommand_.substr( 0, cursorPos_-1 ) + astr + currentCommand_.substr( cursorPos_-1, 0xff );
    cursorPos_++;
  }

  _updateCommandRect();
}

void Console::_updateCommandRect()
{
  if (font().isValid())
  {
    _d->commandTextSize = font().getTextSize(("$>" + currentCommand_).substr(0, 2 + cursorPos_ - 1));
    _d->commandCursorWidth = font().getWidthFromCharacter('X');
    _d->dirty = true;
  }
}

void Console::keyPress( const NEvent& event )
{
  if( event.keyboard.key == KEY_RETURN )
  {
    if( currentCommand_.size() > 0 )
      resolveCommand_();
  }
  else if( event.keyboard.pressed )
  {
    switch ( event.keyboard.key )
    {
      case KEY_BACK:
        if( currentCommand_.size() > 0 && cursorPos_ > 1 )
        {
          cursorPos_--;
          currentCommand_.erase( cursorPos_-1 );
        }
      break;
      case KEY_DELETE:
            if( cursorPos_ <= currentCommand_.size() )
               currentCommand_.erase( cursorPos_-1 );
      break;
      case KEY_UP: setPrevCommand_(); break;

      case KEY_LEFT:
      case KEY_RIGHT: moveCursor_( event.keyboard.key == KEY_LEFT ); break;

      case KEY_DOWN: setNextCommand_(); break;
      case KEY_TAB: tabComplete(); break;
      default: inputChar_( event.keyboard.symbol, event.keyboard.shift ); break;
    }
  }

  _updateCommandRect();
}

void Console::handleCommandString( const std::string& wstr)						//! handle the current command string
{
  if(wstr.size() > 0 )							//check if it is a command
  {
    std::string cmdLine = wstr;

    //append the message
    std::string msg = ">> Command : ";
    msg += cmdLine;
    appendMessage( msg );

    //parsing logic

    StringArray args;
    std::string cmdName;
    ConsoleParser parser(cmdLine);
    if( parser.parse(cmdName,args) )
    {
      Dispatch(cmdName,args,this);
    }
    else
    {
      std::string errorMessage = "The command syntax is incorrect or it could not be parsed";
      LogError(errorMessage);
    }
  }
  else
  {

  }
}

void Console::addToHistory( const std::string& wstr)								//! add to history and readjust history
{
  for( size_t cnt=0; cnt < console_history_.size(); cnt++ )
    if( console_history_[ cnt ] == wstr )
      return;

    if( (int)console_history_.size() >= 20 )
      console_history_.erase( console_history_.begin() );

  console_history_.push_back( wstr.c_str() );
}

void Console::calculateConsoleRect( const Size& screenSize )	//! calculate the whole console rect
{
  setGeometry( RectF(0.1f,0.f,0.9f,0.3f) );
}

void Console::calculatePrintRects( Rect& textRect, Rect& shellRect)  //! calculate the messages rect and prompt / shell rect
{
  unsigned int maxLines, lineHeight;
  int fontHeight;

  if (calculateLimits(maxLines,lineHeight,fontHeight) )
  {
    shellRect = Rect(Point(), size());
    shellRect.setTop(shellRect.bottom() - lineHeight);

    textRect = Rect(Point(), size());
    textRect.setBottom(textRect.top() + lineHeight );
  }
  else
  {
    textRect = Rect(0,0,0,0);
    shellRect = Rect(0,0,0,0);
  }
}

bool Console::calculateLimits(unsigned int& maxLines, unsigned int& lineHeight,int& fontHeight)
{
  unsigned int consoleHeight = height();

  if( font().isValid() && consoleHeight > 0)
  {
    fontHeight = font().getTextSize("X").height() + 2;
    lineHeight = fontHeight + 2;
    maxLines = consoleHeight / lineHeight;
    if(maxLines > 2)
    {
      maxLines -= 2;
    }
    return true;
  }
  else
  {
    return false;
  }
}

void Console::tabComplete()
{
  if(currentCommand_.size() == 0)
  {
    return;
  }

  std::string ccStr = currentCommand_.substr(1,currentCommand_.size() - 1);

  StringArray names;

  GetRegisteredCommands( names );

  StringArray commands_find;

  for(unsigned int i = 0; i < names.size(); i++)
  {
    std::string thisCmd = names[i];
    if(thisCmd.size() == ccStr.size())
    {
      if(thisCmd == ccStr)
      {
        return;
      }
    }
    else if(thisCmd.size() > ccStr.size())
    {
      if(thisCmd.substr(0,ccStr.size()) == ccStr)
      {
        commands_find.push_back( thisCmd );
      }
    }
  }

  if( commands_find.size() == 1 )
  {
    currentCommand_ = commands_find[ 0 ];
    return;
  }
  else
  {
    for( size_t cnt=0; cnt < commands_find.size(); cnt++ )
      appendMessage( commands_find[ cnt ] );
  }
}

void Console::moveCursor_( bool leftStep )
{
  if( leftStep )
    cursorPos_ -= (cursorPos_ > 0 && currentCommand_.size() ) & 1;
  else
    cursorPos_ += (cursorPos_ < currentCommand_.size() + 1) & 1;
}

int Console::initKey() const
{
    return '`';
}

}
