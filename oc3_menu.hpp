
#ifndef __OPENCAESAR3_MENU_H_INCLUDE_
#define __OPENCAESAR3_MENU_H_INCLUDE_

#include "oc3_widget.hpp"
#include "oc3_signals.hpp"
#include "oc3_scopedptr.hpp"

class PushButton;
class GuiTilemap;

class Menu : public Widget
{
public:
    static Menu* create( Widget* parent, int id );

    // draw on screen
    void draw( GfxEngine& engine );

    bool onEvent(const NEvent& event);

    bool unselectAll();

oc3_signals public:
    Signal1< int >& onCreateConstruction();
    Signal0<>& onRemoveTool();
    Signal0<>& onMaximize();

protected:
    class Impl;
    ScopedPtr< Impl > _d;

    Menu( Widget* parent, int id, const Rect& rectangle );
    void _createBuildMenu( int type, Widget* parent );
    PushButton* _addButton( int startPic, bool pushBtn, int yMul, 
                            int id, bool haveSubmenu, int midPic, 
                            const std::string& tooltip="" );
};

class ExtentMenu : public Menu
{
public:
    static ExtentMenu* create( Widget* parent, GuiTilemap& tilemap, int id );

    void minimize();
    void maximize();

    bool onEvent(const NEvent& event);

    void draw( GfxEngine& engine );

    void toggleOverlays();

oc3_signals public:
    Signal1<int>& onSelectOverlayType();
    Signal0<>& onEmpireMapShow();
    Signal0<>& onAdvisorsWindowShow();

protected:
    ExtentMenu( Widget* parent, GuiTilemap&, int id, const Rect& rectangle );
    GuiTilemap& _tmap;
};

#endif