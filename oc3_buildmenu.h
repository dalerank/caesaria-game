#ifndef __OPENCAESAR_BUILDMENU_H_INCLUDE_
#define __OPENCAESAR_BUILDMENU_H_INCLUDE_

#include "oc3_widget.h"
#include "enums.hpp"
#include "oc3_signals.h"

class PushButton;

class BuildMenu : public Widget
{
public:
    BuildMenu( Widget* parent, const Rect& rectangle, int id );
    virtual void addButtons() = 0;
    virtual ~BuildMenu();
    void init();

    static BuildMenu* getMenuInstance(const BuildMenuType menuType, Widget* parent);

    // add the subMenu in the menu.
    void addSubmenuButton(const BuildMenuType menuType, const std::string &text);
    // add the button in the menu.
    void addBuildButton(const BuildingType buildingType);

    bool onEvent(const NEvent& event);

oc3_signals public:
    Signal2< int, Widget* >& onCreateBuildMenu();
    Signal1< int >& onCreateConstruction();

private:
    class Impl;
    std::auto_ptr< Impl > _d;
};

class BuildMenu_water : public BuildMenu
{
public:
    BuildMenu_water( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_security : public BuildMenu
{
public:
    BuildMenu_security( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_education : public BuildMenu
{
public:
    BuildMenu_education( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_health : public BuildMenu
{
public:
    BuildMenu_health( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_engineering : public BuildMenu
{
public:
    BuildMenu_engineering( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_administration : public BuildMenu
{
public:
    BuildMenu_administration( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_entertainment : public BuildMenu
{
public:
    BuildMenu_entertainment( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_commerce : public BuildMenu
{
public:
    BuildMenu_commerce( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_farm : public BuildMenu
{
public:
    BuildMenu_farm( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_raw_factory : public BuildMenu
{
public:
    BuildMenu_raw_factory( Widget* parent, const Rect& rectangle );
    void addButtons();
};


class BuildMenu_factory : public BuildMenu
{
public:
    BuildMenu_factory( Widget* parent, const Rect& rectangle );
    void addButtons();
};

class BuildMenu_religion: public BuildMenu
{
public:
    BuildMenu_religion( Widget* parent, const Rect& rectangle );
    void addButtons();
};

class BuildMenu_temple : public BuildMenu
{
public:
    BuildMenu_temple( Widget* parent, const Rect& rectangle );
    void addButtons();
};

class BuildMenu_bigtemple : public BuildMenu
{
public:
    BuildMenu_bigtemple( Widget* parent, const Rect& rectangle );
    void addButtons();
};


#endif //__OPENCAESAR_BUILDMENU_H_INCLUDE_