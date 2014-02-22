#ifndef _NRP_CONSOLE_H_
#define _NRP_CONSOLE_H_
/*
#include "command_dispatcher.hpp"
#include "NrpMessageSink.h"
#include "NrpConsoleUtils.h"
#include <NrpWidget.h>

NERPA_MODULE_BEGIN(gui)

class NrpConsole : public Widget, public core::CommandDispatcher, public core::MessageSink
{
public:
    NrpConsole( Widget* parent, s32 id, const core::RectI& rectangle );															//! constructor
    virtual ~NrpConsole();													//! destructor

    void setVisible( bool vis );
    void ToggleVisible();

    void onPaint( Painter* painter );

    void AppendMessage( const String& message );						//   
    void ClearMessages();													// 
    String getTypeName() const;

    int InitKey() const;

    void KeyPress( const NEvent& event );								//     

    virtual void styleChanged() {}

private:
    void HandleCommandString( const core::String& wstr);							// 
    void AddToHistory( const core::String& line);								//   
    void CalculateConsoleRect(const core::NSizeU& screenSize); //   
    void CalculatePrintRects(core::RectI& textRect, core::RectI& shellRect);
    bool CalculateLimits(u32& maxLines, u32& lineHeight,s32& fontHeight);	//  
    void ResizeMessages();
    void TabComplete();														// 
    void SetNextCommand_();
    void SetPrevCommand_();
    void ResolveCommand_();
    void InputChar_( wchar_t key_char, bool shift_down );
    void RegisterDefaultCommands_();					// 
    void MoveCursor_( bool leftStep );

    typedef core::Array< String > _STRINGS;

    _STRINGS console_messages_;								//   
    _STRINGS console_history_;								// 
    u32 consoleHistoryIndex_;
    Font _font;

    typedef enum { NONE=0, UPLIGTH, DOWNLIGTH } TOGGLE_TYPE;
    TOGGLE_TYPE toggle_visible_;

    core::String currentCommand_;												// 
    u32 cursorPos_;

    void SaveCommands_();
    void LoadSaveCommands_();
};

NERPA_MODULE_END(gui)

*/
#endif
