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

#ifndef STEAMHELPER_H
#define STEAMHELPER_H

#if defined( SHELPER_API_EXPORTS )
#define SHELPER_API extern "C" __declspec( dllexport ) 
#else
#define SHELPER_API extern "C" __declspec( dllimport ) 
#endif // STEAM_API_EXPORTS

extern "C"
{
SHELPER_API uint64 __cdecl sth_getSteamID();
SHELPER_API void __cdecl   sth_requestCurrentStats();
SHELPER_API int __cdecl    sth_getMediumFriendAvatar(uint64 id);
SHELPER_API void __cdecl   sth_runCallbacks();
SHELPER_API bool __cdecl   sth_isStatsAvailable();
SHELPER_API bool __cdecl   sth_getAchievementReached(const char* name);
SHELPER_API void __cdecl   sth_getAchievementAttribute(const  char* achName, const char* attrName, char* outputName);
SHELPER_API int __cdecl    sth_getAchievementIcon(const char* achName);
SHELPER_API int __cdecl    sth_getStat(const char* statName);
SHELPER_API bool __cdecl   sth_setAchievement(const char* achName);
SHELPER_API void __cdecl   sth_SetStat(const char* statName, int value);
SHELPER_API bool __cdecl   sth_StoreStats();
}

#endif