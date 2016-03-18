g_config.movie = {}

g_config.movie.folder = ":/smk"
g_config.movie.extension = ".smk"

g_config.movie.items = {
  population1 : "population1",
  population2 : "population2",
  first_glad    : "1st_glad",
  spy_barbarian : "spy_barbarian",
  emp_2nd_chance : "emp_2nd_chance",
  emp_displeased : "emp_displeased",
  emp_angry : "emp_angry",
  city_fire : "city_fire",
  urgent_message2 : "urgent_message2",
  quake : "quake",
  festival1_feast : "festival1_feast",
  festival3_glad : "festival3_glad",
  festival2_chariot : "festival2_chariot",
  god_mercury : "god_mercury",
  god_mars : "god_mars",
  god_ceres : "god_ceres",
  god_neptune : "god_neptune",
  god_venus : "god_venus",
  sick : "sick"
}

g_config.movie.getPath = function(alias) {
  var folders = [ g_config.movie.folder,
                  g_session.c3video ]

  if (g_config.movie.items.hasOwnProperty(alias))
    alias = g_config.movie.items[alias]

  var folder = new Directory()
  for (var i in folders)
  {
    folder.set(folders[i])
    var result = folder.findWithName(alias)
    if (result.str.length>0)
      return result;
  }

  return null;
}
