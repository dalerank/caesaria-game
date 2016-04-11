sim.ui.menu.config = {
  housing : { pos: {x:13,y:277}, image: { rc:"paneling", index:123 }, object:"house",
               miniature: { rc:"panelwindows", index:1 },
               pushButton: true, submenu: false, sound:"housing",
               ingame : true, ineditor : false },
  clearLand : { pos: {x:63,y:277}, image : { rc:"paneling", index:131 }, object:"removeTool",
                miniature: { rc:"panelwindows", index:12 },
                pushButton : true, submenu: false, sound: "clear_land",
                ingame : true, ineditor : false },

  road : { pos : {x:113,y:277}, image : { rc:"paneling", index:135 }, object: "road",
           miniature : { rc:"panelwindows", index:11 },
           pushButton : true, submenu: false, sound: "road",
           ingame : true, ineditor : false },

  water: { pos:{x:13,y:313}, image : { rc:"paneling", index:127 }, object:"water",
           miniature : { rc:"panelwindows", index:4 },
           pushButton : true, submenu: false, sound: "water",
           ingame : true, ineditor : false },

  health: { pos:{x:63,y:313}, image : { rc:"paneling", index:163 }, object:"health",
            miniature : { rc:"panelwindows", index:6 },pushButton : true,
            submenu: false, sound: "health",
            ingame : true, ineditor : false },

  religion: { pos:{x:113,y:313}, image : { rc:"paneling", index:151 }, object:"religion",
              miniature : { rc:"panelwindows", index:2 },
              pushButton : true, submenu: false, sound: "temples",
              ingame : true, ineditor : false },

  education: { pos:{x:13,y:349}, image : { rc:"paneling", index:147 }, object:"education",
               miniature : { rc:"panelwindows", index:7 },
               pushButton : true, submenu: false, sound: "education",
               ingame : true, ineditor : false },

  entertainment : { pos: {x:63,y:349}, image : { rc:"paneling", index:143 }, object:"entertainment",
                    miniature : { rc:"panelwindows", index:5 },
                    pushButton : true, submenu: false, sound: "entertainment",
                    ingame : true, ineditor : false },

  govt : { pos:{x:113,y:349}, image : { rc:"paneling", index:139 }, object:"administration",
           miniature : { rc:"panelwindows", index:3 },
           pushButton : true, submenu: false, sound: "administration",
           ingame : true, ineditor : false },

  engineering: { pos:{x:13,y:385}, image : { rc:"paneling", index:167 }, object:"engineering",
           miniature : { rc:"panelwindows", index:8 },
           pushButton : true, submenu: false, sound: "engineering",
           ingame : true, ineditor : false },

  security : { pos:{x:63,y:385}, image : { rc:"paneling", index:159 }, object:"security",
           miniature : { rc:"panelwindows", index:9 },
           pushButton : true, submenu: false, sound: "security",
           ingame : true, ineditor : false },

  commerce : { pos:{x:113,y:385}, image: { rc:"paneling", index:155 }, object:"commerce",
           miniature : { rc:"panelwindows", index:10 },
           pushButton : true, submenu: false, sound: "commerce",
           ingame : true, ineditor : false },

  messages : { pos:{x:63, y:421}, image: { rc:"paneling", index:115 }, object:"message",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "message",
           ingame : true, ineditor : false, height : 22 },

  disaster: { pos:{x:113, y:421}, image: { rc:"paneling", index:119 }, object:"disaster",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "troubles",
           ingame : true, ineditor : false, height : 22 },

  undo : { pos:{x:13,y:421}, image: { rc:"paneling", index:171 }, object:"cancel",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "cancel",
           ingame : true, ineditor : false, height : 22},

  minimize : { pos:{x:127,y:5}, image: { rc:"paneling", index:97 }, object:"maximize",
               miniature : { rc:"panelwindows", index:12 },
               pushButton : false, submenu: false, sound: "hide_bigpanel",
               ingame : true, ineditor : false,},

  senate : { pos:{x:7,y:155}, image: { rc:"paneling", index:79 }, object:"senate",
             miniature : { rc:"panelwindows", index:12 },
             pushButton : false, submenu: false, sound: "hide_bigpanel",
             ingame : true, ineditor : false,},

  empire : { pos:{x:84,y:155}, image: { rc:"paneling", index:82 }, object:"empire",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "empire",
            ingame : true, ineditor : false, },

  mission : { pos:{x:7,y:184}, image: { rc:"paneling", index:85 }, object:"senate",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "senate",
            ingame : true, ineditor : false, },

  reorient_map_to_north : { pos:{x:46,y:184}, image: { rc:"paneling", index:88 }, object:"reorient_map_to_north",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "reorient_map_to_north",
            ingame : true, ineditor : false, },

  rotate_map_counter_clockwise : { pos:{x:84,y:184}, image: { rc:"paneling", index:91 }, object:"rotate_map_counter_clockwise",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "rotate_map_counter_clockwise",
            ingame : true, ineditor : false, },

  rotate_map_clockwise : { pos:{x:123,y:184}, image: { rc:"paneling", index:94 }, object:"rotate_map_clockwise",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "rotate_map_clockwise",
            ingame : true, ineditor : false, },

}

sim.ui.menu.funcs = [];

sim.ui.menu.rightSide = engine.getOption("rightMenu");

