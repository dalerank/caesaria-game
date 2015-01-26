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

#include "event_messagebox.hpp"
#include "label.hpp"
#include "good/goodhelper.hpp"
#include "game/datetimehelper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

EventMessageBox::EventMessageBox(Widget* parent, const std::string& title,
                                  const std::string& message, DateTime time, good::Product gtype, const std::string& additional)
  : Simple( parent, Rect( 0, 0, 480, 320 ), Rect( 18, 40, 480 - 18, 320 - 50 ) )
{
  setTitle( title );
  setAutoPosition( false );

  setCenter( parent->center() );

  _lbTextRef()->setGeometry( Rect( 25, 45, width() - 25, height() - 55 ) );
  _lbTextRef()->setWordwrap( true );
  _lbTextRef()->setText( message );

  Rect rect = _lbTextRef()->relativeRect();
  rect.LowerRightCorner = Point( rect.width() / 2, rect.top() + 30 );

  Label* lbTime = new Label( this, rect, util::date2str( time ) );
  lbTime->setFont( Font::create( FONT_2_WHITE ) );

  if( !additional.empty() )
  {
    Label* lbAdditional = new Label( this, rect + Point( rect.width() / 2, 0 ), additional );
    lbAdditional->setTextAlignment( align::upperLeft, align::center );
    lbAdditional->setTextOffset( Point( 30, 0 ) );
  }

  _lbTextRef()->setTop( _lbTextRef()->top() + 30 );

  if( gtype != good::none )
  {
    Rect rect = _lbTextRef()->relativeRect();
    _lbTextRef()->move( Point( 0, 30 ) );

    rect.LowerRightCorner.setY( rect.top() + 30 );
    rect.UpperLeftCorner.setX( rect.left() + 40 );
    Label* goodLabel = new Label( this, rect, good::Helper::getTypeName( gtype ) );
    goodLabel->setTextAlignment( align::upperLeft, align::center );
    goodLabel->setTextOffset( Point( 30, 0 ) );
    goodLabel->setIcon( good::Helper::picture( gtype ), Point( 0, 7 ) );
  }    
}

EventMessageBox::~EventMessageBox() {}

}//end namespace gui
