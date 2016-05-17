sim.ui.advisors.entertainment = {}

sim.ui.advisors.entertainment.coverageDescriptions = [ "entert_poor",   "entert_very_bad",
                                                       "entert_bad",    "entert_not_bad",
                                                       "entert_simple", "entert_above_simple",
                                                       "entert_good",   "entert_very_good",
                                                       "entert_pretty", "entert_awesome" ];

sim.ui.advisors.entertainment.info = [ { type:"theater",  building:"theaters",   people:"peoples",  service:g_config.service.theater},
                                       { type:"amphitheater",  building:"amphitheaters",   people:"peoples",   service:g_config.service.amphitheater},
                                       { type:"colosseum",   building:"colosseum",      people:"peoples",   service:g_config.service.colosseum},
                                       { type:"hippodrome",building:"hippodromes",  people:"peoples",  service:g_config.service.hippodrome}  ];

sim.ui.advisors.entertainment.getFestivalAdvice = function(monthFromLastFestival) {
    var strIndex = [ 0, 4, 4, 4,
                     4, 4, 4, 8,
                     8, 8, 8, 12,
                     12, 12, 12, 12,
                     12, 16, 16, 16,
                     16, 16, 16, 24,
                     24, 24, 24, 24,
                     24, 24, 31, 31 ];

    var currentThinkIndex = Math.clamp(monthFromLastFestival, 0, strIndex.length-1);
    return _format( "more_{0}_month_from_festival", strIndex[currentThinkIndex] );
}

sim.ui.advisors.entertainment.findInfo = function(bType) {
  for (var i in sim.ui.advisors.entertainment.info) {
    var config = sim.ui.advisors.entertainment.info[i];
    if (config.type == bType) {
      engine.log("Config found for " + bType)
      return config;
    }
  }
  return null;
}

sim.ui.advisors.entertainment.getInfo = function(bType) {
  var info = sim.ui.advisors.entertainment.findInfo(bType);

  var ret = Utils.clone(info);

  ret.buildingWork = 0;
  ret.peoplesServed = 0;
  ret.buildingShow = 0;
  ret.buildingCount = 0;
  ret.partlyWork = 0;
  ret.needService = 0;

  var buildings = g_city.findOverlays(bType);
  for(var i=0; i < buildings.length; i++) {
    var b = buildings[i].as(EntertainmentBuilding);

    if (b==null)
      continue;

    if (b.numberWorkers > 0) {
      ret.buildingWork++;
      ret.peoplesServed += b.maxVisitors * b.numberWorkers / b.maximumWorkers;
    }

    ret.buildingCount++;
    ret.partlyWork += (b.numberWorkers != b.maximumWorkers ? 1 : 0);
  }

  var houses = g_city.findOverlays("house");
  for (var i=0; i < houses.length; i++) {
    var h = houses[i].as(House);

    if (h == null || !h.habitable)
      continue;

    if (h.isEntertainmentNeed(ret.service)) {
      ret.needService += h.habitantsCount;
    }
  }

  return ret;
}

sim.ui.advisors.entertainment.showDetails = function(objType) {
  var w = g_ui.addSimpleWindow(0, 0, 480, 600);

  var blackFrame = ibox.addLabel(15, 15, w.w-30, w.h-50);
  blackFrame.style = "blackFrame";

  var buildings = g_city.findOverlays(objType);
  var ry = 5;
  for (var i in buildings) {
    var bld = buildings[i].as(HealthBuilding);
    var btn = new Button(blackFrame);
    btn.geometry = { x:5, y:ry, w:blackFrame.w-10, h:20};

    var text = bld.name;
    if (text.length == 0) {
      var pos = bld.pos;
      text = _format( "{0} [{1},{2}]", bld.typename, pos.i, pos.j);
    }

    btn.addLabel(0, 0, btn.w/2, btn.h, text);

    text = _format( "workers {0}/{1}, served {2}", bld.numberWorkers, bld.maximumWorkers, bld.currentVisitors);
    btn.addLabel(btn.w/2, 0, btn.w/2, btn.h, text);

    (function(pos) {
      btn.callback = function() {
        g_session.camera.tileCenter = pos;
     }}
    )(bld.pos);
    ry += 22;
  }

  w.moveToCenter();
  w.mayMove = false;
  w.setModal();
}

