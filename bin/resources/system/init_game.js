game.setLanguage = function(config)
{
  var currentFont = engine.getOption("font");

  if (!config.talks)
    config.talks = ":/audio/wavs_citizen_en.zip";

  g_session.setLanguage(config.ext,config.talks);

  if (config.font !== undefined && currentFont !== config.font)
  {
    engine.setOption("font",config.font);
    g_session.setFont(config.font);
  }
}
