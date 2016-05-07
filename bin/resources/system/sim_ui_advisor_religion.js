function OnShowAdvisorWindow(type) {
  switch (type) {
  case g_config.advisor.religion: sim.ui.advisors.religion.show(); break;
  case g_config.advisor.education: sim.ui.advisors.education.show(); break;

  default: sim.ui.advisors.hide();
  }
}

sim.ui.advisors.hide = function() {
  var window = g_ui.find("#advisorWindow")

  if (window != null)
    window.deleteLater();
}

sim.ui.advisors.religion = {}

sim.ui.advisors.religion.getAvice = function() {
  var advices = [];
  var houses =  g_session.city.findOverlays("house");

  var needBasicReligion = 0;
  var needSecondReligion = 0;
  var needThirdReligion = 0;
  for (var i in houses) {
    var house = houses[i].as(House);

    switch (house.spec.minReligionLevel) {
      case 1: needBasicReligion += (house.spec.religionLevel == 0 ? 1 : 0); break;
      case 2: needSecondReligion += (house.spec.religionLevel < 2 ? 1 : 0); break;
      case 3: needThirdReligion += (house.spec.religionLevel < 3 ? 1 : 0); break;
    }
  }

  var text = "religionadv_unknown_reason";
  if( !needSecondReligion && !needThirdReligion && !needBasicReligion) {
    text = "this_time_you_city_not_need_religion";

    for(var i in game.gods.roman) {
      var god = game.gods.roman[i];
      if( god.relation() < 75 ) {
        text = "religion_in_your_city_is_flourishing";
        break;
      }
    }

  } else {
    if (needBasicReligion > 0) { advices.push("religionadv_need_basic_religion" );}
    if (needSecondReligion > 0){ advices.push("religionadv_need_second_religion" ); }
    if (needThirdReligion > 0) { advices.push("religionadv_need_third_religion"); }

    if (game.gods.roman.neptune.relation() < 40) {
      advices.push("neptune_despleasure_tip");
      if (game.gods.roman.neptune.wrathPoints() > 0) {
        advices.push9("neptune_wrath_of_you");
      }
    }

    if (game.gods.roman.mars.relation() < 40) {
      advices.push("mars_watches_over_soldiers");
    }

    text = advices.length == 0
            ? "religionadv_unknown_reason"
            : advices[Math.randomIndex(0, advices.length-1)];
  }

  return text;
}

sim.ui.advisors.religion.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";

  w.geometry = {x:0, y:0, w:640, h:290};
  w.title = _u("religion_advisor");
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.mayMove = false;

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = {rc:"paneling", index:264};

  var lbBlackframe = w.addLabel(35, 64, w.w-70, 130);
  lbBlackframe.style = "blackFrame";

  var lbTemplesText = w.addLabel(268, 32, 100, 20);
  lbTemplesText.font = "FONT_1";
  lbTemplesText.text = "##temples##";

  var lbSmallText = w.addLabel(240, 47, 57, 20);
  lbSmallText.font = "FONT_1";
  lbSmallText.text = "##small##";

  var lbLargeText = w.addLabel(297, 47, 53, 20);
  lbLargeText.font = "FONT_1";
  lbLargeText.text = "##large##";

  var lbTestText = w.addLabel(370, 47, 60, 20);
  lbTestText.font = "FONT_1";
  lbTestText.text = "##test_t##";

  var lbMoodText = w.addLabel(450, 47, 100, 20);
  lbMoodText.font = "FONT_1";
  lbMoodText.text = "##rladv_mood_t##";

  var lbReligionAdvice = w.addLabel(40, w.h-95, w.w-80, 85);
  lbReligionAdvice.multiline = true;
  lbReligionAdvice.text = _u(sim.ui.advisors.religion.getAvice());
  lbReligionAdvice.font = "FONT_1";

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "religion_advisor";

  w.addReligionButton = function(pos, god) {
    var btn = new Button(lbBlackframe);
    btn.geometry = {x:pos.x, y:pos.y, w:lbBlackframe.w-pos.x*2, h:20}
    btn.style = "brownBorderOnly";

    var small_n = 0;
    var big_n = 0;
    var month2lastFest = 0;
    var shortDesc = 0;
    var wPoints = 0;
    var moodText = "";
    var godText = "";
    var relation = 0;
    if (god != null) {
      small_n = g_session.city.getOverlaysNumber(god.smallt);
      big_n = g_session.city.getOverlaysNumber(god.bigt);
      month2lastFest = god.lastFestivalDate().monthsTo(g_session.date);
      shortDesc = _format("{0}_desc", god.iname);
      wPoints = god.wrathPoints();
      moodText = god.moodDescription;
      godText = god.name();
      relation = god.relation();
    } else {
      small_n = g_session.city.getOverlaysNumber("oracle");
      //big_n = g_session.city.getOverlaysNumber();
      godText = _u("oracle");
      shortDesc = _u("all");
    }

    btn.addLabel(0, 0, 80, 20, godText, "FONT_1_WHITE");
    btn.addLabel(80, 0, 140, 20, "(" + _ut(shortDesc) + ")", "FONT_1");
    btn.addLabel(220, 0, 60, 20, small_n, "FONT_1");
    btn.addLabel(280, 0, 70, 20, big_n, "FONT_1");
    btn.addLabel(350, 0, 50, 20, _format( "{0}/{1}", month2lastFest, relation ), "FONT_1");

    var wrathImage = g_render.picture("paneling", 334);
    for (var k=0; k < wPoints / 15; k++ )
      btn.addImage(400 + k * 15, 0, wrathImage);

    if (moodText.length > 0) {
      var moodOffsetX = 410 + wPoints / 15 * 15;
      btn.addLabel(moodOffsetX, 0, 100, 20, _ut(moodText), "FONT_1" );
    }
  }

  var ry=5;
  for (var i in game.gods.roman) {
    var god = game.gods.roman[i];

    w.addReligionButton({x:12, y:ry}, god);
    ry += 20;
  }
  w.addReligionButton({x:12, y:ry}, null);
}
