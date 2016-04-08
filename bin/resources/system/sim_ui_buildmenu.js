function OnShowBuildMenu(type, parentName) {
  sim.ui.buildmenu.show(type);
}

g_config.buildmenu = {
  water : {
    buildings : [ "fountain", "well", "aqueduct", "reservoir" ]
  },

  security : {
    buildings : [ "prefecture", "wall", "fortification", "fort_javelin", "fort_legionaries",
                  "fort_horse", "barracks", "gatehouse", "tower" ]
  },

  education : {
    buildings : [ "school", "library", "academy", "missionaryPost" ]
  },

  health : {
    buildings : [ "clinic", "barber", "baths", "hospital" ]
  },

  engineering : {
    buildings : [ "engineering_post", "low_bridge", "high_bridge", "dock",
                  "shipyard", "wharf", "triumphal_arch", "garden", "plaza", "roadBlock" ]
  },

  administration : {
    buildings : [ "forum", "senate", "governorHouse", "governorVilla",
                  "governorPalace", "statue_small", "statue_middle",
                  "statue_big" ]
  },

  entertainment : {
    buildings : [ "theater", "amphitheater", "colosseum", "hippodrome", "actorColony",
                  "gladiatorSchool", "lionsNursery", "chariotSchool" ]
  },

  farm : {
    buildings : [ "wheat_farm", "fig_farm", "olive_farm", "vinard",
                  "meat_farm", "vegetable_farm" ]
  },

  commerce : {
    buildings : [ "market", "granary", "warehouse" ],
    submenu : [ "farm", "raw_material", "factory" ]
  },

  raw_material : {
    buildings : [ "quarry", "iron_mine", "lumber_mill", "clay_pit" ]
  },

  factory : {
    buildings : [ "wine_workshop", "oil_workshop", "weapons_workshop",
                  "furniture_workshop", "pottery_workshop" ]
  },

  religion : {
    buildings : [ "oracle" ],
    submenu : [ "small_temples", "big_temples" ],
  },

  small_temples : {
    buildings : [ "small_ceres_temple", "small_neptune_temple", "small_mars_temple",
                  "small_venus_temple", "small_mercury_temple" ]
  },

  big_temples : {
    buildings : [ "big_ceres_temple", "big_neptune_temple", "big_mars_temple",
                  "big_venus_temple", "big_mercury_temple" ]
  },
}

sim.ui.buildmenu.show = function(type, parentName) {
  var menus = g_ui.find("BuildMenu");
  if (menus != null)
    menus.deleteLater();

  var extMenu = g_ui.find("ExtentMenu");
  var parent = g_ui.find(parentName);
  var buildMenu = new Widget("Widget");
  buildMenu.internalName = "BuildMenu";

  if (buildMenu != null) {
    buildMenu.setNotClipped(true);
    buildMenu.init(type);
    buildMenu.internalName = "BuildMenu";
    buildMenu.buttons = [];
    var resolution = g_session.resolution;
    buildMenu.geometry = { x:extMenu.left()-60, y:parent.top(), w:60, h:60 ) );
  }

  buildMenu.addButton = functon(branch,type) {
    var ltype = branch.length ? branch : type;
    var title = _format( "bldm_{0}", ltype);

    var btn = new Button(buildMenu);
    btn.geometry = { x:0, y:0, w:buildMenu.w, h:20};
    btn.hoverFont = "FONT_2_RED";
    btn.textAlign = { h:"upperLeft", v:"center" };
    btn.textOffset = { x:15, y:0 };
    btn.lbCost = new Label(btn);
    btn.lbCost.geometry = { x:btn.w-60, y:0, w:30, h:0 };
    btn.lbCost.align = { left:g_config.align.lowerRight, top:g_config.align.lowerRight,
                         right:g_config.align.lowerRight, bottom:g_config.align.lowerRight };

    btn.btnHelp = new HelpButton(btn);
    btn.position = { x:btn.w -24, y:0 };
    btn.btnHelp.align = { left:g_config.align.lowerRight, top:g_config.align.lowerRight,
                          right:g_config.align.lowerRight, bottom:g_config.align.lowerRight };

    btn.sound = _format( "bmsel_{0}", ltype );
    buildMenu.buttons.push(btn);

    buildMenu.h = (buildMenu.h + 30);
  }

  buildMenu.update = function(type) {
    var max_text_width = 0;
    var max_cost_width = 0;
    //Size textSize;
    //Font font = Font::create( "FONT_2" );

    //VariantMap allItems = config::load( _d->menuModel );
    //VariantMap config = allItems.get( city::development::toString( _d->branch ) ).toMap();
    //VariantList submenu = config.get( "submenu" ).toList();
    //VariantList buildings = config.get( "buildings" ).toList();

    var config = g_config.buildMenu;

    if (config.submenu != undefined) {
      for (var i in config.submenu)
        buildMenu.addButton(config.submenu[i], "");
    }

    if (config.buildings != undefined)
    {
      for (var i in config.buildings)
        buildMenu.addButton("", config.buildings[i]);
    }

    auto buildButtons = children().select<BuildButton>();
    for( auto bbutton : buildButtons )
    {
      textSize = font.getTextSize( bbutton->text());
      max_text_width = std::max(max_text_width, textSize.width() );
      textSize = font.getTextSize( utils::i2str( bbutton->cost() ) );
      max_cost_width = std::max(max_cost_width, textSize.width());
    }

    setWidth( std::max(150, max_text_width + max_cost_width + 30 + 27) );

    // set the same size for all buttons
    for( auto button : buildButtons )
      button->setWidth( width() );
  }

  buildMenu.update(type);
}
