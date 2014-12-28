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

#include "steam.hpp"

#ifdef CAESARIA_USE_STEAM

#include "public/steam/steam_api.h"
#include "core/osystem.hpp"
#include "core/logger.hpp"

namespace steamapi
{

static const AppId_t CAESARIA_STEAM_APPID=327640;
static gfx::Picture const* avatarImage = 0;
static CSteamID glbSteamId;

//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
  // if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
  // if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
  Logger::warning( pchDebugText );

  if ( nSeverity >= 1 )
  {
    // place to set a breakpoint for catching API errors
    int x = 3;
    x = x;
  }
}

bool Handler::checkSteamRunning()
{
  // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
  // local Steam client and also launches this game again.

  // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
  // removed steam_appid.txt from the game depot.
  Logger::warning( "Check running Steam" );
  bool needRestart = SteamAPI_RestartAppIfNecessary( CAESARIA_STEAM_APPID );
  return !needRestart;
}

bool Handler::connect()
{
  // Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
  // You don't necessarily have to though if you write your code to check whether all the Steam
  // interfaces are NULL before using them and provide alternate paths when they are unavailable.
  //
  // This will also load the in-game steam overlay dll into your process.  That dll is normally
  // injected by steam when it launches games, but by calling this you cause it to always load,
  // even when not launched via steam.
  Logger::warning( "Init Steam api" );
  if ( !SteamAPI_Init() )
  {
    Logger::warning( "SteamAPI_Init() failed" );
    OSystem::error( "Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n" );
    return false;
  }

  // set our debug handler
  SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

  // Tell Steam where it's overlay should show notification dialogs, this can be top right, top left,
  // bottom right, bottom left. The default position is the bottom left if you don't call this.
  // Generally you should use the default and not call this as users will be most comfortable with
  // the default position.  The API is provided in case the bottom right creates a serious conflict
  // with important UI in your game.
  SteamUtils()->SetOverlayNotificationPosition( k_EPositionTopRight );

  // Ensure that the user has logged into Steam. This will always return true if the game is launched
  // from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
  // will return false.
  ISteamApps* apps = SteamApps();

  Logger::warning( "CurrentGameLanguage: %s", apps->GetCurrentGameLanguage() );
  if ( !SteamUser()->BLoggedOn() )
  {
    Logger::warning( "Steam user is not logged in\n" );
    OSystem::error( "Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n" );
    return false;
  }  

  Logger::warning("Reqesting Current Stats:" );
  bool result = SteamUserStats()->RequestCurrentStats();
  return true;
}

void Handler::close()
{
  SteamAPI_Shutdown();
}

void Handler::update()
{
  // Run Steam client callbacks
  SteamAPI_RunCallbacks();
}

void Handler::init()
{
#ifndef CAESARIA_PLATFORM_WIN //strange bug on windows bl, access violation when using SteamUser()((
  if( SteamUser()->BLoggedOn() )
  {
    Logger::warning( "Try receive steamID: %d", (int)SteamUser() );
    glbSteamId = SteamUser()->GetSteamID();
  }
  else
  {
    Logger::warning( "SteamUser is null" );
  }
#endif
}

std::string Handler::userName()
{
  // We use Steam persona names for our players in-game name.  To get these we
  // just call SteamFriends()->GetFriendPersonaName() this call will work on friends,
  // players on the same game server as us (if using the Steam game server auth API)
  // and on ourself.
  std::string playerName = SteamFriends()->GetPersonaName();

  return playerName;
}

const gfx::Picture& Handler::userImage()
{
  avatarImage = &gfx::Picture::getInvalid();
  // We also want to use the Steam Avatar image inside the HUD if it is available.
  // We look it up via GetMediumFriendAvatar, which returns an image index we use
  // to look up the actual RGBA data below.

  if( glbSteamId.IsValid() )
  {
    int iImage = SteamFriends()->GetMediumFriendAvatar( glbSteamId );
    if ( iImage != -1 )
    {
      // We haven't created a texture for this image index yet, do so now
      // Get the image size from Steam, making sure it looks valid afterwards
      uint32 uAvatarWidth, uAvatarHeight;
      SteamUtils()->GetImageSize( iImage, &uAvatarWidth, &uAvatarHeight );

      Size newSize( uAvatarWidth, uAvatarHeight );
      if( newSize.area() > 0 && ( !avatarImage || avatarImage->size() != newSize) )
      {
        // Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
        const unsigned int imgSize = uAvatarWidth * uAvatarHeight * 4;
        std::vector<unsigned char> avatarRGBA( imgSize );
        SteamUtils()->GetImageRGBA( iImage, (uint8*)avatarRGBA.data(), imgSize );

        int32* rImg = (int32*)avatarRGBA.data();
        for( unsigned int y=0; y < uAvatarHeight; y++ )
          for( unsigned int x=0; x< uAvatarWidth; x++ )
          {
            NColor cl( rImg[ y * uAvatarWidth + x ] );
            rImg[ y * uAvatarWidth + x ] = cl.abgr();
          }

        avatarImage = gfx::Picture::create( newSize, avatarRGBA.data() );
      }
    }
  }

  return *avatarImage;
}

}

#endif //CAESARIA_USE_STEAM
