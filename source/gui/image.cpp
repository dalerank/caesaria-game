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

#include "image.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color.hpp"

using namespace std;

namespace gui
{

class Image::Impl
{
public:
  Picture bgPicture;
  PictureRef background;
  Image::Mode mode;
  bool needUpdateTexture;

  ~Impl()
  {
    background.reset();
  }

public oc3_signals:
	Signal0<> onClickedSignal;
};

//! constructor
Image::Image( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
	_d->mode = Image::fit;
}

Image::Image(Widget* parent, Rect rectangle, Picture pic, Mode mode, int id)
: Widget( parent, id, rectangle),
	_d( new Impl )
{
  if( mode == Image::image )
  {
    mode = Image::fit;
    setWidth( pic.getWidth() );
    setHeight( pic.getHeight() );
  }

  _d->bgPicture = pic;
  _d->needUpdateTexture = true;
  #ifdef _DEBUG
    setDebugName( "Image");
#endif
}

Image::Image(Widget* parent, Point pos, Picture pic, int id)
	: Widget( parent, id, Rect( pos, pic.getSize() ) ),
		_d( new Impl )
{
  _d->mode = Image::image;
  _d->bgPicture = pic;
  _d->needUpdateTexture = true;
}

void Image::_updateTexture( GfxEngine& painter )
{
  Size imageSize = getSize();

  if( _d->background && _d->background->getSize() != imageSize )
  {
    _d->background.reset();  
  }

  if( !_d->background )
  {
    _d->background.reset( Picture::create( imageSize ) );
  }

  // draw button background
  if( _d->bgPicture.isValid() )
  {    
    switch( _d->mode )
    {
    case Image::native: _d->background->draw( _d->bgPicture, Point( 0, 0 ), true ); break;

    case Image::fit:
      _d->background->draw( _d->bgPicture, Point( getWidth() - _d->bgPicture.getWidth(),
                                                  getHeight() - _d->bgPicture.getHeight() ) / 2, false );
    break;

    case Image::image:
      _d->background->draw( _d->bgPicture,
                            Rect( Point(0, 0), _d->bgPicture.getSize()),
                            Rect( Point( 0, 0 ), getSize() ), false );
    break;
    }
  }    
  else
  {
    _d->background->fill( 0xff000000, Rect( 0, 0, 0, 0 ) );
  }
}

//! destructor
Image::~Image()
{
}

//! draws the element and its children
void Image::draw( GfxEngine& painter )
{
  if ( !isVisible() )
    return;

  // draw background
  if( _d->background )
  {
    painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop(), &getAbsoluteClippingRectRef() );
  }

  Widget::draw( painter );
}


Signal0<>& Image::onClicked()
{
  return _d->onClickedSignal;
}


void Image::beforeDraw( GfxEngine& painter )
{
  if( _d->needUpdateTexture )
  {
    _updateTexture( painter );

    _d->needUpdateTexture = false;
  }

  Widget::beforeDraw( painter );
}

void Image::setPicture(Picture picture )
{
  _d->bgPicture = picture;
  _d->needUpdateTexture = true;
}

void Image::_resizeEvent()
{
  _d->needUpdateTexture = true;
}

void Image::setupUI(const VariantMap& ui)
{
  Widget::setupUI( ui );

  setPicture( Picture::load( ui.get( "image" ).toString() ) );
  std::string mode = ui.get( "mode", "fit" ).toString();
  if( mode == "fit" ) { _d->mode = Image::fit; }
  else if( mode == "image" ) { _d->mode = Image::image; }
  else if( mode == "native" ) { _d->mode = Image::native; }
  else { _d->mode = Image::fit; }
}

PictureRef& Image::getPicture()
{
  return _d->background;
}

}//end namespace gui
