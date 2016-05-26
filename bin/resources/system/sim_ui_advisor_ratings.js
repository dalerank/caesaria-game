sim.ui.advisors.ratings = {}

sim.ui.advisors.ratings.calcCoverage = function(type)
{
   var visitors = 0;
   var population = g_city.population;
   if (population == 0) {
     return 0;
   }

   var objects = g_city.findOverlays(type);

   for (var i=0; i<objects.length; i++) {
     visitors += objects[i].getProperty("currentVisitors");
   }

   return {coverage: visitors/population, number: objects.length};
}

sim.ui.advisors.ratings.checkFavourRating = function() {
  var problems = [];

  var brokenEmpireTax = g_city.getIssueValue( "overdueEmpireTax", g_config.history.lastYear );
  if( brokenEmpireTax > 0 )
  {
    var twoYearsAgoBrokenTax = g_city.getIssueValue( "overdueEmpireTax", g_config.history.twoYearsAgo );

    if( twoYearsAgoBrokenTax > 0 ) { problems.push("##broke_empiretax_with2years_warning##"); }
    else { problems.push("##broke_empiretax_warning##"); }
  }

  var current = g_session.player.rank();
  var availableSalary = g_config.ranks[current].salary;
  var salaryKoeff = g_session.player.salary()/availableSalary;

  if( salaryKoeff >= 3 )       { problems.push("##high_salary_angers_senate##");  }
  else if( salaryKoeff > 2.5 ) { problems.push("##more_salary_dispeasure_senate##");  }
  else if( salaryKoeff > 2 )   { problems.push("##try_reduce_your_high_salary##"); }
  else if( salaryKoeff > 1.5 ) { problems.push("##try_reduce_your_salary##"); }
  else if( salaryKoeff > 1 )   { problems.push("##your_salary_frowned_senate##"); }

  var lastFavor = g_city.getYearParam(g_config.history.lastYear,g_config.cityparam.favour);
  var currentFavor = g_city.getYearParam(g_config.history.thisYear,g_config.cityparam.favour);

  if (currentFavor == lastFavor)   {    problems.push("##your_favour_unchanged_from_last_year##");  }
  else if(currentFavor > lastYearFavor) { problems.push("##your_favour_increased_from_last_year##"); }

  if (currentFavor < 30) { problems.push("##your_favor_is_dropping_catch_it##"); }
  else if(currentFavor > 90) { problems.push("##emperoradv_caesar_has_high_respect_for_you##"); }

  if (g_city.haveCanceledRequest) {
    problems.push("##imperial_request_cance_badly_affected##");
    problems.push("##request_failed##");
  }

  if (problems.length==0) {
    problems.push("##no_favour_problem##");
  }

  return problems[Math.randomIndex(0, problems.length-1)];
}

sim.ui.advisors.ratings.checkCultureRating = function()
{
  var culture = g_city.culture

  if (culture == 0) {
    return _u("no_culture_building_in_city");
  }

  var troubles = [];
  var types = [ "school", "library", "academy", "temple", "theater" ];
  for (var i=0; i<types.length; i++) {
    var t = types[i];
    var info = sim.ui.advisors.ratings.calcCoverage(t);
    if( info.number == 0 ) {
      var troubleDesc = _format( "##haveno_{0}_in_city##", t);
      troubles.push(troubleDesc);
    } else if( info.coverage < 100 ) {
      var troubleDesc = _format( "##have_less_{0}_in_city_{1}##", t, Math.floor(info.coverage / 50) );
      troubles.push(troubleDesc);
    }
  }

  return _u( troubles[Math.randomIndex(0, troubles.length-1)] );
}

sim.ui.advisors.ratings.checkProsperityRating = function() {
  var text = "";
  var troubles = [];
  var currentProsperity = g_city.prosperity;
  if (currentProsperity == 0) {
    return "##cant_calc_prosperity##";
  }

  var lastProsperity = g_city.getYearParam(g_config.history.lastYear,g_config.cityparam.prosperity);
  var currentProsperity = g_city.getYearParam(g_config.history.thisYear,g_config.cityparam.prosperity);

  if( currentProsperity > lastProsperity ) { troubles.push("##your_prosperity_raising##"); }

    if( g_city.getProsperityMark( g_config.prosperity.housesCap ) < 0 ) { troubles.push("##bad_house_quality##"); }
    if( g_city.getProsperityMark( g_config.prosperity.haveProfit ) == 0 ) { troubles.push("##lost_money_last_year##"); }
    if( g_city.getProsperityMark( g_config.prosperity.worklessPercent ) > g_config.labor.highWorkless ) { troubles.push("##high_workless_number##"); }
    if( g_city.getProsperityMark( g_config.prosperity.workersSalary ) < 0 ) { troubles.push("##workers_salary_less_then_rome##"); }
    if( g_city.getProsperityMark( g_config.prosperity.plebsPercent ) > g_config.labor.muchPlebsPercent ) { troubles.push("##much_plebs##"); }
    if( g_city.getProsperityMark( g_config.prosperity.changeValue ) == 0 ) {
      troubles.push("##no_prosperity_change##");
      troubles.push("##how_to_grow_prosperity##");
    }
    if( currentProsperity > g_config.ratings.amazinProsperity ) { troubles.push("##amazing_prosperity_this_city##"); }

    var payDiff = g_city.getParam(g_config.cityparam.payDiff);
    if (payDiff > 0) { troubles.push("##prosperity_lack_that_you_pay_less_rome##"); }

    var caesarsHelper = g_city.getIssueValue( "caesarsHelp", g_config.history.thisYear );
    caesarsHelper += g_city.getIssueValue( "caesarsHelp", g_config.history.lastYear );
    if( caesarsHelper > 0 ) {
      troubles.push("##emperor_send_money_to_you_nearest_time##");
    }

    text = troubles.length == 0
                        ? "##good_prosperity##"
                        : troubles[Math.randomIndex(0, troubles.length-1)];

    return text;
}

