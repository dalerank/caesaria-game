function OnDevtextSplashScreen() {
  splash.fillDevText()
}

function OnStartSplashScreen() {
  if (splash.execute)
    splash.execute();
}

splash.execute = function() {
  g_session.setOption("background", "logo")
}

splash.fillDevText = function()
{
  var text = "This is a development version of CaesarIA!\n" +
             "therefore this game still has a lot of bugs and some features are not complete!\n" +
             "This version is not tested, as well, be aware of that\n" +
             "You can support the development of this game at\n" +
             " www.caesaria.net\n" +
             "If you encounter bugs or crashes please send us a report"
  g_seesion.setOption("dev-text", text)
}
