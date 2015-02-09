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

#include <cstdio>

#include "infobox_senate.hpp"

#include "gfx/tile.hpp"
#include "objects/metadata.hpp"
#include "objects/senate.hpp"
#include "objects/constants.hpp"
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "good/goodhelper.hpp"
#include "texturedbutton.hpp"
#include "events/showadvisorwindow.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

namespace {
  int advisorBtnId = 0x2552;
  Signal0<> invalidBtnClickedSignal;
}

AboutSenate::AboutSenate(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : Simple( parent, Rect( 0, 0, 510, 290 ), Rect( 16, 126, 510 - 16, 126 + 62 ) )
{
  setupUI( ":/gui/infoboxsenate.gui" );

  SenatePtr senate = ptr_cast<Senate>( tile.overlay() );
  std::string title = MetaDataHolder::instance().getData( objects::senate ).prettyName();
  setTitle( _(title) );

  // number of workers
  _updateWorkersLabel( Point( 32, 136), 542, senate->maximumWorkers(), senate->numberWorkers() );

  std::string denariesStr = utils::format( 0xff, "%s %d", _("##senate_save##"), senate->funds() );

  Label* lb = new Label( this, Rect( 60, 35, width() - 16, 35 + 30 ), denariesStr );
  lb->setIcon( good::Helper::picture( good::denaries ) );
  lb->setText( denariesStr );
  lb->setTextOffset( Point( 30, 0 ));

  new Label( this, Rect( 60, 215, 60 + 300, 215 + 24 ), _("##visit_rating_advisor##") );
  TexturedButton* btnAdvisor = new TexturedButton( this, Point( 350, 215 ), Size(28), advisorBtnId, 289 );
  CONNECT( btnAdvisor, onClicked(), this, AboutSenate::_showRatingAdvisor );
  CONNECT( btnAdvisor, onClicked(), this, AboutSenate::deleteLater );
}

AboutSenate::~AboutSenate() {}

Signal0<>& AboutSenate::onButtonAdvisorClicked()
{
  TexturedButton* btn = safety_cast<TexturedButton*>( findChild( advisorBtnId, true ) );
  return btn ? btn->onClicked() : invalidBtnClickedSignal;
}

void AboutSenate::_showRatingAdvisor()
{
  events::GameEventPtr e = events::ShowAdvisorWindow::create( true, advisor::ratings );
  e->dispatch();
}

}

}//end namespace gui
