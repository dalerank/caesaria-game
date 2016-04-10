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

  govt : { pos:{x:113,y:349}, image : { rc:"paneling", index:133 }, object:"administration",
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
}

sim.ui.menu.funcs = [];

sim.ui.menu.initialize = function() {
  var menu = new Widget(0);
  menu.name = "ExtentMenu";

  var topmenu = g_ui.find("TopMenu");

  var rbody = g_render.picture("paneling", 14);

  var bgbody = g_render.picture("paneling", 17);
  var bgbottom = g_render.picture("paneling", 20);

  var resolution = g_session.resolution;

  menu.geometry = {x:resolution.w - bgbody.w - rbody.w, y:topmenu.h, w:bgbody.w, h:resolution.h};
  var imgBody = new Image(menu);
  imgBody.picture = bgbody;
  imgBody.mode = "image";

  var ty = imgBody.h;
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
    btn.states = { rc:config.image.rc, normal:config.image.index, hover:config.image.index+1,
                   pressed:config.image.index+2, disabled:config.image.index+3 };
    btn.fitToImage();

    if (config.height)
      btn.h = config.height;

    (function(o, y){
      sim.ui.menu.funcs[i] = function(){
                  sim.ui.buildmenu.show(o, y);
              }
    })(config.object, btn.screeny);

    if (config.func) { btn.callback = config.func; }
    else { btn.callback = sim.ui.menu.funcs[i] };
  }

  sim.ui.menu.initialized = true;
}

if (sim.ui.menu.initialized) {
  var menu = g_ui.find("ExtentMenu");
  if (menu != null)
  {
    engine.log( "Find old main menu" );
    menu.deleteLater();
  }

  sim.ui.menu.initialize();
}
