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

#include "core/osystem.hpp"
#include "core/logger.hpp"
#include "public/steam/steam_api.h"

#ifdef CAESARIA_PLATFORM_WIN
#include "helper/helper.h"
#endif

namespace steamapi
{

#ifdef CAESARIA_USE_STEAM

#define _ACH_ID( id ) { id, #id, #id, "", 0, 0 }
enum MissionName { n2_nvillage=0, nx_count };
enum StatName { stat_num_games=0, stat_num_wins, stat_num_lose, stat_count };

static const AppId_t CAESARIA_STEAM_APPID=0x04ffd8;
static const AppId_t CAESARIA_AVEC3_APPID=0x053124;
static bool gameRunInOfflineMode = false;

struct Achievement
{
  AchievementType id;
  const char* steamName;
  char caption[128];
  char description[256];
  bool reached;
  int idIconImage;
  gfx::Picture image;

  void unlockAchievement(steamapi::Achievement &achievement);
};

template<class T>
struct AbstractStat
{
  T id;
  std::string name;
  int count;

  AbstractStat() : id( T(-1) ), count( 0 ) {}

  AbstractStat( T vId, const char* vName, int vCount )
  {
    id = vId;
    name = vName;
    count = vCount;
  }
};

typedef AbstractStat<MissionName> MissionInfo;
typedef AbstractStat<StatName> StatInfo;

struct Stat
{
};

struct XClient
{
  ISteamUser* user;
  ISteamUserStats* stats;
  ISteamApps* apps;
} xclient;

Achievement glbAchievements[achv_count] =
{
  _ACH_ID( achievementFirstWin     ),
  _ACH_ID( achievementNewGraphics  ),
  _ACH_ID( achievementNewVillage   )
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

  MissionInfo missions[nx_count];
  StatInfo    stats[ stat_count];

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
    statsUpdate = false;
    needStoreStats = false;
    statsValid = false;

    #define _INIT_MISSION( id ) missions[ id ] = MissionInfo( id, #id, 0 );

    _INIT_MISSION( n2_nvillage )

    #undef _INIT_MISSION

    #define _INIT_STAT( id ) stats[ id ] = StatInfo(id, #id, 0 );

    _INIT_STAT( stat_num_games )
    _INIT_STAT( stat_num_wins  )
    _INIT_STAT( stat_num_lose  )

    #undef _INIT_STAT
  }    

  void unlockAchievement( Achievement &achievement );
  void clearAchievement( Achievement &achievement );
  void evaluateAchievement( Achievement& achievement );
  void checkMissions( const std::string& name );
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
  sth_setAchievement( achievement.steamName );
#else
  if( xclient.stats )
  {
    xclient.stats->SetAchievement( achievement.steamName );
  }
#endif

  // Store stats end of frame
  needStoreStats = true;
}

void UserStats::checkMissions( const std::string& name )
{
  for( int k=0;k < nx_count; k++ )
  {
    if( name == missions[ k ].name )
    {
      missions[ k ].count++;
      break;
    }
  }
}

//-----------------------------------------------------------------------------
// Purpose: Unlock this achievement
//-----------------------------------------------------------------------------
void UserStats::clearAchievement( Achievement &achievement )
{
  achievement.reached = false;

  // the icon may change once it's unlocked
  achievement.idIconImage = 0;

  // mark it down
#ifdef  CAESARIA_PLATFORM_WIN
  //sth_clearAchievement( achievement.uniqueName );
#else
  if( xclient.stats )
  {
    xclient.stats->ClearAchievement( achievement.steamName );
  }
#endif

  // Store stats end of frame
  needStoreStats = true;
}

std::string language()
{
#ifdef CAESARIA_PLATFORM_WIN
  return "";
#else
  std::string lang = SteamUtils()->GetSteamUILanguage();

  if( lang == "english" ) lang = "en";
  else if( lang == "russian" ) lang = "ru";
  else if( lang == "czech" ) lang = "cz";
  else if( lang == "finnish" ) lang = "fn";
  else if( lang == "french" ) lang = "fr";
  else if( lang == "german" ) lang = "de";
  else if( lang == "italian" ) lang = "it";
  else if( lang == "polish" ) lang = "pl";
  else if( lang == "spanish" ) lang = "sp";
  else if( lang == "swedish" ) lang = "sv";
  else if( lang == "ukranian" ) lang = "ua";
  else lang = "";

  return lang;
#endif
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
  case achievementNewVillage:
      if(missions[n2_nvillage].count>0){unlockAchievement(achievement);}
  break;

  case achievementFirstWin:
      if(stat_num_wins>0){ unlockAchievement(achievement);};
  break;

  case achievementNewGraphics:
  {
    bool haveDlc = false;
#ifdef CAESARIA_PLATFORM_WIN
    haveDlc = sth_isDlcInstalled( CAESARIA_AVEC3_APPID );
#else
    if( xclient.apps )
      haveDlc = xclient.apps->BIsDlcInstalled( CAESARIA_AVEC3_APPID );
#endif
    if( haveDlc ) unlockAchievement( achievement );
    else clearAchievement( achievement );
  }
  break;

  default:
  break;
  }
}

