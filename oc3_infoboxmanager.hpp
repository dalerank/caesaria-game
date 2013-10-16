#ifndef __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_
#define __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_

#include "oc3_smartptr.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_enums.hpp"

class InfoBoxSimple;

class InfoBoxManager;
typedef SmartPtr< InfoBoxManager > InfoBoxManagerPtr;

class InfoboxCreator
{
public:
  virtual InfoBoxSimple* create( Widget*, const Tile& ) = 0;
};

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  InfoBoxSimple* create( Widget* parent, const Tile& tile )
  {
    return new T( parent, tile ); 
  }
};

class InfoBoxManager : public ReferenceCounted
{
public:
  static InfoBoxManagerPtr create( CityPtr city, GuiEnv* gui );

  void showHelp( const Tile& tile ); 
  void setShowDebugInfo( const bool showInfo );

  void addInfobox( const LandOverlayType type, const std::string& typeName, InfoboxCreator* ctor );
  bool canCreate( const LandOverlayType type ) const;
private:
  InfoBoxManager( CityPtr city, GuiEnv* gui );
  ~InfoBoxManager();
   
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif
