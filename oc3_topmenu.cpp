// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.


#include "oc3_topmenu.hpp"
#include "oc3_label.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_sdl_facade.hpp"

static const Uint32 dateLabelOffset = 155;
static const Uint32 populationLabelOffset = 344;
static const Uint32 fundLabelOffset = 464;
static const Uint32 panelBgStatus = 15;

class TopMenu::Impl
{
public:
    Label* lbPopulation;
    Label* lbFunds;
    Label* lbDate;
    Picture* bgPicture;
};

TopMenu* TopMenu::create( Widget* parent, const int height )
{
    TopMenu* ret = new TopMenu( parent, height);
    ret->setGeometry( Rect( 0, 0, parent->getWidth(), height ) );

    PicLoader& loader = PicLoader::instance();
    FontCollection& fonts = FontCollection::instance();

    std::vector<Picture> p_marble;

    for (int i = 1; i<=12; ++i)
    {
        p_marble.push_back(loader.get_picture( ResourceGroup::panelBackground, i));
    }

    SdlFacade &sdlFacade = SdlFacade::instance();
    ret->_d->bgPicture = &sdlFacade.createPicture( ret->getWidth(), ret->getHeight() );
    SDL_SetAlpha( ret->_d->bgPicture->get_surface(), 0, 0);  // remove surface alpha

    int i = 0;
    unsigned int x = 0;
    while (x < ret->getWidth())
    {
        const Picture& pic = p_marble[i%10];
        sdlFacade.drawPicture(pic, *ret->_d->bgPicture, x, 0);
        x += pic.get_width();
        i++;
    }

    Size lbSize( 120, 23 );
    ret->_d->lbPopulation = new Label( ret, Rect( Point( ret->getWidth() - populationLabelOffset, 0 ), lbSize ),
        "Pop 34,124", false, true, -1 );
    ret->_d->lbPopulation->setBackgroundPicture( loader.get_picture( ResourceGroup::panelBackground, panelBgStatus ) );
    ret->_d->lbPopulation->setFont( fonts.getFont(FONT_2_WHITE) );
    ret->_d->lbPopulation->setTextAlignment( alignCenter, alignCenter );
    //_populationLabel.setTextPosition(20, 0);

    ret->_d->lbFunds = new Label( ret, Rect( Point( ret->getWidth() - fundLabelOffset, 0), lbSize ),
        "Dn 10,000", false, true, -1 );
    ret->_d->lbFunds->setFont( fonts.getFont(FONT_2_WHITE));
    ret->_d->lbFunds->setTextAlignment( alignCenter, alignCenter );
    ret->_d->lbFunds->setBackgroundPicture( loader.get_picture( ResourceGroup::panelBackground, panelBgStatus ) );
    //_fundsLabel.setTextPosition(20, 0);

    ret->_d->lbDate = new Label( ret, Rect( Point( ret->getWidth() - dateLabelOffset, 0), lbSize ),
        "Feb 39 BC", false, true, -1 );
    ret->_d->lbDate->setFont( fonts.getFont(FONT_2_YELLOW));
    ret->_d->lbDate->setTextAlignment( alignCenter, alignCenter );
    ret->_d->lbDate->setBackgroundPicture( loader.get_picture( ResourceGroup::panelBackground, panelBgStatus ) );
    //_dateLabel.setTextPosition(20, 0);

    GfxEngine::instance().load_picture(*ret->_d->bgPicture);

    return ret;
}

void TopMenu::draw( GfxEngine& engine )
{
    engine.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );
    Widget::draw( engine );
}

void TopMenu::setPopulation( int value )
{
    char buffer[100];
    sprintf(buffer, "Pop %d", value);  // "'" is the thousands separator
    _d->lbPopulation->setText( buffer );
}

void TopMenu::setFunds( int value )
{
    char buffer[100];
    sprintf(buffer, "Dn %d", value );  // "'" is the thousands separator
    _d->lbFunds->setText( buffer );
}

void TopMenu::setDate( int value )
{
    char buffer[100];

    const char *args[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const char *age[] = {"BC", "AD"};

    sprintf(buffer, "%.3s %d %.2s", args[value % 12], (int)std::abs(((int)value/12-39)), age[((int)value/12-39)>0]);

    //_dateLabel.setText("Feb 39 BC");
    _d->lbDate->setText( buffer );
}

TopMenu::TopMenu( Widget* parent, const int height ) 
: MainMenu( parent, Rect( 0, 0, parent->getWidth(), height ) ),
  _d( new Impl )
{
  addItem( "File", -1, true, true, false, false );
  addItem( "Options", -1, true, true, false, false );
  addItem( "Help", -1, true, true, false, false );
  addItem( "Advisers", -1, true, true, false, false );
}