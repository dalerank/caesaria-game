game.ui.infobox.aboutObject = function(typename,info) {
  var ibox = game.ui.infobox.simple(0,0,510,300)

  ibox.title = _u(typename)

  if (!info)
    info = _u(typename + "_info")

  ibox.setInfoText(info)
  ibox.show();
}

game.ui.infobox.aboutRuins = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)

  var ruins = g_session.city.getOverlay(location).as(Ruins)
  engine.log(ruins.typename)

  ibox.title = _u(ruins.typename)
  ibox.setInfoText(ruins.getProperty("pinfo"))

  ibox.show();
}

game.ui.infobox.aboutBarracks = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350);
  ibox.initBlackframe(16, 80, ibox.w-32, 56);

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

  ibox.show();
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

  ibox.show();
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
  ibox.initBlackframe(16, 160, ibox.w-32, 52);

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
  var pinfo = factory.produce;
  // paint picture of in good
  if (cinfo.type != g_config.good.none)
  {
    var lbStockInfo = ibox.addLabel(20, 65, ibox.w-40, 25);
    lbStockInfo.icon = cinfo.picture.local;

    var whatStock = _format( "{0}_factory_stock", cinfo.name );
    var text = _format( "{0} {1}",
                        factory.inStock().qty() / 100,
                        _ut(whatStock));
    if (pinfo.type != g_config.good.none)
    {
      var textOut = _format("{0} {1}",
                        factory.outStock().qty() / 100,
                        _ut(typeOut) );
      text += textOut;
    }

    lbStockInfo.text = text;
    lbStockInfo.textOffset = { x:30, y:0 };

    ibox.initInfoLabel(30, lbStockInfo.bottom(), ibox.w-30, ibox.blackFrame.top() - lbStockInfo.bottom());
  }

  var workInfo = factory.workersProblemDesc();
  var cartInfo = factory.cartStateDesc();
  ibox.setInfoText(_format( "{0} {1}", _t(workInfo), _t(cartInfo)));

  ibox.setWorkersStatus(32, 160, 542, factory.maximumWorkers(), factory.numberWorkers());
  ibox.setWorkingStatus(factory.active);

  ibox.show();

  return ibox;
}

game.ui.infobox.aboutRaw = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350);
  ibox.initBlackframe(16, 170, ibox.w-32, 64);

  var factory = g_session.city.getOverlay(location).as(Factory);
  ibox.title = _u(factory.typename);
  ibox.overlay = factory;

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

  ibox.setWorkersStatus(32, 160, 542, factory.maximumWorkers(), factory.numberWorkers());
  ibox.setWorkingStatus(factory.active);

  ibox.show();
}

game.ui.infobox.aboutSenate = function(location) {
  var ibox = this.simple(0, 0, 510, 290)
  ibox.initBlackframe( 16, 126, ibox.w-32, 62);

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

  ibox.show();
}

game.ui.infobox.aboutWorkingBuilding = function(location) {
  var ibox = this.aboutConstruction(0,0,510,256);

  ibox.initInfoLabel(16,50,ibox.w-32,64);
  ibox.initBlackframe(16, 136, ibox.w-32, 62);

  var working = g_session.city.getOverlay(location).as(WorkingBuilding);
  ibox.overlay = working;
  ibox.title = _u(working.typename)

  ibox.setWorkersStatus(32, 150, 542, working.maximumWorkers(), working.numberWorkers());
  ibox.setWorkingStatus(working.active);

  ibox.setInfoText(working.workersProblemDesc());

  ibox.show();

  return ibox;
}

game.ui.infobox.aboutServiceBuilding = function(location, text) {
  var ibox = this.aboutWorkingBuilding(location);

  if (text != undefined)
    ibox.setInfoText(text);

  ibox.showAdvInfo = function() {
    var lastServiceDate = ibox.overlay.getProperty("lastServiceDate");
    var state = _format( "Damage={0}\nFire={1}\nService={2}",
                         ibox.overlay.state(g_config.overlay.params.damage),
                         ibox.overlay.state(g_config.overlay.params.fire),
                         lastServiceDate.format(g_session.metric) );

    g_ui.addInformationDialog(_u("overlay_status"), state);
  }
}

