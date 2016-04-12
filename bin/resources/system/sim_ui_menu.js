function OnChangeSessionUndoState(enable) {
  sim.ui.menu.enableUndo(enable);
}

function OnChangeSessionAlarmState(enable) {
  sim.ui.menu.enableAlarm(enable);
}

sim.ui.menu.simConfig = {
  housing : { pos: {x:13,y:277}, image: { rc:"paneling", index:123 }, object:"house",
               miniature: { rc:"panelwindows", index:1 },
               pushButton: true, submenu: false, sound:"housing" },

  clearLand : { pos: {x:63,y:277}, image : { rc:"paneling", index:131 }, object:"removeTool",
                miniature: { rc:"panelwindows", index:12 },
                pushButton : true, submenu: false, sound: "clear_land" },

  road : { pos : {x:113,y:277}, image : { rc:"paneling", index:135 }, object: "road",
           miniature : { rc:"panelwindows", index:11 },
           pushButton : true, submenu: false, sound: "road" },

  water: { pos:{x:13,y:313}, image : { rc:"paneling", index:127 }, object:"water",
           miniature : { rc:"panelwindows", index:4 },
           pushButton : true, submenu: false, sound: "water" },

  health: { pos:{x:63,y:313}, image : { rc:"paneling", index:163 }, object:"health",
            miniature : { rc:"panelwindows", index:6 },pushButton : true,
            submenu: false, sound: "health"},

  religion: { pos:{x:113,y:313}, image : { rc:"paneling", index:151 }, object:"religion",
              miniature : { rc:"panelwindows", index:2 },
              pushButton : true, submenu: false, sound: "temples"},

  education: { pos:{x:13,y:349}, image : { rc:"paneling", index:147 }, object:"education",
               miniature : { rc:"panelwindows", index:7 },
               pushButton : true, submenu: false, sound: "education" },

  entertainment : { pos: {x:63,y:349}, image : { rc:"paneling", index:143 }, object:"entertainment",
                    miniature : { rc:"panelwindows", index:5 },
                    pushButton : true, submenu: false, sound: "entertainment" },

  govt : { pos:{x:113,y:349}, image : { rc:"paneling", index:139 }, object:"administration",
           miniature : { rc:"panelwindows", index:3 },
           pushButton : true, submenu: false, sound: "administration" },

  engineering: { pos:{x:13,y:385}, image : { rc:"paneling", index:167 }, object:"engineering",
           miniature : { rc:"panelwindows", index:8 },
           pushButton : true, submenu: false, sound: "engineering" },

  security : { pos:{x:63,y:385}, image : { rc:"paneling", index:159 }, object:"security",
           miniature : { rc:"panelwindows", index:9 },
           pushButton : true, submenu: false, sound: "security" },

  commerce : { pos:{x:113,y:385}, image: { rc:"paneling", index:155 }, object:"commerce",
           miniature : { rc:"panelwindows", index:10 },
           pushButton : true, submenu: false, sound: "commerce" },

  messages : { pos:{x:63, y:421}, image: { rc:"paneling", index:115 }, object:"message",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "message", height : 22 },

  disaster: { pos:{x:113, y:421}, image: { rc:"paneling", index:119 }, object:"disaster",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "troubles", height : 22 },

  undo : { pos:{x:13,y:421}, image: { rc:"paneling", index:171 }, object:"cancel",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "cancel", height : 22},

  minimize : { pos:{x:127,y:5}, image: { rc:"paneling", index:97 }, object:"maximize",
               miniature : { rc:"panelwindows", index:12 },
               pushButton : false, submenu: false, sound: "hide_bigpanel"},

  senate : { pos:{x:7,y:155}, image: { rc:"paneling", index:79 }, object:"senate",
             miniature : { rc:"panelwindows", index:12 },
             pushButton : false, submenu: false, sound: "hide_bigpanel"},

  empire : { pos:{x:84,y:155}, image: { rc:"paneling", index:82 }, object:"empire",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "empire" },

  mission : { pos:{x:7,y:184}, image: { rc:"paneling", index:85 }, object:"senate",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "senate" },

  reorient_map_to_north : { pos:{x:46,y:184}, image: { rc:"paneling", index:88 }, object:"reorient_map_to_north",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "reorient_map_to_north" },

  rotate_map_counter_clockwise : { pos:{x:84,y:184}, image: { rc:"paneling", index:91 }, object:"rotate_map_counter_clockwise",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "rotate_map_counter_clockwise" },

  rotate_map_clockwise : { pos:{x:123,y:184}, image: { rc:"paneling", index:94 }, object:"rotate_map_clockwise",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "rotate_map_clockwise" },
}