sim.ui.advisors.ratings.checkPeaceRating = function() {
  var advices = [];
  var peace = g_city.peace;

  if (g_city.chastenerThreat) {
    advices.push("##city_under_rome_attack##");
  } else if(g_city.barbarianThreat) {
    advices.push("##city_under_barbarian_attack##");
  } else {
    if( g_city.monthFromLastAttack < 36 ) {
      advices.push("##province_has_peace_a_short_time##");
    }

    if( peace > g_config.ratings.peaceLongTime ) { advices.push("##your_province_quiet_and_secure##"); }
    else if( peace > 80 ) { advices.push("##overall_city_become_a_sleepy_province##"); }
    else if( peace > 70 ) { advices.push("##its_very_peacefull_province##"); }
    else if( peace > 60 ) { advices.push("##this_province_feels_peaceful##"); }
    else if( peace > g_config.ratings.peaceAverage ) { advices.push("##this_lawab_province_become_very_peacefull##"); }
  }

  var peaceReason = g_city.peaceReason;
  if (peaceReason.length>0)
    advices.push(peaceReason);

  if (advices.length==0) {
    advices.push("##peace_rating_text##");
  }

  return advices[Math.randomIndex(0, advices.length-1)];
}

sim.ui.advisors.ratings.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";
  w.geometry = {x:0, y:0, w:640, h:432};
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.mayMove = false;
  w.title = _u("wnd_ratings_title");

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = { rc:"paneling", index:258 };

  var lbRatingInfo = w.addLabel(46, 360, 560, 55);
  lbRatingInfo.style = "blackFrame"
  lbRatingInfo.font = "FONT_1"
  lbRatingInfo.name = "#ratingInfo";
  lbRatingInfo.multiline = true
  lbRatingInfo.textAlign = {h:"upperLeft", v:"upperLeft"};
  lbRatingInfo.textOffset = {x:5, y:2}
  lbRatingInfo.text = "##click_on_rating_for_info##";

  w.addRatingButton = function(x, y, tx, tlp, value, target) {
    var btn = w.addButton(x, y, 108, 65);
    btn.textAlign = {h:"center", v:"upperLeft"}
    btn.text = tx;
    btn.style = "whiteBorderUp";
    btn.tooltip = tlp;

    var lb = btn.addLabel(0, 17, btn.w, btn.h-30, value, {size:32, bold:true, color:"black"});
    lb.textAlign = {v:"center", h:"center"}
    var targetText = _format( "{0} {1}", target, _ut("wndrt_need") );
    lb = btn.addLabel(0, btn.h-15, btn.w, 15, targetText, {size:14, color:"black"});
    lb.textAlign = {v:"center", h:"center"}

    return btn;
  }

  w.addRatingColumn = function(x,value) {
    var columnStartY = 275;
    var footer = g_render.picture( "paneling", 544 );
    var header = g_render.picture( "paneling", 546 );
    var body = g_render.picture( "paneling", 545 );

    for (var i=0; i < value; i++) {
      var icon = w.addLabel(x - body.w / 2, -columnStartY + (10 + i * 2));
      icon.icon = body;
    }

    var icon = w.addLabel(x - footer.w / 2, -columnStartY + footer.h);
    icon.icon = footer;

    if (value >= 50) {
      var icon = w.addLabel(x - header.w / 2, -columnStartY + (10 + value * 2));
      icon.icon = header;
    }
  }

  var wint = g_city.victoryConditions();
  w.addRatingColumn(130, g_city.culture);
  var btnCulture = w.addRatingButton(80, 290, _u("wndrt_culture"), _u("wndrt_culture_tooltip"),
                                     g_city.culture, wint.culture);
  btnCulture.callback = function() {
    g_ui.find("#ratingInfo").text = sim.ui.advisors.ratings.checkCultureRating();
  }

  w.addRatingColumn(250, g_city.prosperity);
  var btnProsperity = w.addRatingButton(200, 290, _u("wndrt_prosperity"), _u("wndrt_prosperity_tooltip"),
                                        g_city.prosperity, wint.prosperity);
  btnProsperity.callback = function() {
    g_ui.find("#ratingInfo").text = sim.ui.advisors.ratings.checkProsperityRating();
  }

  w.addRatingColumn(370, g_city.peace);
  var btnPeace = w.addRatingButton(320, 290, _u("wndrt_peace"), _u("wndrt_peace_tooltip"),
                                   g_city.peace, wint.peace);
  btnPeace.callback = function() {
    g_ui.find("#ratingInfo").text = sim.ui.advisors.ratings.checkPeaceRating();
  }

  w.addRatingColumn(490, g_city.favour);
  var btnFavour = w.addRatingButton(440, 290, _u("wndrt_favour"), _u("wndrt_favour_tooltip"),
                                    g_city.favor, wint.favour);
  btnFavour.callback = function() {
    g_ui.find("#ratingInfo").text = sim.ui.advisors.ratings.checkFavourRating();
  }

  var imgBg = w.addImage(60, 50, "panelwindows_00027")

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "ratings_advisor";

  var lbNeedPopulation = w.addLabel(60, 30, 240, 20)
  lbNeedPopulation.font = {size:14, color:"black"}
  lbNeedPopulation.text = _format( "{0} {1} ({2} {3}",
                                    _ut("population"), g_city.population,
                                    wint.population, _ut("need_population"));
}
