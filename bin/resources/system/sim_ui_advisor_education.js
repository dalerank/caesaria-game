sim.ui.advisors.education = {}

sim.ui.advisors.education.param = {
  maxDescriptionNumber : 10,
  badAccessValue : 30,
  middleCoverage : 75,
  awesomeAccessValue : 100,
  awesomeCoverage : 100,
  fantasticCoverage : 150
}

sim.ui.advisors.education.coverageDescriptions = [ "edu_poor",   "edu_very_bad",
                                                   "edu_bad",    "edu_not_bad",
                                                   "edu_simple", "edu_above_simple",
                                                   "edu_good",   "edu_very_good",
                                                   "edu_pretty", "edu_awesome" ];

sim.ui.advisors.education.info = [ { type:"school",  building:"schools",   people:"children", service:g_config.service.school,  max:75,  age:"scholar_n" },
                                   { type:"academy", building:"colleges",  people:"students", service:g_config.service.academy, max:100, age:"student_n" },
                                   { type:"library", building:"libraries", people:"peoples",  service:g_config.service.library, max:800, age:"mature_n"  } ];

sim.ui.advisors.education.getTrouble = function() {
  var advices = [];
  var schoolInfo = sim.ui.advisors.education.getInfo( "school" );
  var collegeInfo = sim.ui.advisors.education.getInfo( "academy" );
  var libraryInfo = sim.ui.advisors.education.getInfo( "library" );

  if( schoolInfo.need == 0 && collegeInfo.need == 0 && libraryInfo.need == 0 ) {
    return "not_need_education";
  }

  var param = sim.ui.advisors.education.param;

  if( schoolInfo.nextLevel > 0 ) { advices.push( "have_no_access_school_colege" ); }
  if( libraryInfo.nextLevel > 0 ) { advices.push( "have_no_access_to_library" ); }

  if( schoolInfo.minAccessLevel < param.badAccessValue
      || collegeInfo.minAccessLevel < param.badAccessValue ) {
    advices.push( "edadv_need_better_access_school_or_colege" );
  }

  if( schoolInfo.coverage < param.middleCoverage
      && collegeInfo.coverage < param.middleCoverage
      && libraryInfo.coverage < param.middleCoverage ) {
    advices.push( "need_more_access_to_lbr_school_colege" );
  }

  if( schoolInfo.coverage < param.middleCoverage ) {
    advices.push( "need_more_school_colege" );
  } else if( schoolInfo.coverage >= param.awesomeCoverage
             && schoolInfo.coverage < param.fantasticCoverage ) {
    advices.push( "school_access_perfectly" );
  }

  if( collegeInfo.coverage >= param.awesomeCoverage && collegeInfo.coverage < param.fantasticCoverage ) {
    advices.push( "colege_access_perfectly" );
  }

  if( libraryInfo.coverage < param.middleCoverage ) {
    advices.push( "need_more_access_to_library" );
  } else if( libraryInfo.coverage > param.awesomeCoverage && libraryInfo.coverage < param.fantasticCoverage ) {
    advices.push( "library_access_perfectrly" );
  }

  if( libraryInfo.minAccessLevel < param.badAccessValue ) {
    advices.push("some_houses_need_better_library_access");
  }

  if( libraryInfo.nextLevel > 0 && collegeInfo.nextLevel > 0 ) {
    advices.push( "some_houses_need_library_or_colege_access" );
  }

  return advices.length == 0 ? "education_awesome" : advices[ Math.randomIndex(0, advices.length-1) ];
}

sim.ui.advisors.education.getCityInfo = function() {
  var sumScholars = 0;
  var sumStudents = 0;
  var houses = g_session.city.findOverlays("house");

  for (var i in houses) {
    var house = houses[i];
    sumScholars += house.getProperty("scholar_n");
    sumStudents += house.getProperty("student_n");
  }

  return _format( "{0} {1}, {2} {3}, {4} {5}",
                  g_session.city.population, _ut("people"),
                  sumScholars, _ut("scholars"),
                  sumStudents, _ut("students") );
}