sim.ui.menu.buildConfig = {
  terrain : { pos: {x:13,y:277}, image: { rc:"paneling", index:183 }, object:"terrain",
               miniature: { rc:"panelwindows", index:12 },
               pushButton: true, submenu: false, sound:"terrain" },

  tree : { pos: {x:63,y:277}, image : { rc:"paneling", index:186 }, object:"tree",
                miniature: { rc:"panelwindows", index:12 },
                pushButton : true, submenu: false, sound: "tree" },

  water : { pos : {x:113,y:277}, image : { rc:"paneling", index:189 }, object: "water",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "water" },

  rock : { pos:{x:63,y:313}, image : { rc:"paneling", index:198 }, object:"rock",
            miniature : { rc:"panelwindows", index:12 },pushButton : true,
            submenu: false, sound: "rock" },

  meadow: { pos:{x:113,y:313}, image : { rc:"paneling", index:201 }, object:"meadow",
              miniature : { rc:"panelwindows", index:2 },
              pushButton : true, submenu: false, sound: "meadow" },

  plateau: { pos:{x:13,y:349}, image : { rc:"paneling", index:204 }, object:"plateau",
               miniature : { rc:"panelwindows", index:7 },
               pushButton : true, submenu: false, sound: "plateau" },

  rift : { pos: {x:63,y:349}, image : { rc:"paneling", index:192 }, object:"rift",
                    miniature : { rc:"panelwindows", index:5 },
                    pushButton : true, submenu: false, sound: "rift" },

  river : { pos:{x:113,y:349}, image : { rc:"paneling", index:189 }, object:"river",
           miniature : { rc:"panelwindows", index:3 },
           pushButton : true, submenu: false, sound: "river" },

  native_hut: { pos:{x:13,y:385}, image : { rc:"paneling", index:210 }, object:"native_hut",
           miniature : { rc:"panelwindows", index:8 },
           pushButton : true, submenu: false, sound: "native_hut" },

  road : { pos:{x:63,y:385}, image : { rc:"paneling", index:213 }, object:"road",
           miniature : { rc:"panelwindows", index:9 },
           pushButton : true, submenu: false, sound: "road" },

  waymark : { pos:{x:113,y:385}, image: { rc:"paneling", index:216 }, object:"waymark",
           miniature : { rc:"panelwindows", index:10 },
           pushButton : true, submenu: false, sound: "waymark" },

  attackTrigger : { pos:{x:63, y:421}, image: { rc:"paneling", index:225 }, object:"attackTrigger",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "attackTrigger",
           ingame : true, ineditor : false, height : 22 },

  undo : { pos:{x:13,y:421}, image: { rc:"paneling", index:171 }, object:"cancel",
           miniature : { rc:"panelwindows", index:12 },
           pushButton : true, submenu: false, sound: "cancel",
           ingame : true, ineditor : false, height : 22 },

  minimize : { pos:{x:127,y:5}, image: { rc:"paneling", index:97 }, object:"maximize",
               miniature : { rc:"panelwindows", index:12 },
               pushButton : false, submenu: false, sound: "hide_bigpanel",
               ingame : true, ineditor : false,},

  senate : { pos:{x:7,y:155}, image: { rc:"paneling", index:79 }, object:"senate",
             miniature : { rc:"panelwindows", index:12 },
             pushButton : false, submenu: false, sound: "hide_bigpanel", enabled : false},

  empire : { pos:{x:84,y:155}, image: { rc:"paneling", index:82 }, object:"empire",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "empire", enabled : false },

  mission : { pos:{x:7,y:184}, image: { rc:"paneling", index:85 }, object:"senate",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "senate", enabled : false },

  reorient_map_to_north : { pos:{x:46,y:184}, image: { rc:"paneling", index:88 }, object:"reorient_map_to_north",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "reorient_map_to_north" },

  rotate_map_counter_clockwise : { pos:{x:84,y:184}, image: { rc:"paneling", index:91 }, object:"rotate_map_counter_clockwise",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "rotate_map_counter_clockwise" },

  rotate_map_clockwise : { pos:{x:123,y:184}, image: { rc:"paneling", index:94 }, object:"rotate_map_clockwise",
            miniature : { rc:"panelwindows", index:12 },
            pushButton : false, submenu: false, sound: "rotate_map_clockwise" },
}

