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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "steam.hpp"

#ifdef CAESARIA_USE_STEAM

#include "core/osystem.hpp"
#include "core/logger.hpp"
#include "public/steam/steam_api.h"

#ifdef CAESARIA_PLATFORM_WIN
#include "helper/helper.h"
#endif

namespace steamapi
{

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

static const AppId_t CAESARIA_STEAM_APPID=327640;
CAESARIA_LITERALCONST(stat_num_games)
CAESARIA_LITERALCONST(stat_num_wins)

struct Achievement
{
  AchievementType id;
  const char* uniqueName;
  char caption[128];
  char description[256];
  bool reached;
  int idIconImage;
  gfx::Picture image;

  void unlockAchievement(steamapi::Achievement &achievement);
};

struct XClient
{
  ISteamUser* user;
  ISteamUserStats* stats;
} xclient;

Achievement glbAchievements[achievementNumber] =
{
  _ACH_ID( achievementFirstWin, "achv_first_win" ),
  _ACH_ID( achievementNewGraphics, "achv_new_graphics" ),
  _ACH_ID( achievementNewVillage,  "achv_new_village"  ),
};

class UserStats
{
public:
#ifdef CAESARIA_PLATFORM_WIN
  uint64 steamId;
#else
  CSteamID steamId;
#endif

  gfx::Picture avatarImage;
  int32 campaignFirstMission;
  int32 totalGamesPlayed;
  int32 totalNumWins;
  int32 totalNumLosses;
  bool needStoreStats;
  bool statsValid, statsUpdate;

#ifndef CAESARIA_PLATFORM_WIN
  STEAM_CALLBACK( UserStats, receivedUserStats, UserStatsReceived_t, _callbackUserStatsReceived );
  STEAM_CALLBACK( UserStats, updateUserStats, UserStatsStored_t, _callbackUserStatsStored );
  STEAM_CALLBACK( UserStats, updateAchievementInfo, UserAchievementStored_t, _callbackAchievementStored );

  UserStats()
    :
    _callbackUserStatsReceived( this, &UserStats::receivedUserStats ),
    _callbackUserStatsStored( this, &UserStats::updateUserStats ),
    _callbackAchievementStored( this, &UserStats::updateAchievementInfo )
#else
  void receivedUserStats();
  void updateUserStats();
  void updateAchievementInfo();

  UserStats()
#endif
  {
    totalGamesPlayed = 0;
    totalNumWins = 0;
    campaignFirstMission = 0;
    totalNumLosses = 0;
    statsUpdate = false;
    needStoreStats = false;
    statsValid = false;
  }    