void UserStats::storeStatsIfNecessary()
{
#ifdef CAESARIA_PLATFORM_WIN
  if( needStoreStats )
  {
    // already set any achievements in UnlockAchievement

    // set stats
    for( int i=0; i < stat_count; i++ )
    {
      StatInfo& stat = stats[ i ];
      sth_SetStat( stat.name.c_str(), stat.count );
    }
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
    for( int i=0; i < stat_count; i++ )
    {
      StatInfo& stat = stats[ i ];
      xclient.stats->SetStat( stat.name.c_str(), stat.count );
    }
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
  gameRunInOfflineMode = !SteamUser()->BLoggedOn();

  bool mayStart = SteamApps()->BIsSubscribedApp( CAESARIA_STEAM_APPID );
  if( !mayStart )
  {
    Logger::warning( "Cant play in this account" );
    OSystem::error( "Warning", "Cant play in this account" );
    return false;
  }

  if( gameRunInOfflineMode )
  {
    Logger::warning( "Game work in offline mode" );
    OSystem::error( "Warning", "Game work in offline mode" );
  }  

  return true;
}

void close()
{  
  SteamAPI_Shutdown();
  Logger::warning( "Game: try close steam" );
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
  xclient.apps = SteamApps();

#if defined(CAESARIA_PLATFORM_WIN) && defined(__GNUC__)
  xclient.user = 0;
  xclient.stats = 0;
  xclient.apps = 0;
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

  evaluateAchievement( achievementNewGraphics );
}

void evaluateAchievements()
{
  // Evaluate achievements
  for( int iAch = 0; iAch < achv_count; ++iAch )
  {
    glbUserStats.evaluateAchievement( glbAchievements[iAch] );
  }

  // Store stats
  glbUserStats.storeStatsIfNecessary();
}

void unlockAchievement(AchievementType achivId)
{
  if( achivId >=0 && achivId < achv_count )
  {
    if( !glbAchievements[ achivId ].reached )
      glbUserStats.unlockAchievement( glbAchievements[ achivId ] );
  }
  else
  {
    Logger::warning( "Unknown achievement ID:%d", achivId );
  }
}

gfx::Picture achievementImage(AchievementType achivId)
{
  if( achivId >=0 && achivId < achv_count )
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

gfx::Picture getSteamImage( int id )
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

      return gfx::Picture( newSize, avatarRGBA.data() );
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
#endif


//-----------------------------------------------------------------------------
// Purpose: We have stats data from Steam. It is authoritative, so update
//			our data with those results now.
//-----------------------------------------------------------------------------
#ifdef CAESARIA_PLATFORM_WIN
void UserStats::receivedUserStats()
{
  if( sth_isStatsAvailable() )
  {
    statsValid = true;

    // load achievements
    for( int iAch = 0; iAch < achv_count; ++iAch )
    {
      Achievement &ach = glbAchievements[iAch];
      ach.reached = sth_getAchievementReached( ach.steamName );
      sth_getAchievementAttribute( ach.steamName, "name", ach.caption );
      sth_getAchievementAttribute( ach.steamName, "desc", ach.description );

      ach.idIconImage = sth_getAchievementIcon( ach.steamName );
      ach.image = getSteamImage( ach.idIconImage );
    }

    // load stats
    for( int index=0; index < nx_count; index++ )
    {
      MissionInfo& mission = missions[ index ];
      mission.count = sth_getStat( mission.name.c_str() );
    }

    // load stats
    for( int index=0; index < stat_count; index++ )
    {
      StatInfo& stat = stats[ index ];
      stat.count = sth_getStat( stat.name.c_str() );
    }
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
      for( int iAch = 0; iAch < achv_count; ++iAch )
      {
        Achievement& ach = glbAchievements[iAch];
        steamUserStats->GetAchievement( ach.steamName, &ach.reached );
        sprintf( ach.caption, "%s", steamUserStats->GetAchievementDisplayAttribute( ach.steamName, "name" ) );
        sprintf( ach.description, "%s", steamUserStats->GetAchievementDisplayAttribute( ach.steamName, "desc" ) );

        ach.idIconImage = steamUserStats->GetAchievementIcon( ach.steamName );
        ach.image = getSteamImage( ach.idIconImage );
      }

      for( int index=0; index < nx_count; index++ )
      {
        MissionInfo& mission = missions[ index ];
        steamUserStats->GetStat( mission.name.c_str(), &mission.count );
      }

      // load stats
      for( int index=0; index < stat_count; index++ )
      {
        StatInfo& stat = stats[ index ];
        steamUserStats->GetStat( stat.name.c_str(), &stat.count );
      }
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
  if( achivId >=0 && achivId < achv_count )
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
  if( achivId >=0 && achivId < achv_count )
  {
    return glbAchievements[ achivId ].reached;
  }
  else
  {
    Logger::warning( "Unknown achievement ID:%d", achivId );    
  }
  return false;
}

void missionWin( const std::string& name )
{
  glbUserStats.stats[ stat_num_wins ].count++;
  glbUserStats.stats[ stat_num_games ].count++;
  glbUserStats.checkMissions( name );
  evaluateAchievements();
}

void evaluateAchievement( AchievementType achivId )
{
  glbUserStats.evaluateAchievement( glbAchievements[achivId] );
  // Store stats
  glbUserStats.storeStatsIfNecessary();
}

bool available() { return true; }

#else

bool available() { return false; }
bool checkSteamRunning() { return true; }
bool connect() { return true; }
void close() {}
bool isAchievementReached(steamapi::AchievementType) { return true; }
gfx::Picture achievementImage(steamapi::AchievementType) { return gfx::Picture(); }
void init() {}
std::string userName(){ return ""; }
void update(){}
std::string achievementCaption(AchievementType achivId) { return ""; }
const gfx::Picture&userImage() { return gfx::Picture::getInvalid(); }
bool isStatsReceived() { return false; }

#endif //CAESARIA_USE_STEAM
}
