sim.ui.advisors.health = {}

sim.ui.advisors.health.consts = {
  rowOffset : 20,
  smallCityNormalhealthValue : 85,
  minPopullation4healthCalc : 100,
  smallCityPopulation : 300
}

sim.ui.advisors.health.coverageDescriptions = [ "health_poor",   "health_very_bad",
                                                "health_bad",    "health_not_bad",
                                                "health_simple", "health_above_simple",
                                                "health_good",   "health_very_good",
                                                "health_pretty", "health_awesome" ];

sim.ui.advisors.health.info = [ { type:"clinic",  building:"clinics",   people:"patients",  service:g_config.service.doctor,  max:75,  age:"scholar_n" },
                                { type:"barber",  building:"barbers",   people:"peoples",   service:g_config.service.barber,  max:100, age:"student_n" },
                                { type:"baths",   building:"bath",      people:"peoples",   service:g_config.service.baths,   max:800, age:"mature_n"  },
                                { type:"hospital",building:"hospital",  people:"patients",  service:g_config.service.hospital,max:800, age:"mature_n"  }  ];

sim.ui.advisors.health.findInfo = function(bType) {
  for (var i in sim.ui.advisors.health.info) {
    var config = sim.ui.advisors.health.info[i];
    if (config.type == bType) {
      engine.log("Config found for " + bType)
      return config;
    }
  }
  return null;
}


sim.ui.advisors.health.getInfo = function(bType) {
  var info = sim.ui.advisors.health.findInfo(bType);

  var ret = Utils.clone(info);

  ret.buildingWork = 0;
  ret.peoplesServed = 0;
  ret.buildingCount = 0;
  ret.needService = 0;

  var buildings = g_city.findOverlays(ret.type);

  for (var i=0; i < buildings.length; i++) {
    var b = buildings[i].as(HealthBuilding);

    if (b==null)
      continue;

    ret.buildingWork += b.numberWorkers > 0 ? 1 : 0;
    ret.peoplesServed += b.patientsCurrent;
    ret.buildingCount++;
  }

  var houses = g_city.findOverlays("house");
  for (var i=0; i < houses.length; i++) {
    var h = houses[i].as(House);

    if (h == null || !h.habitable)
      continue;

    if (h.isHealthNeed(ret.service)) {
      ret.needService += h.habitantsCount;
    }
  }

  return ret;
}

sim.ui.advisors.health.getAdvice = function() {
  var outText = [];
  var consts = sim.ui.advisors.health.consts;

  if( g_city.population < consts.minPopullation4healthCalc ) {
    outText.push("healthadv_not_need_health_service_now");
  } else {
    if( g_city.population < consts.smallCityPopulation ) {
      if( g_city.health > consts.smallCityNormalhealthValue ) {
        outText.push("healthadv_noproblem_small_city");
      }
    } else {
      var houses = g_city.findOverlays("house");

      var needBath = 0;
      var needBarbers = 0;
      var needDoctors = 0;
      var needHospital = 0;
      for (var i=0; i<houses.length; i++) {
        needBath += house.isHealthNeed( g_config.service.baths ) ? 1 : 0;
        needDoctors += house.isHealthNeed( g_config.service.doctor ) ? 1 : 0;
        needBarbers += house.isHealthNeed( g_config.service.barber ) ? 1 : 0;
        needHospital += house.isHealthNeed( g_config.service.hospital ) ? 1 : 0;
      }

      if (needBath > 0) {
        outText.push("healthadv_some_regions_need_bath");
        outText.push("healthadv_some_regions_need_bath_2");
      }

      if (needDoctors > 0) {
        outText.push("healthadv_some_regions_need_doctors");
        outText.push("healthadv_some_regions_need_doctors_2");
      }

      if (needBarbers > 0) {
        outText.push("healthadv_some_regions_need_barbers");
        outText.push("healthadv_some_regions_need_barbers_2");
      }

      if (needHospital > 0) {
        outText.push("healthadv_some_regions_need_hospital");
      }

      outText.push(g_city.getProperty("healthReason"));
    }
  }

  var text = outText.length == 0
                        ? "##healthadv_unknown_reason##"
                        : outText[ Math.randomIndex(0, outText.length-1)];
  return _u(text);
}

sim.ui.advisors.health.showDetails = function(objType) {
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

sim.ui.advisors.health.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";
  w.geometry = {x:0, y:0, w:640, h:290};
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.title = _u("health_advisor");

  var blackframe = w.addLabel(35, 90, w.w-70, 90);
  blackframe.style = "blackFrame"

  var lbWorkText = w.addLabel(180, 72, 100, 20);
  lbWorkText.font = "FONT_1"
  lbWorkText.text = "##work##"

  var lbMaxAv = w.addLabel(290, 72, 180, 20)
  lbMaxAv.font = "FONT_1";
  lbMaxAv.text = "##hlth_care_of##";

  var lbCoverage = w.addLabel(480, 72, 100, 20);
  lbCoverage.font = "FONT_1"
  lbCoverage.text = "##coverage##"

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = { rc:"paneling", index:261 };

  var lbAdvice = w.addLabel(35, w.h-100, w.w-70, 80)
  lbAdvice.multiline = true;
  lbAdvice.font = "FONT_1"
  lbAdvice.text = sim.ui.advisors.health.getAdvice();

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "health_advisor";

  w.addHealthButton = function(pos, type) {
    var btn = new Button(blackframe);
    btn.geometry = {x:pos.x, y:pos.y, w:blackframe.w-pos.x*2, h:20}
    btn.style = "brownBorderOnly";

    var info = sim.ui.advisors.health.getInfo(type);

    var strBuildings = _format( "{0} {1}", info.buildingCount, _ut(info.building) );
    var strPeoples = _format( "{0} ({1}) {2}", info.peoplesServed, info.needService, _ut(info.people) );
    var strWorking = info.buildingWork;
    var descriptions = sim.ui.advisors.health.coverageDescriptions;
    var coverageStr = (info.coverage > 0 && info.buildingCount > 0)
                                   ? descriptions[ Math.clamp( info.coverage / descriptions.length, 0, descriptions.length-1 ) ]
                                   : _u("non_cvrg");

    btn.addLabel(0,   0, 120, 20, strBuildings, "FONT_1_WHITE");
    btn.addLabel(165, 0, 60,  20, strWorking,   "FONT_1");
    btn.addLabel(255, 0, 140, 20, strPeoples,   "FONT_1");
    btn.addLabel(440, 0, 70,  20, coverageStr,  "FONT_1");
  }

  var ry=5;
  w.addHealthButton({x:12, y:ry}, "clinic");
  w.addHealthButton({x:12, y:ry+20}, "baths");
  w.addHealthButton({x:12, y:ry+40}, "hospital");
  w.addHealthButton({x:12, y:ry+60}, "barber");
}
