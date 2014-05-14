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
#include "smk/smacker.h"
#include "gfx/picture.hpp"
#include "gfx/engine.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"

#include <stdio.h>

using namespace std;
using namespace gfx;

namespace gui
{

class SmkViewer::Impl
{
public:
  SmkViewer::Mode mode;
  PictureRef background;
  bool needUpdateTexture;
  unsigned int lastFrameTime;
  smk s;

  unsigned long videoWidth, videoHeight;
  unsigned long frameCount, currentFrame;

  double usecsInFrame;

  /* arrays for audio track metadata */
  unsigned char   a_trackmask, a_channels[7], a_depth[7];
  unsigned long   a_rate[7];

  //unsigned char *palette_data;
  unsigned char* image_data;
  unsigned char* pallete;

  int colors[256];

  void updateTexture(gfx::Engine& painter, const Size& size );
  void updatePallete();
  void nextFrame();

  Impl() : s( 0 ), image_data( 0 ) {}

public oc3_signals:
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
  if( isFocused() && _d->s != NULL && DateTime::elapsedTime() - _d->lastFrameTime > (_d->usecsInFrame / 1000) )
  {
    _d->lastFrameTime = DateTime::elapsedTime();
    _d->needUpdateTexture = true;

    _d->nextFrame();

    _d->updatePallete();
    /* Retrieve the palette and image */
    _d->image_data = smk_get_video( _d->s );
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

  _d->s = smk_open_file( rpath.toString().c_str(), SMK_MODE_MEMORY );
  if( _d->s != NULL )
  {
    smk_info_all( _d->s, &_d->currentFrame, &_d->frameCount, &_d->usecsInFrame );
    smk_info_video( _d->s, &_d->videoWidth, &_d->videoHeight, NULL );
    smk_info_audio( _d->s, &_d->a_trackmask, _d->a_channels, _d->a_depth, _d->a_rate);

    Logger::warning( "Opened file %s\nWidth: %d\nHeight: %d\nFrames: %d\nFPS: %lf\n", path.toString().c_str(),
                     _d->videoWidth, _d->videoHeight, _d->frameCount, 1000000.0 / _d->usecsInFrame );

    smk_enable_video( _d->s, 1 );

    /* process first frame */
    smk_first( _d->s );
    _d->updatePallete();
    _d->image_data = smk_get_video(_d->s);

    _d->lastFrameTime = DateTime::elapsedTime();
    _d->needUpdateTexture = true;

    if( _d->mode == SmkViewer::video )
    {
      setWidth( _d->videoWidth );
      setHeight( _d->videoHeight );
    }
  }
}

Signal0<>& SmkViewer::onFinish() { return _d->onFinishSignal; }

SmkViewer::SmkViewer(Widget* parent, const Rect& rectangle, Mode mode)
: Widget( parent, -1, rectangle ), _d( new Impl )
{
  _d->mode = mode;
  _d->needUpdateTexture = true;
  #ifdef _DEBUG
    setDebugName( "Image");
  #endif
}

void SmkViewer::Impl::updateTexture( gfx::Engine& painter, const Size& size )
{
  Size imageSize = size;

  if( background && background->size() != imageSize )
  {
    background.reset();
  }

  if( !background )
  {
    background.reset( Picture::create( imageSize ) );
  }

  // draw button background
  //background->fill( 0x0000000, Rect() );
  background->lock();

  if( s )
  {
    for( int i = videoHeight - 1; i >= 0; i--)
    {
      for( int j = 0; j < videoWidth; j++ )
      {
        unsigned char index = image_data[i * videoWidth + j];
        background->setPixel( Point( j, i ), colors[ index ] );
      }
    }
  }

  background->unlock();
}

void SmkViewer::Impl::updatePallete()
{
    pallete = smk_get_palette( s );

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
  if( currentFrame == frameCount )
    return;

  smk_info_all(s, &currentFrame, NULL, NULL);

  if( currentFrame+1 == frameCount )
  {
    currentFrame++;
    onFinishSignal.emit();
  }

  Logger::warning( " -> Frame %d...", currentFrame );

  /* Get the audio chunk for this frame from track 0 */
  /* printf("Audio info for track 0: bit-depth %u, channels %u, rate %u\n",
    a_depth[0],
  a_channels[0],
  a_rate[0] );

  audio_data = smk_get_audio(s,0);

  printf(" done.\n");
   */
  /* Advance to next frame */
  smk_next(s);
}

//! destructor
SmkViewer::~SmkViewer()
{
  if( _d->s )
  {
    smk_close( _d->s );
  }
}

//! draws the element and its children
void SmkViewer::draw(gfx::Engine& painter )
{
  if ( !isVisible() )
    return;

  // draw background
  if( _d->background )
  {
    painter.draw( *_d->background, screenLeft(), screenTop(), &absoluteClippingRectRef() );
  }

  Widget::draw( painter );
}

void SmkViewer::_resizeEvent() {  _d->needUpdateTexture = true;}

}//end namespace gui