sim.ui.menu.enableAlarm = function(enable) {
  if (enabled) {
    g_session.playAudio("extm_alarm_00001", 100, g_config.audio.effects);
  }

  sim.ui.menu.disaster.button.enabled = enable;
}

sim.ui.menu.config.housing.func = function() {
  sim.ui.buildmenu.hide();
  sim.ui.menu.widget.miniature.picture = sim.ui.menu.config.housing.miniature;
  g_session.setOption( "buildMode", "house");
}

sim.ui.menu.config.clearLand.func = function() {
  sim.ui.buildmenu.hide();
  sim.ui.menu.widget.miniature.picture = sim.ui.menu.config.clearLand.miniature;
  g_session.setOption( "removeTool", true );
}

sim.ui.menu.config.road.func = function() {
  sim.ui.buildmenu.hide();
  sim.ui.menu.widget.miniature.picture = sim.ui.menu.config.road.miniature;
  g_session.setOption( "buildMode", "road");
}

sim.ui.menu.showbuildmenu = function(o, y) {
  g_session.playAudio(o.sound, 100, g_config.audio.infobox);
  sim.ui.menu.widget.miniature.picture = o.miniature;
  sim.ui.buildmenu.show(o.object, y);
}

sim.ui.menu.showshortmenu = function() {}

sim.ui.menu.minimize = function() {
  var menu = sim.ui.menu.widget;

  sim.ui.buildmenu.hide();
  var stopPos = { x:menu.x - menu.w * (sim.ui.menu.rightSide ? 1 : -1 ), y:menu.y};
  var animator = g_ui.addPosAnimator(menu, stopPos, 300, sim.ui.menu.config.showshortmenu);
  animator.setFlag("showParent", false);

  g_session.playAudio("panel_00003", 100, g_config.audio.infobox);
}

sim.ui.menu.maximize = function() {
  var menu = sim.ui.menu.widget;

  sim.ui.buildmenu.hide();
  menu.visible = true;
  var stopPos = { x:menu.x - menu.w * (sim.ui.menu.rightSide ? 1 : -1), y:menu.y};
  var animator = g_ui.addPosAnimator(menu, stopPos, 300, sim.ui.menu.config.showshortmenu);
  animator.setFlag("showParent", true);

  g_session.playAudio( "panel_00003", 100, g_config.audio.infobox );
}

sim.ui.menu.config.senate.func = function() { g_session.setOption( "advisor", g_config.advisor.employers ); }
sim.ui.menu.config.empire.func = function() {}
sim.ui.menu.config.mission.func = function() { sim.ui.dialogs.showMissionTargets(); }
sim.ui.menu.config.reorient_map_to_north.func = function() {}
sim.ui.menu.config.rotate_map_counter_clockwise.func = function() {}
sim.ui.menu.config.rotate_map_clockwise.func = function() {}

sim.ui.menu.initialize = function() {
  var menu = new Widget(0);
  menu.name = "ExtentMenu";

  var topmenu = g_ui.find("TopMenu");

  var rbody = g_render.picture("paneling", 14);

  var bgbody = g_render.picture("paneling", 17);
  var bgbottom = g_render.picture("paneling", 20);

  var resolution = g_session.resolution;

  menu.geometry = {x:resolution.w - bgbody.w - rbody.w, y:topmenu.h, w:bgbody.w, h:resolution.h};
  menu.imgBody = new Image(menu);
  menu.imgBody.picture = bgbody;
  menu.imgBody.mode = "image";

  var ty = menu.imgBody.h;
  while (ty < menu.h) {
    var img = new Image(menu);
    img.y = ty;
    img.picture = bgbottom;
    img.mode = "image";
    ty += bgbottom.h;
  }

  for (var i in sim.ui.menu.config) {
    var config = sim.ui.menu.config[i];
    var btn = new TexturedButton(menu);
    btn.position = config.pos;
    btn.tooltip = _u("extm_" + config.object + "_tlp");

    btn.states = { rc:config.image.rc, normal:config.image.index, hover:config.image.index+1,
                   pressed:config.image.index+2, disabled:config.image.index+3 };
    btn.fitToImage();

    if (config.height)
      btn.h = config.height;

    (function(c, y){
      if (config.func) sim.ui.menu.funcs[i] = c.func;
      else sim.ui.menu.funcs[i] = function(){ sim.ui.menu.showbuildmenu(c, y); }
    })(config, btn.screeny);

    btn.callback = sim.ui.menu.funcs[i];

    config.button = btn;
  }

  menu.miniature = new Image(menu);
  menu.miniature.geometry = { x:7, y:216, w:148, h:52 };
  menu.miniature.mode = "fit";
  menu.miniature.picture = { rc: "panelwindows", index:13 };

  menu.overlays = new PushButton(menu);
  menu.overlays.text = _u("ovrm_text");
  menu.overlays.style = "greyBorderLineFit";
  menu.overlays.geometry =  {x:4, y:1, w:119, h:28};
  menu.overlays.tooltip =  _u("select_city_layer");

  sim.ui.menu.widget = menu;
}

sim.ui.menu.reset = function() {
  if (sim.ui.menu.widget != null) {
    engine.log( "Find old main menu -> removed" );
    sim.ui.menu.widget.deleteLater();
    sim.ui.menu.widget = null;
    sim.ui.menu.initialize();
  }
}

sim.ui.menu.setRightSide = function(right) {
  sim.ui.menu.rightSide = right;
  sim.ui.menu.reset();
}

sim.ui.menu.reset();