sim.ui.advisors.entertainment.getAdvice = function() {
  var troubles = [];

  var thInfo = sim.ui.advisors.entertainment.getInfo("theater");
  var amthInfo = sim.ui.advisors.entertainment.getInfo("amphitheater");
  var clsInfo = sim.ui.advisors.entertainment.getInfo("colosseum");

  var theatersNeed = 0;
  var amptNeed = 0;
  var clsNeed = 0;
  var hpdNeed = 0;
  var minTheaterSrvc = g_config.service.consts.maxValue;
  var theatersServed = 0;
  var amptServed = 0;
  var clsServed = 0;
  var hpdServed = 0;
  var nextLevelMin = 0;
  var nextLevelAmph = 0;
  var nextLevelColosseum = 0;
  var maxHouseLevel = 0;

  var houses = g_city.findOverlays("house");
  for (var i=0; i < houses.length; i++) {
    var house = houses[i].as(House);

    if (house == null)
      continue;

    maxHouseLevel = Math.max(maxHouseLevel, house.level);
    var habitants = house.mature_n;

    if (house.isEntertainmentNeed(g_config.service.theater)) {
      if (habitants > 0) {
        theatersNeed += habitants;
        theatersServed += house.hasServiceAccess(g_config.service.theater);
        minTheaterSrvc = Math.min(house.getServiceValue(g_config.service.theater), minTheaterSrvc);
      }
    }

    if (house.isEntertainmentNeed(g_config.service.amphitheater)) {
      amptNeed += habitants;
      amptServed += (house.hasServiceAccess(g_config.service.amphitheater) ? habitants : 0);
    }

    if (house.isEntertainmentNeed(g_config.service.colosseum)) {
      clsNeed += habitants;
      clsServed += (house.hasServiceAccess(g_config.service.colosseum) ? habitants : 0);
    }

    if (house.isEntertainmentNeed(g_config.service.hippodrome)) {
      hpdNeed += habitants;
      hpdServed += (house.hasServiceAccess(g_config.service.hippodrome) ? habitants : 0);
    }

    var needEntert = (house.nextEntertainmentLevel - house.minEntertainmentLevel < 0 ? 1 : 0);

    if (needEntert) {
      switch(house.minEntertainmentLevel) {
      case g_config.houseNeeds.theater: nextLevelMin++; break;
      case g_config.houseNeeds.amphitheater: nextLevelAmph++; break;
      case g_config.houseNeeds.colosseum: nextLevelColosseum++; break;
      }
    }
  }

  var allNeed = theatersNeed + amptNeed + clsNeed + hpdNeed;
  var allServed = theatersServed + amptServed + clsServed + hpdServed;

  var entertCoverage = Math.percentage( allServed, allNeed);

  if (hpdNeed > hpdServed) { troubles.push("citizens_here_are_bored_for_chariot_races"); }

  if (entertCoverage > g_config.service.consts.normalCoverage
      && entertCoverage <= g_config.service.consts.maxCoverage) { troubles.push("citizens_like_chariot_races"); }
  else if( entertCoverage > g_config.service.consts.badCoverage
           && entertCoverage <= g_config.service.consts.normalCoverage ) { troubles.push("entertainment_50_80"); }
  else if( allNeed > 0 && entertCoverage <= g_config.service.consts.badCoverage )         { troubles.push("entertainment_less_50"); }

  if( minTheaterSrvc < 30 )   { troubles.push("some_houses_inadequate_entertainment"); }
  if( thInfo.partlyWork > 0 ) { troubles.push("some_theaters_need_actors"); }
  if( thInfo.buildingCount == 0 ) { troubles.push("your_city_need_theaters"); }
  if( amthInfo.partlyWork > 0){ troubles.push("some_amphitheaters_no_actors"); }
  if( amthInfo.buildingCount == 0 ) { troubles.push("blood_sports_add_spice_to_life"); }
  if( clsInfo.partlyWork > 0 ){ troubles.push("small_colosseum_show"); }

  var hippodromes = g_city.findOverlays("hippodrome");
  for (var i=0; i < hippodromes.length; i++) {
    var h = hippodromes[i].as(EntertainmentBuilding);
    if (h.evaluateTrainee(g_config.walker.charioteer) == 100) {
      troubles.push("no_chariots");
      break;
    }
  }

  if( nextLevelMin > 0 )  { troubles.push("entertainment_need_for_upgrade");  }
  if( nextLevelAmph > 0 ) { troubles.push("some_houses_need_amph_for_grow"); }
  if( theatersNeed == 0 ) { troubles.push("entertainment_not_need");  }

  if (troubles.length == 0) {
    if( maxHouseLevel < g_config.house.level.bigDomus ) { troubles.push("entadv_small_city_not_need_entert"); }
    else if( maxHouseLevel < g_config.house.level.mansion ) { troubles.push("##small_city_not_need_entertainment"); }
    else if( maxHouseLevel < g_config.house.level.insula ) { troubles.push("etertadv_as_city_grow_you_need_more_entert"); }

    if( thInfo.buildingCount > 0 ) { troubles.push("citizens_enjoy_drama_and_comedy"); }

    troubles.push("entertainment_full");
  }

  return troubles[Math.randomIndex(0,troubles.length-1)];
}

