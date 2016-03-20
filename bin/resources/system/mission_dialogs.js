game.ui.dialogs.filmwidget = {}

function OnShowFilmWidget(fname, text, title) {
  game.ui.dialogs.filmwidget.show(fname, text, title)
}

function type(obj){
    return Object.prototype.toString.call(obj).slice(8, -1);
}

game.ui.dialogs.filmwidget.show = function(filename, text, title) {
  var w = g_ui.addSimpleWindow(0, 0, 415, 450)
  w.title = "##title##"
  w.mayMove = false
  w.setModal()
  
  var helpButton = w.addHelpButton(11, w.h-34)

  var date = g_session.date
  var gbox = w.addGroupbox(14, w.h-130, w.w-28, 90)
  var lbTime = gbox.addLabel(0, 0, 150, 20)
  lbTime.font = "FONT_1"
  lbTime.text = _format( "{0} {1} {2}", date.nameMonth,
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
  if (path && path.exist()) {
    //var player = w.addSmkViewer(12, 12, w.w - 23, 292);
    //player.filename = path
  } else {
    var pic = g_render.picture(filename, 1)
                      .fallback("freska", 1)

    engine.log("set picture " + pic.name())
    var img = w.addImage(12, 12, w.w-24, 292, pic)
    img.mode = "fit"
  }
}