  void unlockAchievement( Achievement &achievement );
  void evaluateAchievement( Achievement& achievement );
  void storeStatsIfNecessary();
};


//-----------------------------------------------------------------------------
// Purpose: Unlock this achievement
//-----------------------------------------------------------------------------
void UserStats::unlockAchievement( Achievement &achievement )
{
  achievement.reached = true;

  // the icon may change once it's unlocked
  achievement.idIconImage = 0;

  // mark it down
#ifdef  CAESARIA_PLATFORM_WIN
  sth_setAchievement( achievement.uniqueName );
#else
  if( xclient.stats )
  {
      bool result = xclient.stats->SetAchievement( achievement.uniqueName );
  }
#endif

  // Store stats end of frame
  needStoreStats = true;
}

//-----------------------------------------------------------------------------
// Purpose: see if we should unlock this achievement
//-----------------------------------------------------------------------------
void UserStats::evaluateAchievement( Achievement& achievement )
{
  // Already have it?
  if ( achievement.reached )
    return;

  switch ( achievement.id )
  {
  case achievementNewVillage: if(campaignFirstMission>0){unlockAchievement(achievement);} break;
  case achievementFirstWin: if(totalNumWins>0){ unlockAchievement(achievement);}; break;

  /*
  case ACH_TRAVEL_FAR_SINGLE:
    if ( m_flGameFeetTraveled > 500 )
    {
            UnlockAchievement( achievement );
    }
  break;*/
  }
}

void UserStats::storeStatsIfNecessary()
{
#ifdef CAESARIA_PLATFORM_WIN
  if( needStoreStats )
  {
    // already set any achievements in UnlockAchievement

    // set stats
    sth_SetStat( lc_stat_num_games, totalGamesPlayed );
    sth_SetStat( lc_stat_num_wins, totalNumWins );
    sth_SetStat( "NumLosses", totalNumLosses );
    // Update average feet / second stat
    //m_pSteamUserStats->UpdateAvgRateStat( "AverageSpeed", m_flGameFeetTraveled, m_flGameDurationSeconds );
    // The averaged result is calculated for us

    bool bSuccess = sth_StoreStats();
    // If this failed, we never sent anything to the server, try
    // again later.
    needStoreStats = !bSuccess;
  }
#else
  if( !xclient.stats )
    return;

  if( needStoreStats )
  {
    // already set any achievements in UnlockAchievement

    // set stats
    xclient.stats->SetStat( lc_stat_num_games, totalGamesPlayed );
    xclient.stats->SetStat( lc_stat_num_wins, totalNumWins );
    xclient.stats->SetStat( "NumLosses", totalNumLosses );
    // Update average feet / second stat
    //m_pSteamUserStats->UpdateAvgRateStat( "AverageSpeed", m_flGameFeetTraveled, m_flGameDurationSeconds );
    // The averaged result is calculated for us

    bool bSuccess = xclient.stats->StoreStats();
    // If this failed, we never sent anything to the server, try
    // again later.
    needStoreStats = !bSuccess;
  }
#endif
}

static UserStats glbUserStats;

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

bool checkSteamRunning()
{
  // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
  // local Steam client and also launches this game again.

  // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
  // removed steam_appid.txt from the game depot.
  Logger::warning( "Check running Steam" );
  bool needRestart = SteamAPI_RestartAppIfNecessary( CAESARIA_STEAM_APPID );
  return !needRestart;
}

bool connect()
{
#ifdef CAESARIA_PLATFORM_MACOSX
  SteamAPI_Shutdown();
#endif
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
  Logger::warning( "CurrentGameLanguage: %s", SteamApps()->GetCurrentGameLanguage() );
  if ( !SteamUser()->BLoggedOn() )
  {
    Logger::warning( "Steam user is not logged in\n" );
    OSystem::error( "Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n" );
    return false;
  }  

  return true;
}

void close()
{
  SteamAPI_Shutdown();
}

void update()
{
// Run Steam client callbacks
#ifdef CAESARIA_PLATFORM_WIN
  sth_runCallbacks();
  glbUserStats.receivedUserStats();
#else
  SteamAPI_RunCallbacks();
#endif
}

void init()
{
  xclient.user = SteamUser();
  xclient.stats = SteamUserStats();

#if defined(CAESARIA_PLATFORM_WIN) && defined(__GNUC__)
  xclient.user = 0;
  xclient.stats = 0;
  glbUserStats.steamId = sth_getSteamID();
  sth_requestCurrentStats();
#else
  if( xclient.user->BLoggedOn() )
  {
    Logger::warning( "Try receive steamID:" );
    glbUserStats.steamId = xclient.user->GetSteamID();
  }
  else
  {
    Logger::warning( "SteamUser is null" );
  }

  Logger::warning("Reqesting Current Stats:" );
  xclient.stats->RequestCurrentStats();
#endif
}

void evaluateAchievements()
{
  // Evaluate achievements
  for( int iAch = 0; iAch < achievementNumber; ++iAch )
  {
    glbUserStats.evaluateAchievement( glbAchievements[iAch] );
  }

  // Store stats
  glbUserStats.storeStatsIfNecessary();
}

void unlockAchievement(AchievementType achivId)
{
  if( achivId >=0 && achivId < achievementNumber )
  {
    if( !glbAchievements[ achivId ].reached )
      glbUserStats.unlockAchievement( glbAchievements[ achivId ] );
  }
  else
  {
    Logger::warning( "Unknown achievement ID:%d", achivId );
  }
}

const gfx::Picture& achievementImage(AchievementType achivId)
{
  if( achivId >=0 && achivId < achievementNumber )
  {
    return glbAchievements[ achivId ].image;
  }
  else
  {
    Logger::warning( "Unknown achievement ID:%d", achivId );
  }

  return gfx::Picture::getInvalid();
}

std::string userName()
{
  // We use Steam persona names for our players in-game name.  To get these we
  // just call SteamFriends()->GetFriendPersonaName() this call will work on friends,
  // players on the same game server as us (if using the Steam game server auth API)
  // and on ourself.
  std::string playerName = SteamFriends()->GetPersonaName();

  return playerName;
}

const gfx::Picture& getSteamImage( int id )
{
  if ( id != -1 )
  {
    // We haven't created a texture for this image index yet, do so now
    // Get the image size from Steam, making sure it looks valid afterwards
    uint32 uAvatarWidth, uAvatarHeight;
    SteamUtils()->GetImageSize( id, &uAvatarWidth, &uAvatarHeight );

    Size newSize( uAvatarWidth, uAvatarHeight );
    if( newSize.area() > 0 )
    {
      // Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
      const unsigned int imgSize = uAvatarWidth * uAvatarHeight * 4;
      std::vector<unsigned char> avatarRGBA( imgSize );
      SteamUtils()->GetImageRGBA( id, (uint8*)avatarRGBA.data(), imgSize );

      int32* rImg = (int32*)avatarRGBA.data();
      for( unsigned int y=0; y < uAvatarHeight; y++ )
        for( unsigned int x=0; x< uAvatarWidth; x++ )
        {
          NColor cl( rImg[ y * uAvatarWidth + x ] );
          rImg[ y * uAvatarWidth + x ] = cl.abgr();
        }

      return *gfx::Picture::create( newSize, avatarRGBA.data() );
    }
  }

  return gfx::Picture::getInvalid();
}

const gfx::Picture& userImage()
{
  // We also want to use the Steam Avatar image inside the HUD if it is available.
  // We look it up via GetMediumFriendAvatar, which returns an image index we use
  // to look up the actual RGBA data below.
#ifdef CAESARIA_PLATFORM_WIN
  if( glbUserStats.steamId != 0 )
  {
    if( !glbUserStats.avatarImage.isValid() )
    {
      int iImage = sth_getMediumFriendAvatar( glbUserStats.steamId );
      glbUserStats.avatarImage = getSteamImage( iImage );
    }
  }
#else
  if( glbUserStats.steamId.IsValid() )
  {
    if( !glbUserStats.avatarImage.isValid() )
    {
      int iImage = SteamFriends()->GetMediumFriendAvatar( glbUserStats.steamId );
      glbUserStats.avatarImage = getSteamImage( iImage );
    }
  }
#endif

  return glbUserStats.avatarImage;
}

//-----------------------------------------------------------------------------
// Purpose: Our stats data was stored!
//-----------------------------------------------------------------------------
#ifdef CAESARIA_PLATFORM_WIN
void UserStats::updateUserStats()
{

}
#else
void UserStats::updateUserStats( UserStatsStored_t *pCallback )
{
  // we may get callbacks for other games' stats arriving, ignore them
  if ( CAESARIA_STEAM_APPID == pCallback->m_nGameID )
  {
    if ( k_EResultOK == pCallback->m_eResult )
    {
      Logger::warning( "StoreStats - success\n" );
    }
    else if ( k_EResultInvalidParam == pCallback->m_eResult )
    {
      // One or more stats we set broke a constraint. They've been reverted,
      // and we should re-iterate the values now to keep in sync.
      Logger::warning( "StoreStats - some failed to validate\n" );
      // Fake up a callback here so that we re-load the values.
      UserStatsReceived_t callback;
      callback.m_eResult = k_EResultOK;
      callback.m_nGameID = CAESARIA_STEAM_APPID;
      receivedUserStats( &callback );
    }
    else
    {
      Logger::warning( "StoreStats - failed, %d\n", pCallback->m_eResult );
    }
  }
}
#endif

bool isStatsReceived()
{
  bool ret = glbUserStats.statsValid;
  glbUserStats.statsValid = false;
  return ret;
}

//-----------------------------------------------------------------------------
// Purpose: An achievement was stored
//-----------------------------------------------------------------------------
#ifdef CAESARIA_PLATFORM_WIN
void UserStats::updateAchievementInfo()
{

}
#else
void UserStats::updateAchievementInfo( UserAchievementStored_t *pCallback )
{
  // we may get callbacks for other games' stats arriving, ignore them
  if ( CAESARIA_STEAM_APPID == pCallback->m_nGameID )
  {
    if ( 0 == pCallback->m_nMaxProgress )
    {
      Logger::warning( "Achievement '%s' unlocked!", pCallback->m_rgchAchievementName );
    }
    else
    {
      Logger::warning( "Achievement '%s' progress callback, (%d,%d)\n",
                       pCallback->m_rgchAchievementName,
                       pCallback->m_nCurProgress,
                       pCallback->m_nMaxProgress );
    }
  }
}

//-----------------------------------------------------------------------------
// Purpose: We have stats data from Steam. It is authoritative, so update
//			our data with those results now.
//-----------------------------------------------------------------------------
#endif


#ifdef CAESARIA_PLATFORM_WIN
void UserStats::receivedUserStats()
{
  if( sth_isStatsAvailable() )
  {
    statsValid = true;

    // load achievements
    for( int iAch = 0; iAch < achievementNumber; ++iAch )
    {
      Achievement &ach = glbAchievements[iAch];
      ach.reached = sth_getAchievementReached( ach.uniqueName );
      sth_getAchievementAttribute( ach.uniqueName, "name", ach.caption );
      sth_getAchievementAttribute( ach.uniqueName, "desc", ach.description );

      ach.idIconImage = sth_getAchievementIcon( ach.uniqueName );
      ach.image = getSteamImage( ach.idIconImage );
    }

    // load stats
    totalGamesPlayed = sth_getStat( lc_stat_num_games );
    totalNumWins = sth_getStat( lc_stat_num_wins );
  }
}
#else
void UserStats::receivedUserStats(UserStatsReceived_t *pCallback)
{
  ISteamUserStats* steamUserStats = SteamUserStats();
  if ( !steamUserStats )
    return;

  // we may get callbacks for other games' stats arriving, ignore them
  if ( CAESARIA_STEAM_APPID == pCallback->m_nGameID )
  {
    if ( k_EResultOK == pCallback->m_eResult )
    {
      Logger::warning( "Received stats and achievements from Steam\n" );

      statsValid = true;

      // load achievements
      for( int iAch = 0; iAch < achievementNumber; ++iAch )
        {
          Achievement &ach = glbAchievements[iAch];
          steamUserStats->GetAchievement( ach.uniqueName, &ach.reached );
          sprintf( ach.caption, "%s", steamUserStats->GetAchievementDisplayAttribute( ach.uniqueName, "name" ) );
          sprintf( ach.description, "%s", steamUserStats->GetAchievementDisplayAttribute( ach.uniqueName, "desc" ) );

          ach.idIconImage = steamUserStats->GetAchievementIcon( ach.uniqueName );
          ach.image = getSteamImage( ach.idIconImage );
        }

      // load stats
      steamUserStats->GetStat( lc_stat_num_games, &totalGamesPlayed );
      steamUserStats->GetStat( lc_stat_num_wins, &totalNumWins );
      steamUserStats->GetStat( "NumLosses", &totalNumLosses );
    }
    else
    {
      Logger::warning( "RequestStats - failed, %d\n", pCallback->m_eResult );
    }
  }
}
#endif

std::string achievementCaption(AchievementType achivId)
{
  if( achivId >=0 && achivId < achievementNumber )
  {
    return glbAchievements[ achivId ].caption;
  }
  else
  {
    Logger::warning( "Unknown achievement ID:%d", achivId );
  }

  return "unknown_achv";
}

bool isAchievementReached(AchievementType achivId)
{
  if( achivId >=0 && achivId < achievementNumber )
  {
    return glbAchievements[ achivId ].reached;
  }
  else
  {
    Logger::warning( "Unknown achievement ID:%d", achivId );    
  }
  return false;
}

void missionWin()
{
  glbUserStats.totalNumWins++;
  glbUserStats.totalGamesPlayed++;
  evaluateAchievements();
}

}

#endif //CAESARIA_USE_STEAM
