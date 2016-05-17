sim.ui.dialogs.citySettings = {}

sim.ui.dialogs.citySettings.getop = function (obj) {
    engine.log("get" + obj.group + " " + obj.flag);
    switch (obj.group) {
      case 'city':
        return g_city.getOption(obj.flag);
      case 'game':
        return g_session.getAdvflag(obj.flag);
      case 'engine':
        return engine.getOption(obj.flag);
      case 'build':
        return g_city.getBuildOption(obj.flag);
      case 'gui':
        return sim.ui.dialogs.citySettings.getguiv(obj.flag);
      case 'risks':
        return g_city.getOption(obj.flag);
    }
    return 0;
};

sim.ui.dialogs.citySettings.setop = function(obj, value) {
    engine.log("set " + obj.group + " " + obj.flag);
    switch (obj.group) {
      case "city":
        g_city.setOption(obj.flag, value);
        break;
      case "engine":
        engine.setOption(obj.flag, value);
        break;
      case "game":
        g_session.setAdvflag(obj.flag, value);
        break;
      case "build":
        g_city.setBuildOption(obj.flag, value);
        break;
      case "gui":
        sim.ui.dialogs.citySettings.setguiv(obj.flag, value);
      break;
      case "risks":
        g_city.setOption(obj.flag, value);
      break;
    }
};

sim.ui.dialogs.citySettings.getguiv = function(name) {
    engine.log("get guiopts " + name);
    if (name == "debug_menu") {
      return sim.ui.topmenu.debugmenu.display;
    } else if (name == "andr_menu") {
      var dm = new Widget("AndroidActionsBar")
      return dm.display;
    } else if (name=="rightMenu") {
      return engine.getOption("rightMenu")
    }
};

sim.ui.dialogs.citySettings.setguiv = function(name, value) {
    engine.log("set guiopts " + name);
    if (name == "debug_menu") {
      var dm = sim.ui.topmenu.debugmenu
      dm.display = value;
    } else if (name == "andr_menu") {
      var dm = new Widget("AndroidActionsBar")
      dm.display = value;
    } else if (name=="rightMenu") {
      engine.setOption("rightMenu",value);
      sim.ui.menu.setSide(value);
    }
};

sim.ui.dialogs.citySettings.next = function(obj) {
    var value = {}
    if (obj.group === "risks") {
      value = g_city.getOption(obj.flag)
      value += 10;
      if (value > 100)
        value = 0;
    } else if (obj.group === "gui") {
      value = sim.ui.dialogs.citySettings.getop(obj)
      return value ? 0 : 1;
    } else {
      value = sim.ui.dialogs.citySettings.getop(obj)
      value += 1;
      if (value >= obj.states.length)
        value = 0;
    }

    return value;
};

sim.ui.dialogs.citySettings.text = function(obj) {
    var value = {};
    if (obj.group === "risks") {
      value = g_city.getOption(obj.flag);
      var lb = _t("##" + obj.base + "##");
      return _format("{0} {1} %", lb, value);
    } else if (obj.group === "gui") {
      var value = sim.ui.dialogs.citySettings.getguiv(obj.flag);
      var tx = _format("##{0}_{1}##", obj.base, obj.states[value]);
      return _t(tx);
    } else {
      value = sim.ui.dialogs.citySettings.getop(obj)
      var tx = _format("##{0}_{1}##", obj.base, obj.states[value]);
      return _t(tx);
    }
};

sim.ui.dialogs.citySettings.tooltip = function(obj) {
    if (obj.group === "risks") {
      return "";
    } else if (obj.group === "gui") {
      return "";
    } else {
      var value = sim.ui.dialogs.citySettings.getop(obj)
      return _t("##" + obj.base + "_" + obj.states[value] + "_tlp##");
    }
};