sim.ui.advisors.entertainment.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";
  w.geometry = {x:0, y:0, w:640, h:384};
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.title = _u("entertainment_advisor_title");

  var blackframe = w.addLabel(35, 63, w.w-70, 90);
  blackframe.style = "blackFrame"

  var lbWorkText = w.addLabel(180, 45, 70, 20);
  lbWorkText.font = "FONT_1"
  lbWorkText.text = "##work##"

  var lbShowText = w.addLabel(260, 45, 100, 20)
  lbShowText.font = "FONT_1";
  lbShowText.text = "##show##";

  var lbMaySupport = w.addLabel(350, 45, 100, 20)
  lbMaySupport.font = "FONT_1";
  lbMaySupport.text = "##max_available##";

  var lbCoverage = w.addLabel(480, 45, 100, 20);
  lbCoverage.font = "FONT_1"
  lbCoverage.text = "##coverage##"

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = {rc:"paneling", index:263};

  var lbFestivals = w.addLabel(47, 220, 200, 30);
  lbFestivals.font = "FONT_5";
  lbFestivals.text = "##festivals##";
  lbFestivals.textAlign = {h:"upperLeft", v:"center"};

  var lbTroubleInfo = w.addLabel(50, blackframe.bottom()+2, 540, 70);
  lbTroubleInfo.font = "FONT_1";
  lbTroubleInfo.multiline = true;
  lbTroubleInfo.text = _u(sim.ui.advisors.entertainment.getAdvice());

  var lbFestArea = w.addLabel(50, 247, 540, 110);
  lbFestArea.style = "blackFrame";

  var lbFestIcon = w.addLabel(450, 257, 126, 88);
  lbFestIcon.icon = {rc:"panelwindows", index:16};

  var btnNewFestival = w.addButton(lbFestArea.x + 54, lbFestArea.y+30, 300, 20);
  btnNewFestival.text = _u("new_festival");
  btnNewFestival.style = "blackBorderUp";
  var nextFestivalDate = g_city.getProperty("nextFestivalDate");
  var prepare2Festival = g_session.date.monthsTo(nextFestivalDate) >= 0;
  btnNewFestival.text = prepare2Festival ? _u("prepare_to_festival") : _u("new_festival");
  btnNewFestival.enabled = !prepare2Festival;
  btnNewFestival.callback = function() { game.ui.dialogs.festivalplaning.show(); }

  var lbMonthFromLastFestival = w.addLabel(lbFestArea.x+5, lbFestArea.y+5, 390, 20);
  lbMonthFromLastFestival.font = "FONT_1";
  lbMonthFromLastFestival.textAlign = {v:"center", h:"center"};
  var lastFestivalDate = g_city.getProperty("lastFestivalDate");
  var monthFromLastFestival = lastFestivalDate.monthsTo(g_session.date);
  lbMonthFromLastFestival.text = _format( "{0} {1} {2}", monthFromLastFestival, _ut("month"), _ut("month_from_last_festival"));

  var lbInfoAboutLastFestival = w.addLabel(lbFestArea.x+5, lbFestArea.y+55, 390, 50);
  lbInfoAboutLastFestival.font = "FONT_1";
  lbInfoAboutLastFestival.multiline = true;
  lbInfoAboutLastFestival.text = _u(this.getFestivalAdvice(monthFromLastFestival));
  lbInfoAboutLastFestival.textAlign = {h:"upperLeft", v:"upperLeft"};

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "entertainment_advisor";

  w.addEntertinmentButton = function(pos, type) {
    var btn = new Button(blackframe);
    btn.geometry = {x:pos.x, y:pos.y, w:blackframe.w-pos.x*2, h:20}
    btn.style = "brownBorderOnly";

    var info = sim.ui.advisors.entertainment.getInfo(type);
    var strBuildings = _format( "{0} {1}", info.buildingCount, _ut(info.building) );
    var strPeoples = _format( "{0} ({1}) {2}", info.peoplesServed, info.needService, _ut(info.people) );

    var descriptions = sim.ui.advisors.entertainment.coverageDescriptions;
    var coverageStr = (info.coverage > 0 && info.buildingCount > 0)
                                   ? descriptions[ Math.clamp( info.coverage / descriptions.length, 0, descriptions.length-1 ) ]
                                   : _u("non_cvrg");

    btn.addLabel(0,   0, 120, 20, strBuildings, "FONT_1_WHITE");
    btn.addLabel(165, 0, 60,  20, info.buildingWork,   "FONT_1");
    btn.addLabel(225, 0, 60, 20,  info.buildingShow,   "FONT_1");
    btn.addLabel(305, 0, 100, 20, strPeoples,   "FONT_1");
    btn.addLabel(440, 0, 70,  20, coverageStr,  "FONT_1");
  }

  var ry=5;
  w.addEntertinmentButton({x:12, y:ry},    "theater");
  w.addEntertinmentButton({x:12, y:ry+20}, "amphitheater");
  w.addEntertinmentButton({x:12, y:ry+40}, "colosseum");
  w.addEntertinmentButton({x:12, y:ry+60}, "hippodrome");
}
