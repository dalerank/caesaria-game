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
#include "widget_factory.hpp"

using namespace std;
using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(Image)

class Image::Impl
{
public:
  Picture bgPicture;
  Image::Mode mode;

public signals:
  Signal0<> onClickedSignal;
};

//! constructor
Image::Image( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
  _d->mode = Image::fit;
}

Image::Image(Widget* parent, const Rect& rectangle, const Picture& pic, Mode mode, int id)
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

Image::Image(Widget* parent, const Point& pos, const Picture& pic, int id)
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

    case Image::best:
    {
       Size rSize;
       Size ItemSize = _d->bgPicture.size();
       const Size& rectSize = size();
       if (ItemSize.height()<=rectSize.height() && ItemSize.width()<=rectSize.width())
       {
         rSize = ItemSize;
       }

       // анализ высоты
       if (ItemSize.height()>rectSize.height()) {
           // обнаружено превышение, применяем сдвиг
           float dh = ItemSize.width() / (float)ItemSize.height();
           ItemSize.setHeight( rectSize.height() );
           ItemSize.setWidth( rectSize.height() * dh );
       }

       // анализ ширины
       if (ItemSize.width()>rectSize.width()) {
           // обнаружено превышение, применяем сдвиг
           float dw = ItemSize.height() / (float)ItemSize.width();
           ItemSize.setWidth( rectSize.width() );
           ItemSize.setHeight( rectSize.width() * dw );
       }

       painter.draw( _d->bgPicture, Rect( Point(), _d->bgPicture.size() ),
                     Rect( Point(), ItemSize ) + absoluteRect().lefttop(),
                     &absoluteClippingRectRef() );
    }
    break;
    }
  }

  Widget::draw( painter );
}

Signal0<>& Image::onClicked(){  return _d->onClickedSignal;}

void Image::setPicture(const Picture& picture)
{
  _d->bgPicture = picture;

  if( _d->mode == image )
  {
    setWidth(picture.width());
    setHeight(picture.height());
  }
}

void Image::setPicture(const string& name)
{
  _d->mode = Image::image;
  setPicture(Picture(name));
}

void Image::setPicture(const string& rc, int id)
{
  setPicture(Picture(rc, id));
}

void Image::setupUI(const VariantMap& ui)
{
  Widget::setupUI(ui);

  setPicture( Picture( ui.get( "image" ).toString() ) );
  std::string mode = ui.get( "mode" ).toString();
  setMode(mode);
}

void Image::setupUI(const vfs::Path & ui)
{
  Widget::setupUI(ui);
}

void Image::setMode(Image::Mode mode)
{
  _d->mode = mode;
  if( _d->mode == Image::image )
  {
    setWidth( picture().width() );
    setHeight( picture().height() );
  }
}

void Image::setMode(const std::string & alias)
{
  Image::Mode mode = _d->mode;
  if (alias == "fit") { mode = Image::fit; }
  else if (alias == "image") { mode = Image::image; }
  else if (alias == "native") { mode = Image::native; }
  else if (alias == "center") { mode = Image::center; }
  else if (alias == "best") { mode = Image::best; }
  else { mode = Image::image; }

  setMode(mode);
}

Picture Image::picture() const {  return _d->bgPicture;}

}//end namespace gui