sim.ui.dialogs.citySettings.config = [
{    base: "city_opts_god",    states: ["off", "on"],    group: "city",    flag: "godEnabled"  },
{    base: "city_warnings",    states: ["off", "on"],    group: "city",    flag: "warningsEnabled"  },
{    base: "city_zoom",    states: ["off", "on"],    group: "city",    flag: "zoomEnabled"  },
{    base: "city_zoominv",    states: ["off", "on"],    group: "city",    flag: "zoomInvert"  },
{    base: "city_barbarian",    states: ["off", "on"],    group: "city",    flag: "barbarianAttack"  },
{    base: "city_tooltips",    states: ["off", "on"],    group: "game",    flag: "tooltips"  },
{    base: "city_buildoverdraw",    states: ["off", "on"],    group: "city",    flag: "showGodsUnhappyWarn"  },
{    base: "city_warf_timber",    states: ["off", "on"],    group: "city",    flag: "warfNeedTimber"  },
{    base: "river_side_well",    states: ["off", "on"],    group: "city",    flag: "riversideAsWell"  },
{    base: "sldr_wages",    states: ["off", "on"],    group: "city",    flag: "soldiersHaveSalary"  },
{    base: "personal_tax",    states: ["off", "on"],    group: "city",    flag: "housePersonalTaxes"  },
{    base: "cut_forest",    states: ["off", "on"],    group: "city",    flag: "cutForest2timber"  },
{    base: "rightMenu",    states: ["off", "on"],    group: "gui",    flag: "rightMenu"  },
{    base: "city_mapmoving",    states: ["lmb", "mmb"],    group: "game",    flag: "mmbMoving"  },
{    base: "city_chastener",    states: ["off", "on"],    group: "city",    flag: "legionAttack"  },
{    base: "house_avoid_plague",    states: ["off", "on"],    group: "city",    flag: "houseAvoidPlague"  },
/* {    base: "city_androidbar",    states: ["off", "on"],    group: "game",    flag: "showTabletMenu"  },
{    base: "city_ccuseai",    states: ["off", "on"],    group: "game",    flag: "ccUseAI"  },*/
{    base: "city_highlight_bld",    states: ["off", "on"],    group: "city",    flag: "highlightBuilding"  },
{    base: "city_destroy_epdh",    states: ["off", "on"],    group: "city",    flag: "destroyEpidemicHouses"  },
{    base: "city_border",    states: ["off", "on"],    group: "game",    flag: "borderMoving"  },
{    base: "city_forest_fire",    states: ["off", "on"],    group: "city",    flag: "forestFire"  },
{    base: "city_forest_grow",    states: ["off", "on"],    group: "city",    flag: "forestGrow"  },
{    base: "city_claypit_collapse",    states: ["off", "on"],    group: "city",    flag: "claypitMayFloods"  },
{    base: "city_mines_collapse",    states: ["off", "on"],    group: "city",    flag: "minesMayCollapse"  },
{    base: "draw_svk_border",    states: ["off", "on"],    group: "city",    flag: "svkBorderEnabled"  },
{    base: "city_farm_use_meadow",    states: ["off", "on"],    group: "city",    flag: "farmUseMeadows"  },
{    base: "city_fire_risk",    group: "risks",    flag: "fireKoeff"  },
{    base: "city_collapse_risk",    group: "risks",    flag: "collapseKoeff"  },
{    base: "city_df",    states: ["fun", "easy", "simple", "usual", "nicety", "hard", "impossible"],    group: "city",    flag: "difficulty"  },
{    base: "city_batching",    states: ["off", "on"],    group: "game",    flag: "batching"  },
{    base: "city_lockinfo",    states: ["off", "on"],    group: "engine",    flag: "lockInfobox"  },
{    base: "city_roadblock",    states: ["off", "on"],    group: "build",    flag: "roadBlock"  },
{    base: "city_debug",    states: ["off", "on"],    group: "gui",    flag: "debug_menu"  },
{    base: "city_metric",    states: ["quantity", "kilogram", "modius"],    group: "game",    flag: "metric"  }
];

sim.ui.dialogs.citySettings.switch = function(lbx, index) {
  var obj = sim.ui.dialogs.citySettings.config[index];
  var value = sim.ui.dialogs.citySettings.next(obj)
  var states = obj.states;

  sim.ui.dialogs.citySettings.setop(obj, value);

  lbx.setItemText(index, sim.ui.dialogs.citySettings.text(obj));
  lbx.setItemTooltip(index, sim.ui.dialogs.citySettings.tooltip(obj));
}

sim.ui.dialogs.citySettings.show = function() {
  var w = g_ui.addWindow(0, 0, 480, 540);
  w.closeAfterKey({
    escape: true,
    rmb: true
  })
  w.title = _u("city_options");

  var lbHelp = w.addLabel(15, w.h - 40, w.w - 80, 20);
  lbHelp.text = _u("dblclick_to_change_option");
  lbHelp.font = "FONT_1";

  var lbxModes = w.addListbox(25, 40, w.w - 50, w.h - 90);
  lbxModes.setTextAlignment("center", "center");
  lbxModes.background = true;

  for (var i in sim.ui.dialogs.citySettings.config) {
    var obj = sim.ui.dialogs.citySettings.config[i];
    engine.log(obj.base);
    var index = lbxModes.addLine(sim.ui.dialogs.citySettings.text(obj));
    lbxModes.setItemTooltip(index, sim.ui.dialogs.citySettings.tooltip(obj));
  }

  lbxModes.onDblclickCallback = function(index) { sim.ui.dialogs.citySettings.switch(lbxModes,index) };

  w.addExitButton(w.w - 34, w.h - 34)
  w.moveToCenter()
  w.setModal()
  w.mayMove = false
}
