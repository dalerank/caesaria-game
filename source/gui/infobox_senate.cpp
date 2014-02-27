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
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "good/goodhelper.hpp"
#include "texturedbutton.hpp"
#include "events/showadvisorwindow.hpp"
#include "core/logger.hpp"

using namespace constants;

namespace gui
{

namespace {
  int advisorBtnId = 0x2552;
  Signal0<> invalidBtnClickedSignal;
}

InfoBoxSenate::InfoBoxSenate( Widget* parent, const Tile& tile )
  : InfoboxSimple( parent, Rect( 0, 0, 510, 290 ), Rect( 16, 126, 510 - 16, 126 + 62 ) )
{
  SenatePtr senate = ptr_cast<Senate>( tile.getOverlay() );
  setTitle( MetaDataHolder::instance().getData( building::senate ).getPrettyName() );

  // number of workers
  _updateWorkersLabel( Point( 32, 136), 542, senate->getMaxWorkers(), senate->getWorkersCount() );

  std::string denariesStr = StringHelper::format( 0xff, "%s %d", _("##senate_save##"), senate->getFunds() );

  Label* lb = new Label( this, Rect( 60, 35, getWidth() - 16, 35 + 30 ), denariesStr );
  lb->setIcon( GoodHelper::getPicture( Good::denaries ) );
  lb->setText( denariesStr );
  lb->setTextOffset( Point( 30, 0 ));

  new Label( this, Rect( 60, 215, 60 + 300, 215 + 24 ), _("##visit_rating_advisor##") );
  TexturedButton* btnAdvisor = new TexturedButton( this, Point( 350, 215 ), Size(28), advisorBtnId, 289 );
  CONNECT( btnAdvisor, onClicked(), this, InfoBoxSenate::_showRatingAdvisor );
  CONNECT( btnAdvisor, onClicked(), this, InfoBoxSenate::deleteLater );
}

InfoBoxSenate::~InfoBoxSenate() {}

Signal0<>& InfoBoxSenate::onButtonAdvisorClicked()
{
  TexturedButton* btn = safety_cast<TexturedButton*>( findChild( advisorBtnId, true ) );
  return btn ? btn->onClicked() : invalidBtnClickedSignal;
}

void InfoBoxSenate::_showRatingAdvisor()
{
  events::GameEventPtr e = events::ShowAdvisorWindow::create( true, advisor::ratings );
  e->dispatch();
}

}//end namespace gui
