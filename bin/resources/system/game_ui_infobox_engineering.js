game.ui.infobox.aboutObject = function(typename,info) {
  var ibox = game.ui.infobox.simple(0,0,510,300)

  ibox.title = _u(typename)

  if (!info)
    info = _u(typename + "_info")

  ibox.setInfoText(info)
  ibox.setAutoPosition()
  ibox.setModal()
  ibox.setFocus()

  return ibox
}

game.ui.infobox.aboutRuins = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)

  var ruins = g_session.city.getOverlay(location).as(Ruins)
  engine.log(ruins.typename)

  ibox.title = _u(ruins.typename)
  ibox.setInfoText(ruins.getProperty("pinfo"))
}

game.ui.infobox.aboutBarracks = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350);
  ibox.blackFrame.geometry = {x:16, y:80, w:ibox.w-32, h:56};

  var barracks = g_session.city.getOverlay(location).as(Barracks);
  engine.log(barracks.typename);
  ibox.overlay = barracs;

  ibox.title = _u(barracks.typename);
  ibox.setInfoText(_u("barracks_info"));

  var lbWeaponQty = ibox.addLabel(20, ibox.lbText.bottom(), ibox.w-32, 24);
  lbWeaponQty.font = "FONT_3";
  lbWeaponQty.text = _format( "{0} {1}", _u("weapon_store_of"), barracks.goodQty("weapon") );

  ibox.setWorkersStatus(32, 56+12, 542, barracks.maximumWorkers(), barracks.numberWorkers());
  ibox.setWorkingStatus(barracks.active);
  ibox.setAutoPosition();

  ibox.btnHelp.uri = barracks.typename;
  ibox.setAutoPosition();
  ibox.setModal();
  ibox.setFocus();
}

game.ui.infobox.aboutFountain = function(location) {
  var ibox = this.aboutConstruction(0,0,480,320);

  var fountain = g_session.city.getOverlay(location);
  ibox.title = _u(fountain.typename);
  ibox.overlay = fountain;

  var reservoirAccess = fountain.getProperty("reservoirAccess");
  var reservoirWater = fountain.getProperty("reservoirWater");
  var text = "fountain_unknown_status";

  if (reservoirAccess && reservoirWater<=0) {
    text = "fountain_will_soon_be_hooked";
  } if (fountain.active) {
    text = fountain.mayWork() ? "fountain_info" : "fountain_not_work";
  } else {
    text = reservoirAccess ? "need_full_reservoir_for_work" : "need_reservoir_for_work";
  }

  ibox.setInfoText(_u(text));
}

game.ui.infobox.aboutWharf = function(location) {
  var ibox = game.ui.infobox.aboutFactory(location);

  var waitBoat = ibox.overlay.getProperty("waitBoat");
  if (waitBoat)
  {
    var lb = ibox.addLabel( ibox.lbProduction.left(), ibox.lbProduction.bottom(), ibox.w/2, 25);
    lb.multiline = true;
    lb.text = _u("wait_for_fishing_boat");
    ibox.lbText.position = { x:lb.left(), y:lb.bottom() + 5 };
  }
}

game.ui.infobox.aboutShipyard = function(location) {
  var ibox = game.ui.infobox.aboutFactory(location);

  var progressCount = ibox.overlay.progress();
  if (progressCount > 1 && progressCount < 100)
  {
    var lb = ibox.addLabel(ibox.lbProduction.left(), ibox.lbProduction.bottom(), ibox.w/2, 25);
    lb.multiline = true;
    lb.text = _u("build_fishing_boat");
    ibox.lbText.position = { x:lb.left(), y:lb.bottom() + 5 };
  }
}

