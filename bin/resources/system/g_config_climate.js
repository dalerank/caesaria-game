g_config.climate.archives = {
  c3a : {
      central : [ "/C3.sg2" ],
      northen : [ "/C3_North.sg2" ],
      desert : [ "/C3_South.sg2" ],
    },

  rMa : {
      central : [ ":/gfx/pics_oc3.zip", ":/gfx/pics_land1a.zip" ],
      northen : [ ":/gfx/pics_north.zip"],
      desert : [ ":/gfx/pics_south.zip"]
  }
}

function OnChangeClimate(climate) { sim.climate.set(climate); }

sim.climate.set = function(climate)
{
  var climateConfig = g_session.c3mode
                        ? g_config.climate.archives.c3a
                        : g_config.climate.archives.rMa;

  var resDir = g_session.c3mode
                 ? g_session.getOptPath("c3gfx")
                 : g_session.getPath("");

  switch(climate)
  {
  case g_config.climate.central: climateConfig = climateConfig.central; break;
  case g_config.climate.northen: climateConfig = climateConfig.northen; break;
  case g_config.climate.desert : climateConfig = climateConfig.desert ; break;
  }

  for (var i in climateConfig)
  {
    engine.log( "JS: climate manager load " + climateConfig[i])
    engine.loadArchive(resDir.slice(climateConfig[i]).str, true)
  }
}
