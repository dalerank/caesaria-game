#ifndef __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_
#define __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_

#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "enums.hpp"
#include "gui/info_box.hpp"

class InfoBoxManager;
typedef SmartPtr< InfoBoxManager > InfoBoxManagerPtr;

class InfoboxCreator
{
public:
  virtual gui::InfoBoxSimple* create( gui::Widget*, const Tile& ) = 0;
};

class InfoBoxManager : public ReferenceCounted
{
public:
  static InfoBoxManagerPtr create( CityPtr city, gui::GuiEnv* gui );

  void showHelp( const Tile& tile ); 
  void setShowDebugInfo( const bool showInfo );

  void addInfobox( const TileOverlayType type, const std::string& typeName, InfoboxCreator* ctor );
  bool canCreate( const TileOverlayType type ) const;
private:
  InfoBoxManager( CityPtr city, gui::GuiEnv* gui );
  ~InfoBoxManager();
   
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
