sim.ui.rpanel.rightSide = engine.getOption("rightMenu");

sim.ui.rpanel.initialize = function () {
    var rpanel = new Widget(0);
    rpanel.name = "MenuRPanel";

    var topmenu = g_ui.find("TopMenu");
    var rbody = g_render.picture("paneling", 14);
    var resolution = g_session.resolution;
    rpanel.geometry = {
        x: sim.ui.rpanel.rightSide ? resolution.w - rbody.w : 0,
        y: topmenu.h,
        w: rbody.w,
        h: resolution.h
    };

    var ty = 0;
    while (ty < rpanel.h) {
        var img = new Image(rpanel);
        img.y = ty;
        img.picture = rbody;
        img.mode = "image";
        ty += rbody.h;
    }

    sim.ui.rpanel.widget = rpanel;
    sim.ui.rpanel.initialized = true;
}

sim.ui.rpanel.reset = function() {
  if (sim.ui.rpanel.widget != null) {
    engine.log( "Found old rpanel menu -> removed" );
    sim.ui.rpanel.widget.deleteLater();
    sim.ui.rpanel.widget = null;
    sim.ui.rpanel.initialize();
  }
}

sim.ui.rpanel.setRightSide = function(right) {
  sim.ui.rpanel.rightSide = right;
  sim.ui.rpanel.reset();
}

sim.ui.rpanel.reset();
