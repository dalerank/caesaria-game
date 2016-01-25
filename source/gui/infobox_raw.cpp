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
#include "good/helper.hpp"
#include "image.hpp"
#include "core/utils.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "dictionary.hpp"
#include "environment.hpp"
#include "objects/factory.hpp"
#include "infobox_factory.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"
#include "game/infoboxmanager.hpp"
#include "dialogbox.hpp"
#include "core/common.hpp"
#include "texturedbutton.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(vinard,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(wheat_farm,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(vegetable_farm,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(olive_farm,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(fig_farm,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(meat_farm,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(clay_pit,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(lumber_mill,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(quarry,AboutRawMaterial)
REGISTER_OBJECT_BASEINFOBOX(iron_mine,AboutRawMaterial)

AboutRawMaterial::AboutRawMaterial(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 170, 510 - 16, 170 + 64 ) )
{  
  Widget::setupUI( ":/gui/infoboxraw.gui" );

  FactoryPtr rawmb = tile.overlay<Factory>();
  _type = rawmb->type();

  setBase( rawmb );
  _setWorkingVisible( true );

  INIT_WIDGET_FROM_UI( Label*, lbProductivity )
  INIT_WIDGET_FROM_UI( Label*, lbProgress )

  if( rawmb->produceGoodType() != good::none )
  {
    add<Image>( Point( 10, 10 ), good::Info( rawmb->produceGoodType() ).picture() );
  }

  _updateWorkersLabel( Point( 32, 160 ), 542, rawmb->maximumWorkers(), rawmb->numberWorkers() );

  if( lbProgress != NULL )
  {
    std::string text = fmt::format( "{} {}%", _("##rawm_production_complete_m##"), rawmb->progress() );
    lbProgress->setText( text );
  }

  _lbTitle()->setText( _( rawmb->info().prettyName() ) );

  std::string text = rawmb->workersProblemDesc();
  std::string cartInfo = rawmb->cartStateDesc();
  text = ( fmt::format( "{}\n{}", _(text), _( cartInfo ) ) );

  if( lbProductivity != NULL )
  {
    lbProductivity->setText( _(text) );
  }

  INIT_WIDGET_FROM_UI( TexturedButton*, btnHelp )
  if( btnHelp )
  {
    Rect rect = btnHelp->relativeRect();
    rect += Point( btnHelp->width() + 5, 0 );
    rect.rright() += 60;
    auto& btn = add<PushButton>( rect, "Adv.Info", -1, false, PushButton::whiteBorderUp );
    CONNECT_LOCAL( &btn, onClicked(), AboutRawMaterial::_showAdvInfo )
  }
}

void AboutRawMaterial::_showAdvInfo()
{
  std::string workerState = fmt::format( "Damage={}\nFire={}\n",
                                         utils::objectState( base(), pr::damage ),
                                         utils::objectState( base(), pr::fire ) );

  dialog::Information( ui(), "Information", workerState );
}

AboutRawMaterial::~AboutRawMaterial() {}
void AboutRawMaterial::_showHelp() { ui()->add<DictionaryWindow>( _type ); }

}

}//end namespace gui
