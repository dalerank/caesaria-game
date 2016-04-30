function OnInitRomePantheon() {
  game.religion.initRomePantheon()
}

game.religion.initRomePantheon = function() {
  for (var i in g_config.religion.gods_config) {
    var config = g_config.religion.gods_config[i]
    var god = g_session.addGod(config.id);
    var pic = g_render.picture(config.image)
                      .fallback("dlcperf",1)
    god.setName(config.name)
    god.setService(config.service)
    god.setPicture(pic)
    god.setRelation(50)
    god.smallt = config.smallt;
    god.bigt = config.bigt;
    god.iname = config.name;

    game.gods.roman[config.id] = god;
  }
}

game.religion.getFestivalCost = function(festival) {
  var pop = g_session.city.states().population;
  return (pop / festival.limiter) + festival.minCost;
}

game.religion.assignFestival = function(who, size) {
  var festival = {};

  if (size==1) festival = g_config.festival.small;
  else if (size==2) festival = g_config.festival.middle;
  else if (size==3) festival = g_config.festival.big;

  var cost = game.religion.getFestivalCost(festival);
  g_session.city.createIssue("sundries",-cost);
  g_session.assignFestival(who, size);
}

Object.defineProperty( Divinity.prototype, "moodDescription", { get: function() {
  var _moodDescr = [ "god_wrathful", "god_irriated", "god_veryangry",
                     "god_verypoor", "god_quitepoor", "god_poor",
                     "god_displeased", "god_indifferent", "god_pleased",
                     "god_good", "god_verygood", "god_charmed", "god_happy",
                     "god_excellent", "god_exalted" ];

  var delim = 100 / _moodDescr.length;
  return _moodDescr[ math::clamp<int>( _relation.current / delim, 0, _moodDescr.size()-1 ) ];
}
