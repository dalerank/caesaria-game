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

#include "smkviewer.hpp"
#include <smacker.h>
#include "gfx/picture.hpp"
#include "gfx/engine.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "widget_factory.hpp"

#include <stdio.h>

using namespace std;
using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(SmkViewer)

class SmkViewer::Impl
{
public:
  SmkViewer::Mode mode;
  Picture background;
  bool needUpdateTexture;
  unsigned int lastFrameTime;

  struct {
    smk source = nullptr;

    struct {
      unsigned long width;
      unsigned long height;
    } size;

    struct {
      unsigned long count;
      unsigned long current;
    } frame;

    unsigned char* image = nullptr;
  } smkf;
  double usecsInFrame;

  /* arrays for audio track metadata */
  unsigned char   a_trackmask, a_channels[7], a_depth[7];
  unsigned long   a_rate[7];

  //unsigned char *palette_data;
  unsigned char* pallete;

  int colors[256];

  void updateTexture(gfx::Engine& painter, const Size& size );
  void updatePallete();
  void nextFrame();

public signals:
  Signal0<> onFinishSignal;
};

//! constructor
SmkViewer::SmkViewer( Widget* parent )
  : Widget( parent, -1, Rect( 0, 0, 50, 50) ), _d( new Impl )
{
  _d->mode = native;
}

void SmkViewer::beforeDraw( gfx::Engine& painter )
{
  if( isFocused() && _d->smkf.source != nullptr && DateTime::elapsedTime() - _d->lastFrameTime > (_d->usecsInFrame / 1000) )
  {
    _d->lastFrameTime = DateTime::elapsedTime();
    _d->needUpdateTexture = true;

    _d->nextFrame();

    _d->updatePallete();
    /* Retrieve the palette and image */
    _d->smkf.image = smk_get_video( _d->smkf.source );
  }

  if( _d->needUpdateTexture )
  {
    _d->needUpdateTexture = false;
    _d->updateTexture( painter, size() );
  }

  Widget::beforeDraw( painter );
}

void SmkViewer::setFilename(const vfs::Path& path)
{
  vfs::Directory dir( path.directory() );
  vfs::Path rpath = dir.find( path.baseName(), vfs::Path::ignoreCase );

  if( !rpath.exist() )
    return;

  _d->smkf.source = smk_open_file( rpath.toCString(), SMK_MODE_MEMORY );
  if( _d->smkf.source != nullptr )
  {
    smk_info_all( _d->smkf.source, &_d->smkf.frame.current, &_d->smkf.frame.count, &_d->usecsInFrame );
    smk_info_video( _d->smkf.source, &_d->smkf.size.width, &_d->smkf.size.height, NULL );
    smk_info_audio( _d->smkf.source, &_d->a_trackmask, _d->a_channels, _d->a_depth, _d->a_rate);

    Logger::warning( "Opened file {0}\nWidth: {1}\nHeight: {2}\nFrames: {3}\nFPS: {4}\n", path.toString(),
                     _d->smkf.size.width, _d->smkf.size.height, _d->smkf.frame.count, 1000000.0 / _d->usecsInFrame );

    smk_enable_video( _d->smkf.source, 1 );

    /* process first frame */
    smk_first( _d->smkf.source );
    _d->updatePallete();
    _d->smkf.image = smk_get_video(_d->smkf.source);

    _d->lastFrameTime = DateTime::elapsedTime();
    _d->needUpdateTexture = true;

    if( _d->mode == SmkViewer::video )
    {
      setWidth( _d->smkf.size.width );
      setHeight( _d->smkf.size.height );
    }
  }
}

Signal0<>& SmkViewer::onFinish() { return _d->onFinishSignal; }

SmkViewer::SmkViewer(Widget* parent, const Rect& rectangle, Mode mode)
: Widget( parent, -1, rectangle ), _d( new Impl )
{
  _d->mode = mode;
  _d->needUpdateTexture = true;
  #ifdef DEBUG
    setDebugName( TEXT(SmkViewer) );
  #endif
}

void SmkViewer::Impl::updateTexture( gfx::Engine& painter, const Size& size )
{
  Size imageSize = size;

  if( background.isValid() && background.size() != imageSize )
  {
    background = Picture();
  }

  if( !background.isValid() )
  {
    background = Picture( imageSize, 0, true );
  }

  unsigned int* pixels = background.lock();
  unsigned int bw = background.width();

  Size safe( math::min<unsigned int>( background.width(), smkf.size.width ),
             math::min<unsigned int>( background.height(), smkf.size.height ) );

  if( smkf.source )
  {
    for( int i = safe.height() - 1; i >= 0; i--)
    {
      for( int j = 0; j < safe.width(); j++ )
      {
        unsigned char index = smkf.image[i * smkf.size.width + j];
        unsigned int* bufp32;
        bufp32 = pixels + i * bw + j;
        *bufp32 = colors[ index ];
      }
    }
  }
  background.unlock();
  background.update();
}

void SmkViewer::Impl::updatePallete()
{
  pallete = smk_get_palette( smkf.source );

  for( int i = 0; i < 256; i++)
  {
    int c = ( 0xff000000 + (pallete[(i * 3) + 2])
              + (pallete[(i * 3) + 1]<<8)
              + (pallete[(i * 3)]<<16) );

    colors[ i ] = c;
  }
}

void SmkViewer::Impl::nextFrame()
{
  /* get frame number */
  if( smkf.frame.current == smkf.frame.count )
    return;

  smk_info_all( smkf.source, &smkf.frame.current, NULL, NULL);

  if( smkf.frame.current+1 == smkf.frame.count )
  {
    smkf.frame.current++;
    emit onFinishSignal();
  }

  Logger::warning( " -> Frame {}...", smkf.frame.current );

  smk_next( smkf.source );
}

//! destructor
SmkViewer::~SmkViewer()
{
  if( _d->smkf.source )
  {
    smk_close( _d->smkf.source );
  }
}

//! draws the element and its children
void SmkViewer::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  // draw background
  if( _d->background.isValid() )
  {
    painter.draw( _d->background, _d->background.originRect(), absoluteRect(), &absoluteClippingRectRef() );
  }

  Widget::draw( painter );
}

void SmkViewer::_finalizeResize() { _d->needUpdateTexture = true;}

}//end namespace gui
