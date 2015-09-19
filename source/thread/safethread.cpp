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

#include "safethread.hpp"

namespace threading
{

class SafeThread::Impl
{
public:
  // The function object to call
  Delegate1<bool&> function;
  Delegate0<> onFinish;

  // The error message which is filled when exceptions are thrown within the thread
  std::string error;
  std::thread thread;

  int delay;

  bool running;

  void onTask();
};

void _THKERNEL(SafeThread* ptr)
{
  do
  {
    ptr->_d->onTask();
    std::this_thread::sleep_for( std::chrono::milliseconds( ptr->_d->delay ) );
  }
  while( ptr->running() );

  if( !ptr->_d->onFinish.empty() )
    ptr->_d->onFinish();
}


SafeThread::SafeThread(WorkFunction function) : _d( new Impl )
{
  _d->function = function;
  _d->thread = std::thread(_THKERNEL,this);
  _d->delay = 0;
  _d->running = true;
}

SafeThreadPtr SafeThread::create(WorkFunction function)
{
  SafeThreadPtr p( new SafeThread( function ) );
  p->drop();

  return p;
}

SafeThread::SafeThread(WorkFunction function, StopFunction callbackOnFinish) :
  _d( new Impl )
{
  _d->onFinish = callbackOnFinish;
  _d->thread = std::thread(_THKERNEL,this);
  _d->running = true;
  _d->delay = 0;
  _d->function = function;
}

bool SafeThread::failed() const
{
  return !_d->error.empty();
}

const std::string& SafeThread::errorMessage() const
{
  return _d->error;
}

void SafeThread::join()
{
  if( _d->thread.joinable() )
    _d->thread.join();
}

void SafeThread::setDelay(int ms)
{
  _d->delay = ms;
}

bool SafeThread::running() const { return _d->running; }

void SafeThread::Impl::onTask()
{
  try
  {
    if( !function.empty() )
      function( running );
  }
  catch( std::runtime_error& ex )
  {
    error = ex.what();
  }
}


} // threading