sim.ui.advisors.education.showDetails = function(objType) {
  var w = g_ui.addSimpleWindow(0, 0, 480, 600);

  var blackFrame = ibox.addLabel(15, 15, w.w-30, w.h-50);
  blackFrame.style = "blackFrame";

  var buildings = g_session.city.findOverlays(objType);
  var ry = 5;
  for (var i in buildings) {
    var bld = buildings[i].as(EducationBuilding);
    var btn = new Button(blackFrame);
    btn.geometry = { x:5, y:ry, w:blackFrame.w-10, h:20};

    var text = bld.name;
    if (text.length == 0) {
      var pos = bld.pos;
      text = _format( "{0} [{1},{2}]", bld.typename, pos.i, pos.j);
    }

    btn.addLabel(0, 0, btn.w/2, btn.h, text);

    text = _format( "workers {0}/{1}, served {2}", bld.numberWorkers, bld.maximumWorkers, bld.currentVisitors );
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

sim.ui.advisors.education.findInfo = function(bType) {
  for ( var i in sim.ui.advisors.education.info) {
    var config = sim.ui.advisors.education.info[i];
    if (config.type == bType) {
      engine.log("Config found for " + bType)
      return config;
    }
  }

  return null;
}

sim.ui.advisors.education.getInfo = function(bType) {
  var info = sim.ui.advisors.education.findInfo(bType);

  var ret = {};
  for (var i in info) {
    ret[i] = info[i];
  }

  ret.buildingWork = 0;
  ret.peoplesStuding = 0;
  ret.buildingCount = 0;
  ret.need = 0;
  ret.nextLevel = 0;
  ret.coverage = 0;

  var servBuildings = g_session.city.findOverlays(bType);

  ret.buildingCount = servBuildings.length;
  for (var i=0; i < servBuildings.length; ++i) {
    var serv = servBuildings[i].as(EducationBuilding);
    if (serv && serv.numberWorkers > 0) {
      ret.buildingWork++;
      ret.peoplesStuding += ret.maxStudy * serv.numberWorkers / serv.maximumWorkers;
    }
  }

  var houses = g_session.city.findOverlays("house");
  var param = sim.ui.advisors.education.param;
  var minAccessLevel = param.awesomeAccessValue;

  for (var i in houses ) {
    var house = houses[i];

    if (!house.habitable)
      continue;

    ret.need += ( house.getHabitantsNumber(ret.age) * ( house.isEducationNeed(ret.service) ? 1 : 0 ) );
    ret.nextLevel += (house.isEvolveEducationNeed(ret.service) == param.awesomeAccessValue ? 1 : 0);
    minAccessLevel = Math.min(house.getServiceValue(ret.service), minAccessLevel);
  }

  ret.coverage = ret.need == 0
                    ? param.awesomeAccessValue
                    : Math.percentage( ret.peoplesStuding, ret.need );
  return ret;
}

sim.ui.advisors.education.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";

  w.geometry = {x:0, y:0, w:640, h:256};
  w.title = _u("education_advisor_title");
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.mayMove = false;

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = {rc:"paneling", index:262};

  var lbBlackframe = w.addLabel(35, 80, w.w-70, 70);
  lbBlackframe.style = "blackFrame";

  var lbWorkText = w.addLabel(180, 60, 70, 25);
  lbWorkText.font = "FONT_1";
  lbWorkText.text = "##work##";

  var lbMaxAvailableText = w.addLabel(290, 60, 160, 25);
  lbMaxAvailableText.font = "FONT_1";
  lbMaxAvailableText.text = "##max_teach##";

  var lbCoverageText = w.addLabel(480, 60, 120, 25);
  lbCoverageText.font = "FONT_1";
  lbCoverageText.text = "##coverage##";

  var lbCityInfo = w.addLabel(65, 40, 550, 30);
  lbCityInfo.font = "FONT_1";
  lbCityInfo.text = _u(sim.ui.advisors.education.getCityInfo());

  var lbTroubleInfo = w.addLabel(40, w.h-90, w.w-80, 85);
  lbTroubleInfo.multiline = true;
  lbTroubleInfo.font = "FONT_1";
  //lbTroubleInfo.text = _u(sim.ui.advisors.education.getTrouble());

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "education_advisor";

  w.addEducationButton = function(pos, type) {
    var btn = new Button(lbBlackframe);
    btn.geometry = {x:pos.x, y:pos.y, w:lbBlackframe.w-pos.x*2, h:20}
    btn.style = "brownBorderOnly";

    var info = sim.ui.advisors.education.getInfo(type);

    var strBuildings = _format( "{0} {1}", info.buildingCount, _ut(info.building) );
    var strPeoples = _format( "{0} {1}", info.peoplesStuding, _ut(info.people) );
    var strWorking = info.buildingWork;
    var descriptions = sim.ui.advisors.education.coverageDescriptions;
    var coverageStr = (info.coverage > 0 && info.buildingCount > 0)
                                   ? descriptions[ Math.clamp( info.coverage / descriptions.length, 0, descriptions.length-1 ) ]
                                   : _u("non_cvrg");

    btn.addLabel(0,   0, 120, 20, strBuildings, "FONT_1_WHITE");
    btn.addLabel(255, 0, 140, 20, strPeoples,   "FONT_1");
    btn.addLabel(165, 0, 60,  20, strWorking,   "FONT_1");
    btn.addLabel(440, 0, 70,  20, coverageStr,  "FONT_1");
  }

  var ry=5;
  w.addEducationButton({x:12, y:ry}, "school");
  w.addEducationButton({x:12, y:ry+20}, "academy");
  w.addEducationButton({x:12, y:ry+40}, "library");
}
