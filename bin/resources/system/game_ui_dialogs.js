game.ui.dialogs.filmwidget = {}

function OnShowFilmWidget(fname, text, title) {
  game.ui.dialogs.filmwidget.show(fname, text, title)
}

function type(obj){
    return Object.prototype.toString.call(obj).slice(8, -1);
}

game.ui.dialogs.filmwidget.show = function(filename, text, title) {
  var w = g_ui.addSimpleWindow(0, 0, 415, 450)
  w.mayMove = false
  w.setModal()
  w.pauseGame = true

  var lbTitle = w.addLabel( 40, w.h - 40, w.w - 80, 25)
  lbTitle.font = "FONT_1"
  lbTitle.text = title
  lbTitle.textAlign = { h:"center", v:"center" }

  var helpButton = w.addHelpButton(14, w.h-36)

  var date = g_session.date
  var gbox = w.addGroupbox(14, w.h-130, w.w-28, 90)
  var lbTime = gbox.addLabel(0, 0, 150, 20)
  lbTime.font = "FONT_1"
  lbTime.text = _format( "{0} {1} {2}", date.nameMonth,
                                        date.absYear,
                                        date.age ) //.year() < 0 ? "BC" : "AD"

  var lbReceiver = gbox.addLabel(150, 0, w.w-150, 20)
  lbReceiver.font = "FONT_1"

  var lbMessage = gbox.addLabel(0, 20, w.w, 90)
  lbMessage.font = "FONT_1"
  lbMessage.multiline = true
  lbMessage.text = text
  lbMessage.padding = { left:6, top:0, right:6, bottom:0 }

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

game.ui.dialogs.requestExit = function()
{
  var dialog = g_ui.addConfirmationDialog( "", "##exit_without_saving_question##" );
  dialog.onYesCallback = function() { g_session.setMode(5); }
}

game.ui.dialogs.savegame = function()
{
  var savedir = g_session.getOptPath("savedir");
  var ext = engine.getOption("saveExt");

  engine.log("Find save in " + savedir.str + " with ext " + ext)
  if (!savedir.exist())
  {
    g_ui.addInformationDialog("##warning##", "##save_directory_not_exist##");
    return;
  }

  var dialog = g_ui.addSaveGameDialog(savedir, ext);
  dialog.callback = function(path) {
              g_session.save(path);
          }
}
