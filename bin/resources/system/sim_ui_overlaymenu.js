g_config.overlaymenu = {
  all : {
      layers: ["simple", "water"],
      submenu : ["risks", "entertainments", "educations", "healthAll", "commerce", "religion", "products"]
  },

  risks : {
    layers: [ "fire", "damage", "crime", "aborigen", "troubles", "sentiment" ],
    submenu : [ "all" ]
  },

  entertainments : {
    layers: [ "entertainment", "theater", "amphitheater", "colosseum", "hippodrome" ],
    submenu : [ "all" ]
  },

  educations : {
    layers: [ "education", "school", "library", "academy" ],
    submenu : [ "all" ]
  },

  healthAll : {
    layers: [ "health", "barber", "baths", "doctor", "hospital" ],
    submenu : [ "all" ]
  },

  commerce : {
    layers: [ "tax", "food", "market", "desirability", "unemployed", "comturnover" ],
    submenu : [ "all" ]
  }
}

sim.ui.overlaymenu.hide = function() {
  var menus = g_ui.find("OverlayMenu");
  if (menus != null)
    menus.deleteLater();
}

sim.ui.overlaymenu.switch = function(type) {
  var sound = "ovrmsel_" + type;
  g_session.playAudio(sound + "_00001", 100, g_config.audio.infobox);
  g_session.setOption("layer", type);
  sim.ui.menu.changeRendererLayer(type);
  sim.ui.overlaymenu.hide();
}

sim.ui.overlaymenu.sub = function(type, left, top) {
  var sound = "ovrmsel_" + type;
  g_session.playAudio(sound + "_00001", 100, g_config.audio.infobox);
  sim.ui.overlaymenu.hide();
  sim.ui.overlaymenu.show(type, left, top);
}

sim.ui.overlaymenu.show = function(type, left, top) {
  if (type == undefined || type == "")
    return;

  var ovmenu = new Widget(0);

  if (ovmenu != null) {
    ovmenu.clipped = false;
    ovmenu.name = "OverlayMenu";
    ovmenu.buttons = [];
    ovmenu.hleft = left;
    ovmenu.htop = top;
    ovmenu.subElement = true;
    var resolution = g_session.resolution;
    ovmenu.geometry = {x:0, y:0, w:resolution.w - left, h:resolution.h};

    var wcloser = new WidgetClosers(ovmenu);
    wcloser.addCloseCode(g_config.key.KEY_RBUTTON);
    wcloser.addCloseCode(g_config.key.KEY_ESCAPE);
  }

  ovmenu.addButton = function(branch,type) {
    var ltype = branch.length ? branch : type;
    var title = _format( "##ovrm_{0}##", ltype);

    var btn = new Button(ovmenu);
    btn.geometry = { x:0, y:25*ovmenu.buttons.length, w:ovmenu.w, h:24};
    btn.hoverFont = "FONT_2_RED";
    btn.style = "smallGrayBorderLine";
    btn.text = title;
    btn.textAlign = { h:"upperLeft", v:"center" };
    btn.textOffset = { x:15, y:0 };

    btn.btnHelp = new HelpButton(btn);
    btn.btnHelp.position = { x:btn.w-24, y:0 };
    btn.btnHelp.uri = ltype;
    btn.btnHelp.align = { left:g_config.align.lowerRight, top:g_config.align.lowerRight,
                          right:g_config.align.lowerRight, bottom:g_config.align.lowerRight };

    btn.sound = _format( "ovmsel_{0}", ltype );
    ovmenu.buttons.push(btn);

    if (branch != "") btn.callback = function() { sim.ui.overlaymenu.sub(branch, left, top); }
    else if (type != "") btn.callback = function() { sim.ui.overlaymenu.switch(type); }
  }

  ovmenu.update = function(type) {
    var max_text_width = 0;

    engine.log("Opened overlaymenu " + type);
    var config = g_config.overlaymenu[type];

    if (config.layers != undefined) {
      for (var i in config.layers) {
        ovmenu.addButton("", config.layers[i]);
      }
    }

    if (config.submenu != undefined) {
      for (var i in config.submenu)
        ovmenu.addButton(config.submenu[i], "");
    }

    for (var i in ovmenu.buttons) {
      var bbutton = ovmenu.buttons[i];
      max_text_width = Math.max(max_text_width, bbutton.textWidth);
    }

    //engine.log(max_text_width);
    var appear_width = Math.max(150, max_text_width) + 80;

    // set the same size for all buttons
    for(var i in ovmenu.buttons) {
      var btn = ovmenu.buttons[i];
      btn.w = appear_width;
      btn.x = ovmenu.w - btn.w - 35;
      btn.y = ovmenu.htop + 25 * i + 15;
    }
  }

  if (type != "")
    ovmenu.update(type);
}
