sim.ui.shortmenu.funcs = [];

sim.ui.shortmenu.minimize = function() {
  var menu = sim.ui.menu.shortmenu;

  sim.ui.buildmenu.hide();
  var stopPos = { x:menu.x + menu.w * (sim.ui.menu.rightSide ? 1 : -1 ), y:menu.y};
  var animator = g_ui.addPosAnimator(menu, stopPos, 1000, sim.ui.menu.maximize);
  animator.setFlag("showParent", false);

  g_session.playAudio("panel_00003", 100, g_config.audio.infobox);
}

sim.ui.shortmenu.maximize = function() {
  var menu = sim.ui.menu.shortmenu;

  sim.ui.buildmenu.hide();
  menu.display = true;
  var stopPos = { x:menu.x - menu.w * (sim.ui.menu.rightSide ? 1 : -1), y:menu.y};
  var animator = g_ui.addPosAnimator(menu, stopPos, 1000);
  animator.setFlag("showParent", true);

  g_session.playAudio( "panel_00003", 100, g_config.audio.infobox );
}

sim.ui.shortmenu.initialize = function() {
  var menu = new Widget(0);
  menu.name = "ShortMenu";
  menu.buttons = [];

  var topmenu = g_ui.find("TopMenu");

  var rbody = g_render.picture("paneling", 14);

  var bgbody = g_render.picture("paneling", 16);
  var bgbottom = g_render.picture("paneling", 21);

  var resolution = g_session.resolution;

  menu.geometry = {x:resolution.w - rbody.w, y:topmenu.h, w:bgbody.w, h:resolution.h};
  menu.imgBody = new Image(menu);
  menu.imgBody.picture = bgbody;
  menu.imgBody.mode = "image";

  menu.minimize =  new TexturedButton(menu);
  menu.minimize.position = {x:5, y:5};
  menu.minimize.tooltip = _u("extm_maximize_tlp");

  menu.minimize.states = { rc:"paneling", normal:97, hover:97+1,
                           pressed:97+2, disabled:97+3 };
  menu.minimize.fitToImage();
  menu.minimize.callback = function() { sim.ui.shortmenu.minimize(); }
  menu.display = false;

  var ty = menu.imgBody.h;
  while (ty < menu.h) {
    var img = new Image(menu);
    img.y = ty;
    img.picture = bgbottom;
    img.mode = "image";
    ty += bgbottom.h;
  }

  for (var i in sim.ui.menu.simConfig) {
    var config = sim.ui.menu.simConfig[i];

    engine.log(i);

    if (config.shortmenuIndex == undefined)
      continue;

    var btn = new TexturedButton(menu);
    btn.position = { x:1, y:32+35*config.shortmenuIndex};
    btn.tooltip = _u("extm_" + config.object + "_tlp");

    btn.states = { rc:config.image.rc, normal:config.image.index, hover:config.image.index+1,
                   pressed:config.image.index+2, disabled:config.image.index+3 };
    btn.fitToImage();

    if (config.height)
      btn.h = config.height;

    (function(c, x, y){
      if (config.func) sim.ui.shortmenu.funcs[i] = c.func;
      else sim.ui.shortmenu.funcs[i] = function(){ sim.ui.menu.showbuildmenu(c, x, y); }
    })(config, menu.w, btn.screeny);

    btn.callback = sim.ui.shortmenu.funcs[i];

    if (config.enabled != undefined)
      btn.enabled = config.enabled;

    menu.buttons[i] = btn;
  }

  sim.ui.menu.shortmenu = menu;
}

sim.ui.shortmenu.reset = function() {
  if (sim.ui.menu.shortmenu != null) {
    engine.log( "Found old short menu -> removed" );
    sim.ui.menu.shortmenu.deleteLater();
    sim.ui.menu.shortmenu = null;
    sim.ui.shortmenu.initialize();
  }
}

sim.ui.shortmenu.setRightSide = function(right) {
  sim.ui.menu.rightSide = right;
  sim.ui.shortmenu.reset();
}

sim.ui.shortmenu.reset();
