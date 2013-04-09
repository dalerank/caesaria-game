#ifndef __OPENCAESAR_BUILDMENU_H_INCLUDE_
#define __OPENCAESAR_BUILDMENU_H_INCLUDE_

#include "oc3_widget.h"
#include "enums.hpp"

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

    // returns true if widget needs to be deleted
    bool isDeleted() const;
    void setDeleted();

protected:
    bool _isDeleted;  // true if needs to be deleted

    PushButton* _hoverButton;
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
    void addButtons();
};


class BuildMenu_education : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_health : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_engineering : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_administration : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_entertainment : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_commerce : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_farm : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_raw_factory : public BuildMenu
{
public:
    void addButtons();
};


class BuildMenu_factory : public BuildMenu
{
public:
    void addButtons();
};

class BuildMenu_religion: public BuildMenu
{
public:
    void addButtons();
};

class BuildMenu_temple : public BuildMenu
{
public:
    void addButtons();
};

class BuildMenu_bigtemple : public BuildMenu
{
public:
    void addButtons();
};


#endif //__OPENCAESAR_BUILDMENU_H_INCLUDE_