#ifndef __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_
#define __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_

#include "oc3_smartptr.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_enums.hpp"

class GuiInfoBox;

class InfoBoxManager;
typedef SmartPtr< InfoBoxManager > InfoBoxManagerPtr;

class InfoboxCreator
{
public:
  virtual GuiInfoBox* create( Widget*, const Tile& ) = 0;
};

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  GuiInfoBox* create( Widget* parent, const Tile& tile ) 
  {
    return new T( parent, tile ); 
  }
};

class InfoBoxManager : public ReferenceCounted
{
public:
  static InfoBoxManagerPtr create( GuiEnv* gui );

  void showHelp( const Tile& tile ); 
  void setShowDebugInfo( const bool showInfo );

  void addCreator( const BuildingType type, const std::string& typeName, InfoboxCreator* ctor );
  bool canCreate( const BuildingType type ) const;
private:
  InfoBoxManager();
  ~InfoBoxManager();
   
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif
