#include "console.hpp"
#include "console_commands.hpp"
#include "environment.hpp"
#include "core/saveadapter.hpp"
#include "gfx/engine.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "core/event.hpp"

namespace gui
{

//! constructor
Console::Console( Widget* parent, int id, const Rect& rectangle )
      : Widget( parent, id, rectangle ),
							 consoleHistoryIndex_(0),
               toggle_visible_(NONE)
{
  calculateConsoleRect( ui()->rootWidget()->size() );										//calculate the console rectangle

  resizeMessages();														//resize message array

  registerDefaultCommands_();

  hide();
	cursorPos_ = 1;

  _font = Font::create( FONT_1_WHITE );

   appendMessage( "NrpConsole initialized" );								//append a message
}

void Console::SaveCommands_()																	//
{					
	//   
  vfs::Path path( ":/commands.model" );					//

  VariantMap commands;

  foreach( it, console_history_ )							//
  {
    commands[ *it ] = "";
  }

  config::save( commands, path );
}

void Console::LoadSaveCommands_()													//
{
  vfs::Path path( ":/commands.model" );					//

  VariantMap commands = config::load( path );
  foreach( it, commands )
	{
    console_history_.push_back( it->first );
	}	
}

Console::~Console()													//! destructor
{
	SaveCommands_();
}

void Console::registerDefaultCommands_()			//! loads a few default commands into the console
{
	LoadSaveCommands_();
	
  RegisterCommand( new IC_Command_ECHO() );
  RegisterCommand( new IC_Command_HELP() );
  RegisterCommand( new IC_Command_LIST() );
  RegisterCommand( new IC_Command_CLS() );
  RegisterCommand( new IC_Command_SCRIPT() );
}

void Console::resizeMessages()											//! resize the message count
{
  unsigned int maxLines = 0;
  unsigned int lineHeight = 0;
  int fontHeight = 0;
  if( calculateLimits(maxLines,lineHeight,fontHeight) )						//
	{
    unsigned int messageCount = console_messages_.size();
		if(messageCount > maxLines)											// 
      console_messages_.erase( console_messages_.begin(), console_messages_.begin() + messageCount - maxLines );
	}
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

void Console::appendMessage( const std::string& message )					//
{
  console_messages_.push_back( message );								//
}

void Console::clearMessages()											//! clear all the messages in the sink
{
	console_messages_.clear();
}

void Console::draw( gfx::Engine& painter )
{
  resizeMessages();

    if( !_font.isValid() )
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
        //if( getOpacity() > 5 ) setOpacity( getOpacity() - 3 );
        //else _isVisible = false;
			}
			else
			{
        //if( (int)getOpacity() < (int)conf[ MAX_BLEND ] )	setOpacity( getOpacity() + 3 );
        //else toggle_visible_ = NONE;
			}
		}

    painter.draw( *_bgpic, absoluteRect().lefttop() );	//draw the bg as per configured color
		
    Rect textRect,shellRect;										//we calculate where the message log shall be printed and where the prompt shall be printed
    calculatePrintRects(textRect,shellRect);

    unsigned int maxLines, lineHeight;											//now, render the messages
    int fontHeight=0;													//
    if(!calculateLimits(maxLines,lineHeight,fontHeight))
		{
			return;
		}
		
    Rect lineRect( textRect.UpperLeftCorner.x(),						//calculate the line rectangle
                textRect.UpperLeftCorner.y(),
                textRect.LowerRightCorner.x(),
                textRect.UpperLeftCorner.y() + lineHeight);

    NColor fontcolor = DefaultColors::white;

  /* for(unsigned int i = 0; i < console_messages_.size(); i++)
   {
     _font.Draw( console_messages_[i].c_str(),
          lineRect,
          fontcolor,
          false, true,
          &getAbsoluteClippingRectRef() );									//we draw each line with the configured font and color vertically centered in the rectangle

     lineRect.UpperLeftCorner.Y += lineHeight;						//update line rectangle
     lineRect.LowerRightCorner.Y += lineHeight;
   }

   std::string shellText = conf[ PROMT ];	//now, render the prompt
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
    }	*/
  }

  Widget::draw( painter );
}

void Console::resolveCommand_()											//  Enter
{
  addToHistory( currentCommand_ );											//
  handleCommandString( currentCommand_ );									//
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

void Console::inputChar_( unsigned int key_char, bool shift_down )		//
{
	if(key_char)
	{
    char buf[2];
		buf[0] = key_char;
		buf[1] = 0;
    std::string astr = buf;
		
    //if(shift_down)
    //  astr = utils::localUpper( astr );
		
    currentCommand_ = currentCommand_.substr( 0, cursorPos_-1 ) + astr + currentCommand_.substr( cursorPos_-1, 0xff );
		cursorPos_++;
	}
}

void Console::keyPress( const NEvent& event )							//
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
							}//  
							break;
				case KEY_DELETE:
				  			if( cursorPos_ <= currentCommand_.size() )
							{
								 currentCommand_.erase( cursorPos_-1 );
							}
							break;
				case KEY_UP:									//  
              setPrevCommand_();
							break;
				case KEY_LEFT:
				case KEY_RIGHT:
              moveCursor_( event.keyboard.key == KEY_LEFT );
							break;
				case KEY_DOWN:
              setNextCommand_();
							break;
				case KEY_TAB:									// 
              tabComplete();
							break;
				default:
              inputChar_( event.keyboard.symbol, event.keyboard.shift );
						    break;
			}
    }
}

void Console::handleCommandString( const std::string& wstr)						//! handle the current command string
{
	if(wstr.size() > 0 )							//check if it is a command
	{
    std::string cmdLine = wstr;

		//append the message
    std::string msg = ">> Command : ";
		msg += cmdLine;
		AppendMessage( msg );

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
	for( size_t cnt=0; cnt < console_history_.size(); cnt++ )						//     
		if( console_history_[ cnt ] == wstr )										//    
			return;
 
    if( (int)console_history_.size() >= 20 )
      console_history_.erase( console_history_.begin() );

	console_history_.push_back( wstr.c_str() );
}

void Console::calculateConsoleRect(const Size& screenSize)	//! calculate the whole console rect
{
  Rect console_rect;

  Size consoleDim = ui()->rootWidget()->size();									//calculate console dimension

  consoleDim.setWidth( consoleDim.width()  * 0.9 );
  consoleDim.setHeight( consoleDim.height() * 0.3 );

  setGeometry( console_rect );
}

void Console::calculatePrintRects( Rect& textRect, Rect& shellRect)  //! calculate the messages rect and prompt / shell rect
{
  unsigned int maxLines, lineHeight;
  int fontHeight;

  if( calculateLimits(maxLines,lineHeight,fontHeight) )
	{
    shellRect = absoluteRect();
    shellRect.UpperLeftCorner.ry() = shellRect.LowerRightCorner.y() - lineHeight;

    textRect = absoluteRect();
    textRect.LowerRightCorner.ry() = textRect.UpperLeftCorner.y() + lineHeight;
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

  if( _font.isValid() && consoleHeight > 0)
	{
    fontHeight = _font.getTextSize("X").height() + 2;
    lineHeight = fontHeight + 5;
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

void Console::tabComplete()											//
{
	if(currentCommand_.size() == 0)
	{
		return;
	}

  std::string ccStr = currentCommand_.substr(1,currentCommand_.size() - 1); //
	
  StringArray names;						//

	GetRegisteredCommands( names );

  StringArray commands_find;				//

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
      if(thisCmd.substr(0,ccStr.size()) == ccStr) //
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
