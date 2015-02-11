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

#pragma comment( lib, "../bin/win/steam_api" )

#include "steam_api.h"
#include "helper.h"

static const int CAESARIA_STEAM_APPID = 327640;

class CallbackHolder
{
public:
    bool _userStatsAvailable;
    STEAM_CALLBACK(CallbackHolder, receivedUserStats, UserStatsReceived_t, _callbackUserStatsReceived);
    STEAM_CALLBACK(CallbackHolder, updateUserStats, UserStatsStored_t, _callbackUserStatsStored);
    STEAM_CALLBACK(CallbackHolder, updateAchievementInfo, UserAchievementStored_t, _callbackAchievementStored);

    CallbackHolder() :
        _callbackUserStatsReceived(this, &CallbackHolder::receivedUserStats),
        _callbackUserStatsStored(this, &CallbackHolder::updateUserStats),
        _callbackAchievementStored(this, &CallbackHolder::updateAchievementInfo),
        _userStatsAvailable( false )
    {}
};

void CallbackHolder::updateAchievementInfo(UserAchievementStored_t *pCallback)
{

}

void CallbackHolder::updateUserStats(UserStatsStored_t *pCallback)
{

}

void CallbackHolder::receivedUserStats(UserStatsReceived_t *pCallback)
{
    ISteamUserStats* steamUserStats = SteamUserStats();
    if (!steamUserStats)
        return;
    
    // we may get callbacks for other games' stats arriving, ignore them
    if (CAESARIA_STEAM_APPID == pCallback->m_nGameID && k_EResultOK == pCallback->m_eResult )
    {
        _userStatsAvailable = true;
    }
}

static CallbackHolder callbackHolder;

SHELPER_API bool __cdecl sth_isStatsAvailable()
{
    bool saveValue = callbackHolder._userStatsAvailable;
    if (callbackHolder._userStatsAvailable)
        callbackHolder._userStatsAvailable = false;
    return saveValue;
}

SHELPER_API void __cdecl sth_getAchievementAttribute(const  char* achName, const char* attrName, char* outputName)
{
    sprintf(outputName, "%s", SteamUserStats()->GetAchievementDisplayAttribute(achName, attrName));
}

SHELPER_API int __cdecl sth_getAchievementIcon(const char* achName)
{
    return SteamUserStats()->GetAchievementIcon( achName);
}

SHELPER_API bool __cdecl sth_getAchievementReached(const char* name)
{
    bool result;
    SteamUserStats()->GetAchievement(name, &result);
    return result;
}

SHELPER_API int __cdecl    sth_getStat(const char* statName)
{
    int result;
    SteamUserStats()->GetStat( statName, &result);
    return result;
}

SHELPER_API uint64 __cdecl sth_getSteamID()
{
  CSteamID id = SteamUser()->GetSteamID();
  return id.IsValid() ? id.ConvertToUint64() : 0;
}

SHELPER_API bool __cdecl   sth_setAchievement(const char* achName)
{
    return SteamUserStats()->SetAchievement(achName);
}

SHELPER_API void __cdecl   sth_SetStat(const char* statName, int value)
{
    SteamUserStats()->SetStat(statName, value);
}

SHELPER_API void __cdecl sth_requestCurrentStats()
{
  SteamUserStats()->RequestCurrentStats();
}

SHELPER_API bool __cdecl   sth_StoreStats()
{
    return  SteamUserStats()->StoreStats();
}

SHELPER_API int __cdecl sth_getMediumFriendAvatar(uint64 id)
{
  CSteamID steamId(id);
  return SteamFriends()->GetMediumFriendAvatar(steamId);
}

SHELPER_API void __cdecl sth_runCallbacks()
{
  SteamAPI_RunCallbacks();
}

