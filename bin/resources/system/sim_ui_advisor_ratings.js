sim.ui.advisors.ratings = {}

sim.ui.advisors.ratings.calcCoverage = function(type)
{
   var visitors = 0;
   var population = g_city.population;
   if (population == 0) {
     return 0;
   }

   var objects = g_city.findOverlays(type);

   var objects_n = objects.size();

   for (var i=0; i<objects.length; i++) {
     visitors += objects[i].getProperty("currentVisitors");
   }

   return {coverage: visitors/population, number: objects_n};
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
    var info = this.coverage(t);
    if( info.number == 0 ) {
      var troubleDesc = _format( "##haveno_{0}_in_city##", t);
      troubles.push(troubleDesc);
    } else if( coverage < 100 ) {
      var troubleDesc = _format( "##have_less_{0}_in_city_{1}##", t, Math.floor(coverage / 50) );
      troubles.push(troubleDesc);
    }
  }

  return _u( troubles[Math.randomIndex(0, troubles.length-1)] );
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
  w.title = _u("wnd_ratings_title");

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = { rc:"paneling", index:258 };

  var lbRatingInfo = w.addLabel(66, 360, 500, 40)
  lbRatingInfo.style = "blackFrame"
  lbRatingInfo.font = "FONT_1"
  lbRatingInfo.multiline = true
  lbRatingInfo.textAlign = {h:"upperLeft", v:"upperLeft"};
  lbRatingInfo.textOffset = {x:5, y:5}
  lbRatingInfo.text = "##click_on_rating_for_info##";

  w.addRatingButton = function(x, y, tx, tlp, value, target) {
    var btn = w.addButton(x, y, 108, 65);
    btn.textAlign = {h:"center", v:"upperLeft"}
    btn.text = tx;
    btn.tooltip = tlp;

    btn.addLabel(0, 15, btn.w, btn.h-30, value, "FONT_4");
    var targetText = _format( "{0} {1}", target, _ut("wndrt_need") );
    btn.addLabel(0, btn.h-15, btn.w, 15, targetText, "FONT_1");
  }

  w.RatingColumn = function(center,value) {
    var columnStartY = 275;
    var footer = g_render.picture( "paneling", 544 );
    var header g_render.picture( "paneling", 546 );
    var body g_render.picture( "paneling", 545 );

    for (int i=0; i < value; i++) {
      var icon = w.addLabel(center.x - body.w / 2, -columnStartY + (10 + i * 2));
      icon.icon = body;
    }

    var icon = w.addLabel(center.x - footer.w / 2, -columnStartY + footer.h);
    icon.icon = footer;

    if (value >= 50) {
      var icon = w.addLabel(center.x - header.w / 2, -columnStartY + (10 + value * 2));
      icon.icon = header;
    }
  }

  var btnCulture = w.addRatingButton(80, 290, _u("wndrt_culture"), _u("wndrt_culture_tooltip"));
  btnCulture.callback = function() {
    lbRatingInfo.text = this.checkCultureRating();
  }

  var btnProsperity = w.addRatingButton(200, 290, _u("wndrt_prosperity"), _u("wndrt_prosperity_tooltip")):
  var btnPeace = w.addRatingButton(320, 290, _u("wndrt_peace"), _u("wndrt_peace_tooltip"));
  var btnFavour = w.addRatingButton(440, 290, _u("wndrt_favour"), _u("wndrt_favour_tooltip"));

  var imgBg = w.addImage(60, 50, "panelwindows_00027")

  var lbNeedPopulation = w.addLabel(225, 15, w.w-240, 20)
  lbNeedPopulation.font = "FONT_2"
}
