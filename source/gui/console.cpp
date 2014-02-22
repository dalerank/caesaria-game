#include "console.hpp"
#include "console_commands.hpp"
/*
NERPA_MODULE_BEGIN(gui)

//! constructor
NrpConsole::NrpConsole( Widget* parent, s32 id, const core::RectI& rectangle )
			: Widget( NRPGUI_CONSOLE, parent, id, rectangle ),
							 consoleHistoryIndex_(0),
                                                         _font( NULL ), toggle_visible_(NONE)
{
    setInternalName( ConsoleConfig::Instance()[ GUINAME ].As<String>() );

	CalculateConsoleRect( getEnvironment()->getScreenSize() );										//calculate the console rectangle

	ResizeMessages();														//resize message array

	RegisterDefaultCommands_();

	_isVisible = false;
	setOpacity( 3 );
	cursorPos_ = 1;

    ConsoleConfig& conf = ConsoleConfig::Instance();
    _font = Font( conf[ FONTNAME ].As<String>() );

    if( !_font.available() )
        _font = Font( getStyle().getName() );

    AppendMessage( String( L"NrpConsole initialized" ) );								//append a message
}

void NrpConsole::SaveCommands_()																	//  
{					
	//   
    String path = ConsoleConfig::Instance()[ COMMANDS_FILE ];							//  		

	try
	{
        io::IniFile sv( path );
		sv.Set( L"commands", L"number_commands", (int)console_history_.size() );	//   

		for( u32 cnt=0; cnt < console_history_.size(); cnt++ )							//   
		{
            sv.Set( L"commands", String( L"type_") + String::fromInt( cnt ), console_history_[ cnt ] ); 
		}	

		sv.Save();
	}
	catch(...)
	{
#ifdef _DEBUG
        core::Log( core::APP ) << "Can't write command to " << path << core::MsgTerminator();
#endif
	}
}

void NrpConsole::LoadSaveCommands_()													//  
{
    String path = ConsoleConfig::Instance()[ COMMANDS_FILE ];

    io::IniFile sv( path );
	int num_com = sv.Get( L"commands", L"number_commands", (int)0 );

	for( int cnt=0; cnt < num_com; cnt++ )
	{
        String strk = sv.Get( L"commands", String( L"type_" ) + String::fromInt( cnt ), String( L"null" ) );								//       "null"

		console_history_.push_back( strk );
	}	
}

NrpConsole::~NrpConsole()													//! destructor
{
	SaveCommands_();
}

void NrpConsole::RegisterDefaultCommands_()			//! loads a few default commands into the console
{
	LoadSaveCommands_();
	
	RegisterCommand( new core::IC_Command_ECHO() );
	RegisterCommand( new core::IC_Command_HELP() );
	RegisterCommand( new core::IC_Command_LIST() );
	RegisterCommand( new core::IC_Command_CLS() );
	RegisterCommand( new core::IC_Command_SCRIPT() );
}

void NrpConsole::ResizeMessages()											//! resize the message count
{
	u32 maxLines = 0;
	u32 lineHeight = 0;
	s32 fontHeight = 0;
	if( CalculateLimits(maxLines,lineHeight,fontHeight) )						//     
	{
		u32 messageCount = console_messages_.size();	
		if(messageCount > maxLines)											// 
			console_messages_.erase( 0, messageCount - maxLines );
	}
}

void NrpConsole::ToggleVisible()											//! toggle the visibility of the console
{
   setVisible( (toggle_visible_ == NONE && isVisible()) 
                ? false 
                : toggle_visible_ != UPLIGTH );
}

void NrpConsole::setVisible( bool vis )											//! toggle the visibility of the console
{
    toggle_visible_ = vis ? UPLIGTH : DOWNLIGTH;
	_isVisible = true;
}

void NrpConsole::AppendMessage( const core::String& message )					//   
{
	//EnterCriticalSection( &cs_dataaccess_ );

	try
	{
//		console_messages_.push_back( message );								//     
	}
	catch(...)
	{
		//ErrLog(gfx) << all << "     " << term;
	}

	//LeaveCriticalSection( &cs_dataaccess_ );
}

void NrpConsole::ClearMessages()											//! clear all the messages in the sink
{
	console_messages_.clear();
}

void NrpConsole::onPaint( Painter* painter )
{
    ConsoleConfig& conf = ConsoleConfig::Instance();

    ResizeMessages();

    if( !_font.available() )
    {
        Widget::onPaint( painter );
        return;
    }

	if( isVisible() )															// render only if the console is visible
	{
		if( toggle_visible_ != NONE )
		{
			if( toggle_visible_ == DOWNLIGTH )
			{
				if( getOpacity() > 5 ) setOpacity( getOpacity() - 3 );
				else _isVisible = false;
			}
			else
			{
                if( (int)getOpacity() < (int)conf[ MAX_BLEND ] )	setOpacity( getOpacity() + 3 );
				else toggle_visible_ = NONE;
			}
		}

                 if( (bool)conf[ SHOW_BACKGROUND ] )											//if bg is to be drawn fill the console bg with color
		{
                        Color color = conf[ BG_COLOR ].As< Color >();
			color.setAlpha( u32( getOpacity() ) );
			painter->drawRectangle( color, getAbsoluteRect(), 0 );	//draw the bg as per configured color
		}
		
		core::RectI textRect,shellRect;										//we calculate where the message log shall be printed and where the prompt shall be printed
		CalculatePrintRects(textRect,shellRect);

		u32 maxLines, lineHeight;											//now, render the messages
		s32 fontHeight=0;													//  
		if(!CalculateLimits(maxLines,lineHeight,fontHeight))
		{
			return;
		}
		
		core::RectI lineRect( textRect.UpperLeftCorner.X,						//calculate the line rectangle
							  textRect.UpperLeftCorner.Y,
							  textRect.LowerRightCorner.X,
							  textRect.UpperLeftCorner.Y + lineHeight);

		//EnterCriticalSection( &cs_dataaccess_ );
        Color fontcolor = conf[ FONT_COLOR ].As<Color>();
		fontcolor.setAlpha( u32( getOpacity() ) );

		try
		{
			for(u32 i = 0; i < console_messages_.size(); i++)
			{
				_font.Draw( console_messages_[i].c_str(), 
					 		lineRect, 
							fontcolor, 
							false, true,
							&getAbsoluteClippingRectRef() );									//we draw each line with the configured font and color vertically centered in the rectangle

				lineRect.UpperLeftCorner.Y += lineHeight;						//update line rectangle
				lineRect.LowerRightCorner.Y += lineHeight;
			}
		}
		catch(...)
		{
			//ErrLog(gfx) << all << "   " << term;
		}

		//LeaveCriticalSection( &cs_dataaccess_);

        String shellText = conf[ PROMT ];	//now, render the prompt
		shellText.append( L"$>" );
		size_t textSize = shellText.size();
		shellText.append( currentCommand_ );
		
		_font.Draw(  shellText,
			          shellRect,
				      fontcolor,
					  false, false, 
					  &getAbsoluteClippingRectRef() );											//draw the prompt string

        if( ( DateTime::getElapsedTime() % 700 ) < 350 )
		{
			core::NSizeU pos = _font.getDimension( shellText.subString( 0, textSize + cursorPos_ - 1 ).c_str() );
			_font.Draw( L"_", core::RectI( pos.Width , 0, pos.Width + 20, pos.Height ) + shellRect.UpperLeftCorner,
						0xffff0000,
						false, false, &getAbsoluteClippingRectRef() );
		}		
	}

	Widget::onPaint( painter );
}

void NrpConsole::ResolveCommand_()											//  Enter
{
	AddToHistory( currentCommand_ );											//   
	HandleCommandString( currentCommand_ );									// 
	currentCommand_ = L"";
	consoleHistoryIndex_ = 0;
	cursorPos_ = 1;
}

void NrpConsole::SetPrevCommand_()
{
	if(console_history_.size() > 0)
	{
		s32 index = console_history_.size() - 1 - consoleHistoryIndex_;
		if(index >= 0 && index < s32( console_history_.size() ) )
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

void NrpConsole::SetNextCommand_()
{
	if(console_history_.size() > 0)
	{

		s32 index = console_history_.size() - consoleHistoryIndex_;
		if(index >= 0 && index < static_cast< s32 >( console_history_.size() ) )
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

void NrpConsole::InputChar_( wchar_t key_char, bool shift_down )		//  
{
	if(key_char)
	{
		wchar_t buf[2];
		buf[0] = key_char;
		buf[1] = 0;
		core::String astr = buf;
		
		if(shift_down)
			astr.make_upper();
		
		currentCommand_ = currentCommand_.subString( 0, cursorPos_-1 ) + astr + currentCommand_.subString( cursorPos_-1, 0xff );
		cursorPos_++;
	}
}

void NrpConsole::KeyPress( const NEvent& event )							//  
{
        if( event.KeyboardEvent.Key == KEY_RETURN )
	{
		if( currentCommand_.size() > 0 )
			ResolveCommand_(); 	
	}
	else if( event.KeyboardEvent.PressedDown )
			switch ( event.KeyboardEvent.Key )
			{
				case KEY_BACK:	
							if( currentCommand_.size() > 0 && cursorPos_ > 1 )
							{	
								cursorPos_--;
								currentCommand_.erase( cursorPos_-1 );
							}//  
							break;
				case KEY_DELETE:
				  			if( cursorPos_ <= currentCommand_.size() )
							{
								 currentCommand_.erase( cursorPos_-1 );
							}
							break;
				case KEY_UP:									//  
							SetPrevCommand_();
							break;
				case KEY_LEFT:
				case KEY_RIGHT:
                                                        MoveCursor_( event.KeyboardEvent.Key == KEY_LEFT );
							break;
				case KEY_DOWN:
							SetNextCommand_();
							break;
				case KEY_TAB:									// 
							TabComplete();
							break;
				default:
							InputChar_( event.KeyboardEvent.Char, event.KeyboardEvent.Shift );
						    break;
			}
}

void NrpConsole::HandleCommandString( const core::String& wstr)						//! handle the current command string
{
	if(wstr.size() > 0 )							//check if it is a command
	{
		core::String cmdLine = wstr;

		//append the message
		core::String msg = L">> Command : ";
		msg += cmdLine;
		AppendMessage( msg );

		//parsing logic
		
		core::Array< String > args;
		String cmdName;
        core::ConsoleParser parser(cmdLine);
		if( parser.parse(cmdName,args) )
		{		
			Dispatch(cmdName,args,this);
		}
		else
		{
			String errorMessage = L"The command syntax is incorrect or it could not be parsed";
			LogError(errorMessage);
		}
	}
	else
	{

	}
}

void NrpConsole::AddToHistory( const core::String& wstr)								//! add to history and readjust history
{
	for( size_t cnt=0; cnt < console_history_.size(); cnt++ )						//     
		if( console_history_[ cnt ] == wstr )										//    
			return;
 
    if( (int)console_history_.size() >= (int)ConsoleConfig::Instance()[ HISTORY_SIZE ] )
		console_history_.erase( 0 );

	console_history_.push_back( wstr.c_str() );
}

void NrpConsole::CalculateConsoleRect(const core::NSizeU& screenSize)	//! calculate the whole console rect
{
	core::RectI console_rect;
    ConsoleConfig& conf = ConsoleConfig::Instance();
    core::NSizeF scr_rat = conf[ RELATIVE_SIZE ].As< core::NSizeF >();

	if( scr_rat.Width == 0 || scr_rat.Height == 0)
	{
		console_rect = core::RectI(0,0,0,0);
	}
	else
	{
		core::NSizeU consoleDim = getEnvironment()->getScreenSize();									//calculate console dimension

		consoleDim.Width = (s32)(consoleDim.Width  * scr_rat.Width );
		consoleDim.Height= (s32)(consoleDim.Height * scr_rat.Height );

        String al_v = conf[ VERT_ALIGN ].As<String>();
        if( al_v == NrpAlignmentNames[ alignUpperLeft ] )									//set vertical alignment
		{
			console_rect.UpperLeftCorner.Y = 0;
		}
        else if( al_v== NrpAlignmentNames[ alignLowerRight ] )
		{
			console_rect.UpperLeftCorner.Y = screenSize.Height - consoleDim.Height;
		}
        else if( al_v == NrpAlignmentNames[ alignCenter ] )
		{
			console_rect.UpperLeftCorner.Y = (screenSize.Height - consoleDim.Height) / 2; 
		}
	
        String al_h = conf[ HORT_ALIGN ].As<String>();
        if( al_h == NrpAlignmentNames[ alignUpperLeft ] )								//set horizontal alignment
		{
			console_rect.UpperLeftCorner.X = 0;
		}
        else if( al_h == NrpAlignmentNames[ alignLowerRight ] )
		{
			console_rect.UpperLeftCorner.X = screenSize.Width - consoleDim.Width;
		}
        else if( al_h == NrpAlignmentNames[ alignCenter ] )
		{
			console_rect.UpperLeftCorner.X = (screenSize.Width - consoleDim.Width) / 2; 
		}
		
		console_rect.LowerRightCorner.X = console_rect.UpperLeftCorner.X + consoleDim.Width;	 		//set the lower right corner stuff
		console_rect.LowerRightCorner.Y = console_rect.UpperLeftCorner.Y + consoleDim.Height;

		setGeometry( console_rect );
	}
}

void NrpConsole::CalculatePrintRects( core::RectI& textRect, core::RectI& shellRect)  //! calculate the messages rect and prompt / shell rect
{
	u32 maxLines, lineHeight;
	s32 fontHeight;

	if( CalculateLimits(maxLines,lineHeight,fontHeight) )
	{
		shellRect = getAbsoluteRect();
		shellRect.UpperLeftCorner.Y = shellRect.LowerRightCorner.Y - lineHeight;

		textRect = getAbsoluteRect();
		textRect.LowerRightCorner.Y = textRect.UpperLeftCorner.Y + lineHeight;
	}
	else
	{
		textRect = core::RectI(0,0,0,0);
		shellRect = core::RectI(0,0,0,0);
	}
}

bool NrpConsole::CalculateLimits(u32& maxLines, u32& lineHeight,s32& fontHeight)
{
	u32 consoleHeight = getHeight();
    ConsoleConfig& conf = ConsoleConfig::Instance();

	if( _font.available() && consoleHeight > 0)
	{
		fontHeight = _font.getDimension(L"X").Height + 2;
        lineHeight = fontHeight + (int)conf[ LINE_SPACING ];
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

void NrpConsole::TabComplete()											//  
{
	if(currentCommand_.size() == 0)
	{
		return;
	}

	core::String ccStr = currentCommand_.subString(1,currentCommand_.size() - 1); //    
	
	core::Array< core::String > names;						//  

	GetRegisteredCommands( names );

	core::Array< core::String > commands_find;				//    

	for(u32 i = 0; i < names.size(); i++)
	{
		core::String thisCmd = names[i];
		if(thisCmd.size() == ccStr.size())				
		{
			if(thisCmd == ccStr)
			{
				return;
			}
		}
		else if(thisCmd.size() > ccStr.size())
		{
			if(thisCmd.subString(0,ccStr.size()) == ccStr) //   
			{												//      
				commands_find.push_back( thisCmd );
			}
		}
	}

	if( commands_find.size() == 1 )							//    
	{	
		currentCommand_ = commands_find[ 0 ];
		return;
	}
	else													//       
	{
		for( size_t cnt=0; cnt < commands_find.size(); cnt++ )
			AppendMessage( commands_find[ cnt ] );
	}
}

void NrpConsole::MoveCursor_( bool leftStep )
{
	if( leftStep )
		cursorPos_ -= (cursorPos_ > 0 && currentCommand_.size() ) & 1;
	else 
		cursorPos_ += (cursorPos_ < currentCommand_.size() + 1) & 1;
}

String NrpConsole::getTypeName() const
{
    return L"CNrpConsole";
}

int NrpConsole::InitKey() const
{
    return ConsoleConfig::Instance()[ INIT_KEY ];
}

NERPA_MODULE_END(gui)
*/
