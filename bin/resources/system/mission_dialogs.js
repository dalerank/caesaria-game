function OnShowFilmWidget(fname, text, title) {
  sim.ui.filmwidget.show(fname, text, title)
}

sim.ui.filmwidget.show = function(filename, text, title) {
  var w = g_ui.addSimpleWindow(0, 0, 415, 450)
  w.title = "##title##"
  var helpButton = w.addHelpButton(16, w.h-42)

  var date = g_session.date
  var gbox = w.addGroupbox(12, w.h-100, w.w-24, 85)
  var lbTime = gbox.addLabel(0, 0, 150, 20)
  lbTime.font = "FONT_1"
  lbTime.text = _format( "{} {} {}", date.nameMonth
                                     date.year(),
                                     date.age ) //.year() < 0 ? "BC" : "AD"

  var lbReceiver = gbox.addLabel(150, 0, w.w-150, 20)
  lbReceiver.font = "FONT_1"

  var lbMessage = gbox.addLabel(0, 20, w.w, 90)
  lbMessage.font = "FONT_1"
  lbMessage.multiline = true
  lbMessage.text = text
  lbMessage.margin = { left:4, right:4 }

  var path = g_config.movie.getPath(filename)
  if (path.exist())
  {
    var player = w.addSmkViewer(12, 12, w.w - 23, 292);
    player.filename = path
  }
  else {
    var pic = g_render.picture(movieName, 1)
                      .withFallback("freska", 1)

    var img = w.addImage(12, 12, w.w-24,292)
    img.picture = pic
    img.mode = Image.fit
  }
}
