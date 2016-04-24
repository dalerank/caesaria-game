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
