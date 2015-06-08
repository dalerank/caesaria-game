#ifndef _CAESARIA_GUI_CONSOLE_H_INCLUDE_
#define _CAESARIA_GUI_CONSOLE_H_INCLUDE_

#include "console_dispatcher.hpp"
#include "message_sink.hpp"
#include "widget.hpp"
#include "gfx/picture.hpp"

namespace gui
{

class Console : public Widget , public CommandDispatcher, public MessageSink
{
public:
    Console( Widget* parent, int id, const Rect& rectangle );															//! constructor
    virtual ~Console();													//! destructor

    void setVisible( bool vis );
    void toggleVisible();

    void draw( gfx::Engine& painter );

    void appendMessage( const std::string& message );						//
    void clearMessages();													//

    int initKey() const;

    void keyPress( const NEvent& event );								//

private:
    void handleCommandString( const std::string& wstr);							//
    void addToHistory( const std::string& line);								//
    void calculateConsoleRect(const Size& screenSize); //
    void calculatePrintRects( Rect& textRect, Rect& shellRect);
    bool calculateLimits(unsigned int& maxLines, unsigned int& lineHeight, int& fontHeight);	//
    void resizeMessages();
    void tabComplete();														//
    void setNextCommand_();
    void setPrevCommand_();
    void resolveCommand_();
    void inputChar_(unsigned int key_char, bool shift_down );
    void registerDefaultCommands_();					//
    void moveCursor_( bool leftStep );

    StringArray console_messages_;								//
    StringArray console_history_;								//
    unsigned int consoleHistoryIndex_;
    Font _font;

    typedef enum { NONE=0, UPLIGTH, DOWNLIGTH } TOGGLE_TYPE;
    TOGGLE_TYPE toggle_visible_;

    std::string currentCommand_;												//
    gfx::Picture _bgpic;
    unsigned int cursorPos_;

    void SaveCommands_();
    void LoadSaveCommands_();
};

}

#endif //_CAESARIA_GUI_CONSOLE_H_INCLUDE_
