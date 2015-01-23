#ifndef __CAESARIA_ADDON_REQUIREMENTS_INCLUDE_HPP__
#define __CAESARIA_ADDON_REQUIREMENTS_INCLUDE_HPP__

#ifdef CAESARIA_PLATFORM_WIN
  #ifdef CAESARIA_ADDON_DEFINED
    #define  ADDON_EXPORT __declspec(dllexport)
  #else
    #define  ADDON_EXPORT __declspec(dllimport)
  #endif 
#else 
 #define ADDON_EXPORT
#endif

const unsigned int ADDON_API_VERSION = 0x1001;

struct GameBridge
{

};

struct GameInfo
{

};

#endif // __CAESARIA_ADDON_REQUIREMENTS_INCLUDE_HPP__
