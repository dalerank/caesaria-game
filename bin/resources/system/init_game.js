game.setLanguage = function (config) {
    var currentFont = engine.getOption("font");

    if (!config.talks)
        config.talks = ":/audio/wavs_citizen_en.zip";

    g_session.setLanguage(config.ext, config.talks);

    if (config.font !== undefined && currentFont !== config.font) {
      var fontpath = g_fs.getPath(config.font);

      if (fontpath.exist()) {
        engine.setOption("font", config.font);
        g_session.setFont(config.font);
      } else {
        g_ui.addInformationDialog( "", _u("not_found_font_file"));
      }
    }
}
