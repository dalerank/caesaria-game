#ifndef __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_
#define __OPENCAESAR3_INFOBOX_MANAGER_H_INCLUDE_

#include "oc3_smartptr.h"
#include "oc3_referencecounted.h"
#include "oc3_scopedptr.h"

class GuiEnv;
class Tile;

class InfoBoxManager;
typedef SmartPtr< InfoBoxManager > InfoBoxManagerPtr;

class InfoBoxManager : public ReferenceCounted
{
public:
    static InfoBoxManagerPtr create( GuiEnv* gui );

    void showHelp( Tile* tile ); 
private:
    InfoBoxManager();
    ~InfoBoxManager();

    class Impl;
    ScopedPtr< Impl > _d;
};


#endif