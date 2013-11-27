#ifndef _CAESARIA_PACKAGER_H_INCLUDE_
#define _CAESARIA_PACKAGER_H_INCLUDE_

#include <string>

class Packager
{
public:
  Packager( std::string baseset, std::string currentVersion );

  void createUpdate(bool release);

private:
  std::string _baseset;
  std::string _crver;
};

#endif //_CAESARIA_PACKAGER_H_INCLUDE_
