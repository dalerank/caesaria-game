game.sound.player = {}
game.sound.player.index = 0
game.sound.player.items = [ "rome1", "rome2", "rome3", "rome4", "rome5", "rome6", "rome7", "rome8" ]

function OnThemePlayFinished() { game.sound.player.themeFinished() }

game.sound.player.themeFinished = function()
{
  if (this.index>=this.items.length)
    this.index = 0;

  var sample = this.items[this.index]
  g_session.playAudio(sample,100,g_config.audio.theme)
  g_session.addWarningMessage( "##theme_" + sample + "##" )

  this.index++
}

game.sound.player.playeAmbient = function() {

}
