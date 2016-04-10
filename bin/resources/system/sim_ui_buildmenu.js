function OnShowBuildMenu(type, top) {
  sim.ui.buildmenu.show(type, top);
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

sim.ui.buildmenu.build = function(type) {
  var sound = "bmsel_" + type;
  g_session.playAudio(sound + "_00001", 100, g_config.audio.infobox);
  g_session.setOption("buildMode", type);
  sim.ui.buildmenu.hide();
}

sim.ui.buildmenu.sub = function(type) {
  var sound = "bmsel_" + type;
  g_session.playAudio(sound + "_00001", 100, g_config.audio.infobox);
  sim.ui.buildmenu.show(branch);
}

sim.ui.buildmenu.hide = function() {
  var menus = g_ui.find("BuildMenu");
  if (menus != null)
    menus.deleteLater();
}

sim.ui.buildmenu.show = function(type, top) {
  sim.ui.buildmenu.hide();
  engine.log(top);

  if (type == undefined || type == "")
    return;

  var extMenu = g_ui.find("ExtentMenu");
  //var parent = g_ui.find(parentName);
  var buildMenu = new Widget(0);

  if (buildMenu != null) {
    buildMenu.clipped = false;
    buildMenu.name = "BuildMenu";
    buildMenu.buttons = [];
    buildMenu.htop = top;
    buildMenu.subElement = true;
    var resolution = g_session.resolution;
    buildMenu.geometry = {x:0, y:0, w:resolution.w - extMenu.w, h:extMenu.h};

    var wcloser = new WidgetClosers(buildMenu);
    wcloser.addCloseCode(g_config.key.KEY_RBUTTON);
    wcloser.addCloseCode(g_config.key.KEY_ESCAPE);
    //buildMenu.geometry = { x:extMenu.left()-60, y:parent.top(), w:60, h:60 };
    //buildMenu.geometry = { x:0, y:0, w:80, h:25 };
  }

  buildMenu.addButton = function(branch,type) {
    var ltype = branch.length ? branch : type;
    var title = _format( "bldm_{0}", ltype);

    if (type != "" && !g_session.city.getBuildOption(type))
      return;

    engine.log("Build menu add type " + ltype);
    title = _ut(title);
    if (title[0] == "#")
      title = ltype;

    var btn = new Button(buildMenu);
    btn.geometry = { x:0, y:25*buildMenu.buttons.length, w:buildMenu.w, h:24};
    btn.hoverFont = "FONT_2_RED";
    btn.text = _u(title);
    btn.textAlign = { h:"upperLeft", v:"center" };
    btn.textOffset = { x:15, y:0 };

    if (type != "")  {
      var cost = g_config.construction[type].cost;
      btn.lbCost = new Label(btn);
      btn.lbCost.font = "FONT_2_RED";
      btn.lbCost.textAlign = { h:"lowerRight", v:"center" };
      btn.lbCost.text = cost;
    }

    btn.btnHelp = new HelpButton(btn);
    btn.btnHelp.position = { x:btn.w-24, y:0 };
    btn.btnHelp.uri = ltype;
    btn.btnHelp.align = { left:g_config.align.lowerRight, top:g_config.align.lowerRight,
                          right:g_config.align.lowerRight, bottom:g_config.align.lowerRight };

    btn.sound = _format( "bmsel_{0}", ltype );
    buildMenu.buttons.push(btn);

    //buildMenu.h = buildMenu.buttons.length * 25;

    if (branch != "") btn.callback = function() { sim.ui.buildmenu.sub(branch); }
    else if (type != "") btn.callback = function() { sim.ui.buildmenu.build(type); }
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
    engine.log( "Opened buildmenu " + type);
    var config = g_config.buildmenu[type];

    if (config.submenu != undefined) {
      for (var i in config.submenu)
        buildMenu.addButton(config.submenu[i], "");
    }

    if (config.buildings != undefined) {
      for (var i in config.buildings) {
        buildMenu.addButton("", config.buildings[i]);
      }
    }

    for (var i in buildMenu.buttons) {
      var bbutton = buildMenu.buttons[i];
      max_text_width = math.max(max_text_width, bbutton.textWidth );

      if (bbutton.lbCost)
        max_cost_width = math.max(max_cost_width, bbutton.lbCost.textWidth);
    }

    //engine.log(max_text_width);
    var appear_width = math.max(150, max_text_width + max_cost_width ) + 80;

    // set the same size for all buttons
    for(var i in buildMenu.buttons) {
      var btn = buildMenu.buttons[i];
      btn.w = appear_width;
      btn.x = buildMenu.w - btn.w - 35;
      btn.y = buildMenu.htop + 25 * i;
      if (btn.lbCost)
        btn.lbCost.geometry = { x:btn.w-55, y:0, w:30, h:25 };
    }
    //  button->setWidth( width() );
  }

  if (type != "")
    buildMenu.update(type);
}
