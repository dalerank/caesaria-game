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

#ifndef _CAESARIA_GUI_CONSOLE_H_INCLUDE_
#define _CAESARIA_GUI_CONSOLE_H_INCLUDE_

#include "console_dispatcher.hpp"
#include "message_sink.hpp"
#include "widget.hpp"
#include "gfx/picture.hpp"

namespace gui
{

class ConsoleLogger;

class Console : public Widget, public CommandDispatcher, public MessageSink
{
public:
    Console(Widget* parent, int id, const Rect& rectangle);
    virtual ~Console();

    virtual void setVisible(bool vis);
    void toggleVisible();

    virtual void draw( gfx::Engine& painter );

    virtual void setFont(const Font& font);
    virtual Font font() const;

    void appendMessage(const std::string& message);
    void clearMessages();

    int initKey() const;

    void keyPress(const NEvent& event);

private:
    void _updateCommandRect();
    void handleCommandString( const std::string& wstr);
    void addToHistory( const std::string& line);
    void calculateConsoleRect(const Size& screenSize);
    void calculatePrintRects( Rect& textRect, Rect& shellRect);
    bool calculateLimits(unsigned int& maxLines, unsigned int& lineHeight, int& fontHeight);
    void resizeMessages();
    void tabComplete();
    void setNextCommand_();
    void setPrevCommand_();
    void resolveCommand_();
    void inputChar_(unsigned int key_char, bool shift_down );
    void registerDefaultCommands_();
    void moveCursor_( bool leftStep );

    StringArray console_messages_;
    StringArray console_history_;
    unsigned int consoleHistoryIndex_;

    typedef enum { NONE=0, UPLIGTH, DOWNLIGTH } TOGGLE_TYPE;
    TOGGLE_TYPE toggle_visible_;
    SmartPtr<ConsoleLogger> _logger;

    std::string currentCommand_;
    int _opacity;
    unsigned int cursorPos_;

    void SaveCommands_();
    void LoadSaveCommands_();

    class Impl;
    ScopedPtr<Impl> _d;
};

}

#endif //_CAESARIA_GUI_CONSOLE_H_INCLUDE_
