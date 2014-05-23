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

#include "infobox_raw.hpp"
#include "good/goodhelper.hpp"
#include "image.hpp"
#include "core/stringhelper.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "game/settings.hpp"
#include "dictionary.hpp"
#include "environment.hpp"
#include "objects/factory.hpp"
#include "infobox_factory.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

InfoboxRawMaterial::InfoboxRawMaterial( Widget* parent, const Tile& tile )
  : InfoboxConstruction( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 170, 510 - 16, 170 + 74 ) )
{
  Widget::setupUI( GameSettings::rcpath( "/gui/infoboxraw.gui" ) );
  FactoryPtr rawmb = ptr_cast<Factory>( tile.overlay() );
  _type = rawmb->type();

  setConstruction( ptr_cast<Construction>( rawmb ) );

  Label* lbDamage = findChildA<Label*>( "lbDamage", true, this );
  Label* lbProgress = findChildA<Label*>( "lbProgress", true, this );
  //Label* lbAbout = findChildA<Label*>( "lbAbout", true, this );
  Label* lbProductivity = findChildA<Label*>( "lbProductivity", true, this );

  if( rawmb->produceGoodType() != Good::none )
  {
    Picture pic = GoodHelper::getPicture( rawmb->produceGoodType() );
    new Image( this, Point( 10, 10 ), pic );
  }

  _updateWorkersLabel( Point( 32, 160 ), 542, rawmb->maxWorkers(), rawmb->numberWorkers() );

  if( lbDamage != NULL )
  {
    std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                            (int)rawmb->getState( Construction::damage ),
                                            (int)rawmb->getState( Construction::fire ) );
    lbDamage->setText( text );
  }

  if( lbProgress != NULL )
  {
    std::string text = StringHelper::format( 0xff, "%s %d%%", _("##rawm_production_complete_m##"), rawmb->getProgress() );
    lbProgress->setText( text );
  }

  std::string title = MetaDataHolder::getPrettyName( rawmb->type() );
  _title()->setText( _(title) );

  std::string text = rawmb->workersProblemDesc();
  std::string cartInfo = rawmb->cartStateDesc();
  text = ( StringHelper::format( 0xff, "%s\n%s", _(text), _( cartInfo ) ) );

  if( lbProductivity != NULL )
  {
    lbProductivity->setText( _(text) );
  }
}

InfoboxRawMaterial::~InfoboxRawMaterial()
{
}

void InfoboxRawMaterial::showDescription()
{
  DictionaryWindow::show( getEnvironment()->rootWidget(), _type );
}

}//end namespace gui