sim.ui.menu.funcs = [];

sim.ui.menu.rightSide = engine.getOption("rightMenu");

sim.ui.menu.enableAlarm = function(enable) {
  if (enabled) {
    g_session.playAudio("extm_alarm_00001", 100, g_config.audio.effects);
  }

  sim.ui.menu.widget.disaster.button.enabled = enable;
}

sim.ui.menu.simConfig.disaster.func = function() {
  g_session.setOption( "nextAlarm", true );
}

sim.ui.menu.enableUndo = function(enable) {
  sim.ui.menu.widget.undo.button.enabled = enable;
}

sim.ui.menu.simConfig.housing.func = function() {
  sim.ui.buildmenu.hide();
  sim.ui.menu.widget.miniature.picture = sim.ui.menu.config.housing.miniature;
  g_session.setOption("buildMode", "house");
}

sim.ui.menu.simConfig.undo.func = function() {
  g_session.setOption("undo", true);
}

sim.ui.menu.simConfig.clearLand.func = function() {
  sim.ui.buildmenu.hide();
  sim.ui.menu.widget.miniature.picture = sim.ui.menu.config.clearLand.miniature;
  g_session.setOption( "removeTool", true );
}

sim.ui.menu.simConfig.road.func = function() {
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

sim.ui.menu.simConfig.senate.func = function() { g_session.setOption( "advisor", g_config.advisor.employers ); }
sim.ui.menu.simConfig.empire.func = function() { g_session.setOption( "showEmpireMap", true); }
sim.ui.menu.simConfig.mission.func = function() { sim.ui.dialogs.showMissionTargets(); }
sim.ui.menu.simConfig.reorient_map_to_north.func = function() {  }
sim.ui.menu.simConfig.rotate_map_counter_clockwise.func = function() { g_session.setOption("rotateLeft", true); }
sim.ui.menu.simConfig.rotate_map_clockwise.func = function() { g_session.setOption("rotateRight", true); }

sim.ui.menu.initialize = function() {
  if (g_session.city.getOption("constructorMode")) {
    sim.ui.menu.config = sim.ui.menu.buildConfig;
  } else {
    sim.ui.menu.config = sim.ui.menu.simConfig;
  }

  var menu = new Widget(0);
  menu.name = "ExtentMenu";
  menu.buttons = [];

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

    if (config.enabled != undefined)
      btn.enabled = config.enabled;

    menu.buttons[i] = btn;
  }

  menu.minimap = new Minimap(menu);
  menu.minimap.geometry = { x:8, y:35, w:144, h:110 };
  menu.minimap.city =  g_session.city;
  menu.minimap.center = g_session.camera.center;

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