game.ui.infobox.aboutFactory = function(location) {
  var ibox = this.aboutConstruction(0,0,510,256);
  ibox.blackFrame.geometry = {x:16, y:160, w:ibox.w-32, h:52}

  var factory = g_session.city.getOverlay(location).as(Factory);
  ibox.overlay = factory;
  ibox.title = _u(factory.typename);

  // paint progress
  var text = _format( "{0} {1}%", _ut("rawm_production_complete_m"), factory.progress() );

  ibox.lbProduction = ibox.addLabel(20, 40, ibox.w/2, 25);
  ibox.lbProduction.text = text

  var effciencyText = _format("{0} {1}%", _ut("effiency"), factory.effiency);
  var lbEffciency = ibox.addLabel( ibox.w/2, 40, ibox.w/2, 25);
  lbEffciency.text = effciencyText;

  var pinfo = factory.produce;
  ibox.addImage(10, 10, pinfo.picture.local);

  var cinfo = factory.consume;
  // paint picture of in good
  if (cinfo.type != g_config.good.none)
  {
    var lbStockInfo = ibox.addLabel(20, 65, ibox.w-40, 25);
    lbStockInfo.icon = cinfo.picture.local;

    var whatStock = _format( "{0}_factory_stock", cinfo.name );
    var typeOut = _format( "{0}_factory_stock", pinfo.name );
    var text = _format( "{0} {1} {2} {3}",
                        0,//factory.inStock().qty() / 100,
                        _ut(whatStock),
                        0, //factory.outStock().qty() / 100,
                        _ut(typeOut) );

    lbStockInfo.text = text;
    lbStockInfo.textOffset = { x:30, y:0 };
  }

  var workInfo = factory.workersProblemDesc();
  var cartInfo = factory.cartStateDesc();
  ibox.setInfoText(_format( "{0} {1}", _t(workInfo), _t(cartInfo)));

  ibox.setWorkersStatus(32, 160, 542, factory.maximumWorkers(), factory.numberWorkers());
  ibox.setWorkingStatus(factory.active);

  ibox.setAutoPosition();
  ibox.setModal();
  ibox.setFocus();

  return ibox;
}

game.ui.infobox.aboutRaw = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350);
  ibox.blackFrame.geometry = {x:16, y:170, w:ibox.w-32, h:64};

  var factory = g_session.city.getOverlay(location).as(Factory);
  ibox.title = _u(factory.typename);
  ibox.overlay = factory;
  ibox.btnHelp.uri = factory.typename;

  var lbProgress = ibox.addLabel(20, 45, ibox.w-32,24);
  lbProgress.text = _format("{0} {1}%", _ut("rawm_production_complete_m"), factory.progress());

  var lbAbout = ibox.addLabel(16, 70, ibox.w-32, 24);
  lbAbout.multiline = true;

  var lbProductivity = ibox.addLabel(20, 75, ibox.w-32, 48);
  lbProductivity.multiline = true;
  lbProductivity.textAlign = { v:"upperLeft", h:"upperLeft" };

  var problemText = factory.workersProblemDesc();
  var cartInfo = factory.cartStateDesc();

  lbProductivity.text = _t(problemText) + _t(cartInfo);

  var ginfo = factory.produce;
  ibox.addImage(10, 10, factory.produce.picture.local);

  ibox.setWorkersStatus(32, 160, 542, factory.maximumWorkers(), factory.numberWorkers())
  ibox.setWorkingStatus(factory.active)

  ibox.setAutoPosition()
  ibox.setModal()
  ibox.setFocus()
}

game.ui.infobox.aboutSenate = function(location) {
  var ibox = this.simple(0, 0, 510, 290)
  ibox.blackFrame.geometry = {x:16, y:126, w:ibox.w-32, h:62}

  var senate = g_session.city.getOverlay(location).as(Senate);
  ibox.overlay = senate;

  g_session.playAudio("bmsel_senate_00001", 100, g_config.audio.infobox);
  ibox.title = _u(senate.typename);

  // number of workers
  ibox.setWorkersStatus(32, 136, 542, senate.maximumWorkers(), senate.numberWorkers());

  var lb = ibox.addLabel(60, 35, ibox.w-32, 30);
  lb.text = _format( "{0} {1}", _ut("senate_save"), senate.getProperty("funds"));
  lb.icon = g_config.good.getInfo(g_config.good.denaries).picture.local;
  lb.textOffset = {x:30, y:0};

  lb = ibox.addLabel(32, 65, ibox.w-32, 30);
  lb.text = _format( "{0} {1}", _ut("senate_thisyear_tax"), senate.getProperty("thisYearTax") );

  lb = ibox.addLabel(60, 215, 300, 24);
  lb.text = _u("visit_rating_advisor");

  var btnAdvisor = ibox.addTexturedButton(340, 208, 28,28);
  btnAdvisor.states = { rc:"paneling", normal:289, hover:290, pressed:291, disabled:289 };
  btnAdvisor.callback = function() {
    g_session.setOption("advisor",g_config.advisor.ratings);
    ibox.deleteLater();
  }

  ibox.setAutoPosition()
  ibox.setModal()
  ibox.setFocus()
}