game.ui.infobox.aboutColosseum = function(location) {
  var ibox = this.aboutConstruction(0, 0, 470, 300);
  var coloseum = g_session.getOverlay(location).as(WorkingBuilding);

  ibox.overlay = colosseum;
  ibox.initBlackFrame(16, 145, ibox.w - 16,100);

  ibox.setWorkersStatus(32, 150, 542, coloseum.maximumWorkers(), coloseum.numberWorkers());
  ibox.setWorkingStatus(coloseum.active);
  ibox.title = _u(colosseum.typename)

  var isNeedGladiators = colosseum.getProperty("isNeedGladiators");
  if (isNeedGladiators)
  {
    var lb = ibox.addLabel(35, 190,ibox.w-35,20);
    lb.text = _u("colosseum_haveno_gladiatorpit");
  }
  else
  {
    var text = _u("colosseum_haveno_animal_bouts");
    var isShowLionBattles = colosseum.getProperty("isShowLionBattles");
    if(isShowLionBattles)
    {
      var lastAnimalBoutDate = coloseum.getProperty("lastAnimalBoutDate");
      text = _format( "{0} {1} {2}", _ut("colosseum_animal_contest_runs"),
                                     lastAnimalBoutDate.daysTo(g_session.date),
                                     _ut("days") );
    }
    ibox.addLabel(35, 200, ibox.w-35,20, text);

    text = _u("colosseum_haveno_gladiator_bouts");
    var isShowGladiatorBattles = coloseum.getProperty("isShowGladiatorBattles");
    if (isShowGladiatorBattles)
    {
      var lastGlBoutDate = coloseum.getProperty("lastGladiatorBoutDate");
      text = _format( "{0} {1} {2}", _ut("colosseum_gladiator_contest_runs"), lastGlBoutDate.daysTo(g_session.date), _ut("days") );
    }
    ibox.addLabel(35, 220, ibox.w - 35, 20, text);
  }

  ibox.show();
}

game.ui.infobox.aboutTheater = function(location) {
  var ibox = this.aboutConstruction(0, 0, 470, 300);
  ibox.initBlackframe(16, 145, ibox.w-32,100);

  var theater = g_session.city.getOverlay(location).as(WorkingBuilding);

  ibox.title = _u(theater.typename);
  ibox.setWorkersStatus(32, 150, 542, theater.maximumWorkers(), theater.numberWorkers());
  ibox.setWorkingStatus(working.active);

  var shows = g_config.entertainment.shows;
  var showsCount = theater.getProperty("showsCount");
  var isShow = theater.getProperty("isShow");

  if (showsCount == 0) {
    ibox.setInfoText( "##theater_no_have_any_shows##" );
  } else if (isShow) {
    var currentShow = showsCount%shows.length;
    var text = shows[currentShow];

    if (text.length==0)
      text = "##theater_now_local_show##";

    ibox.setInfoText( _(text) );
  } else {
    ibox.setInfoText( "##theater_need_actors##" );
  }

  ibox.show();
}

game.ui.infobox.aboutAmphitheater = function(location) {
  var ibox = this.aboutConstruction(0, 0, 470, 300);
  ibox.initBlackframe(16, 145, ibox.w-32,100);

  var amphitheater = g_session.city.getOverlay(location).as(WorkingBuilding);
  ibox.title = _u(amphitheater.typename);

  ibox.setWorkersStatus(32, 150, 542, amphitheater.maximumWorkers(), amphitheater.numberWorkers());
  ibox.setWorkingStatus(amphitheater.active);

  var needGlagiators = amphitheater.getProperty("needGlagiators");
  if( needGlagiators ) {
    ibox.addLabel(35, 190, ibox.w-70, 20, _u("amphitheater_haveno_gladiatorpit"));
  } else {
    var text = _u("amphitheater_haveno_gladiator_bouts");
    var showGladiatorBouts = amphitheater.getProperty( "showGladiatorBouts");
    if (showGladiatorBouts)
    {
        var lastBoutDate = amphitheater.getProperty("lastShowGladiatorBouts");
        text = _format( "{0} {1} {2}", _ut("amphitheater_gladiator_contest_runs"), lastGlBoutDate.daysTo(g_session.date), _ut("days") );
    }
    ibox.addLabel(35, 200, ibox.w-70, 20, text);

    text = _u("amphitheater_haveno_shows");
    var showTheatrical = amphitheater.getProperty("showTheatrical");
    if (showTheatrical)
    {
      var lastShowDate = amphitheater.getProperty("lastShowTheatrical");
      text = _format( "{0} {1} {2}", _ut("amphitheater_show_runs"), lastShowDate.daysTo(g_session.date), _ut("days") );
    }

    ibox.addLabel(35, 220, ibox.w - 70, 20, text);
  }
}

game.ui.infobox.aboutWell = function(location) {
  var ibox = this.aboutConstruction(0, 0, 480, 320);

  var well = g_session.city.getOverlay(location);
  ibox.initInfoLabel(20, 20, ibox.w-40, ibox.h-60);
  ibox.overlay = well;
  ibox.title = _u(well.typename);

  var haveHouseInArea = well.getProperty("coverageHouse");
  var text;
  if (!haveHouseInArea) {
      text = _u("well_haveno_houses_inarea");
  }
  else
  {
    var houseNeedWell = well.getProperty("housesNeedWell");
    if (!houseNeedWell) {
      text = _u("also_fountain_in_well_area");
    } else {
      var houseNum = well.getProperty("lowHealthHouseNumber");

      text = houseNum > 0
              ? _u("well_infected_info")
              : _u("well_info");
    }
  }

  ibox.setInfoText(text);
  ibox.show();
}
