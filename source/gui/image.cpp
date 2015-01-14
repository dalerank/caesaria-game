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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "image.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "core/variant_map.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color.hpp"

using namespace std;
using namespace gfx;

namespace gui
{

class Image::Impl
{
public:
  Picture bgPicture;
  Image::Mode mode;

  ~Impl()
  {
  }

public signals:
	Signal0<> onClickedSignal;
};

//! constructor
Image::Image( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
	_d->mode = Image::fit;
}

Image::Image(Widget* parent, Rect rectangle, const Picture& pic, Mode mode, int id)
: Widget( parent, id, rectangle),
	_d( new Impl )
{ 
	_d->mode = mode;
	if( mode == image )
	{
		setWidth( pic.width() );
		setHeight( pic.height() );
	}

  _d->bgPicture = pic;
  #ifdef _DEBUG
    setDebugName( "Image");
#endif
}

Image::Image(Widget* parent, Point pos, const Picture& pic, int id)
	: Widget( parent, id, Rect( pos, pic.size() ) ),
		_d( new Impl )
{
  _d->mode = Image::image;

  setWidth( pic.width() );
  setHeight( pic.height() );

  _d->bgPicture = pic;
}

//! destructor
Image::~Image() {}

//! draws the element and its children
void Image::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  // draw background
  if( _d->bgPicture.isValid() )
  {
    switch( _d->mode )
    {
    case Image::image:
    case Image::native:
      painter.draw( _d->bgPicture, absoluteRect().lefttop(), &absoluteClippingRectRef() );
    break;


    case Image::fit:
      painter.draw( _d->bgPicture, Rect( Point(), _d->bgPicture.size() ), absoluteRect(), &absoluteClippingRectRef() );
    break;

    case Image::center:
      painter.draw( _d->bgPicture, Point( width() - _d->bgPicture.width(),
                                          height() - _d->bgPicture.height() ) / 2, &absoluteClippingRectRef() );
    break;

    }
  }

  Widget::draw( painter );
}

Signal0<>& Image::onClicked(){  return _d->onClickedSignal;}

void Image::setPicture( Picture picture )
{
  _d->bgPicture = picture;

	if( _d->mode == image )
	{
		setWidth( picture.width() );
		setHeight( picture.height() );
	}
}

void Image::setupUI(const VariantMap& ui)
{
  Widget::setupUI( ui );

  setPicture( Picture::load( ui.get( "image" ).toString() ) );
  std::string mode = ui.get( "mode" ).toString();
  if( mode == "fit" ) { _d->mode = Image::fit; }
  else if( mode == "image" ) { _d->mode = Image::image; }
  else if( mode == "native" ) { _d->mode = Image::native; }
  else if( mode == "center" ) { _d->mode = Image::center; }
  else { _d->mode = Image::image; }

  if( _d->mode == Image::image )
  {
    setWidth( picture().width() );
    setHeight( picture().height() );
  }
}

Picture Image::picture() const {  return _d->bgPicture;}

}//end namespace gui
