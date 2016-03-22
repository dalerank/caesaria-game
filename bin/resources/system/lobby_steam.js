lobby.steam = {}

function OnReceivedSteamUserName(steamName) {
  engine.log( "User logged as " + steamName)
  lobby.steam.receivedUserName(steamName)
}

function OnReceivedSteamStats() {
  lobby.steam.receivedState()
}

lobby.steam.receivedUserName = function(name) {
  var lastName = engine.getOption("playerName")
  if (lastName.length==0)
     engine.setOption("playerName",name)

  if (name.length == 0)
  {
    g_session.showSysMessage("Error", "Can't login in Steam")
    g_session.setMode(g_config.lobby.closeApplication)
  }

  var text = _format( "Build {0}\n{1}", engine.getOption("buildNumber"), name )
  var lbSteamName = g_ui.addLabel(100, 10, 300, 70)
  lbSteamName.text = text
  lbSteamName.textAlign = { h:"upperLeft", v:"center" }
  lbSteamName.multiline = true;
  lbSteamName.font = "FONT_3_WHITE";

  var image = g_session.getSteamUserImage();
  g_ui.addImage(20, 20, image)
}

lobby.steam.receivedState = function() {
  var offset = 0;

  for(var i in g_config.steam.achievents)
  {
    var achievent = g_config.steam.achievents[i]
    if (g_session.isSteamAchievementReached(achievent.id))
    {
      var pic = g_session.getSteamAchievementImage(achievent.id);
      if (pic.valid)
      {
        var img = g_ui.addImage(10, 100 + offset, pic);
        img.setTooltipText(achievent.title);
        offset += 65;
      }
    }
  }
}
