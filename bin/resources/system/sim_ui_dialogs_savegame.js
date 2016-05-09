
sim.ui.dialogs.savegame = function() {
  var w = g_ui.addWindow(0, 0, 512, 384);

  engine.log("Find save in " + g_session.savedir.str + " with ext " + g_config.saves.ext)
  if (!g_session.savedir.exist()) {
    g_ui.addInformationDialog( _u("warning"), _u("save_directory_not_exist"));
    return;
  }

  w.title = _u("save_city")
  w.directory = g_session.savedir;
  w.extension = g_config.saves.ext;

  w.edFilename = w.addEditbox(20, 40, w.w-40, 30);
  w.edFilename.font = "FONT_2";
  w.edFilename.textOffset = {x:10, y:0}
  w.edFilename.text = "savecity";
  w.realFilename = g_session.savedir + "/savecity.oc3save";

  var lbxSaves = w.addFileSelector(20, 75, w.w-40, w.h - 115);
  lbxSaves.itemsFont = "FONT_2"
  lbxSaves.background = true;
  lbxSaves.itemColor = { simple: "0xffffffff", hovered : "0xff000000" }

  var lbContinue = w.addLabel( 18, w.h-40, 240, 26);
  lbContinue.font = "FONT_2"
  lbContinue.textAlign = { v:"center", h:"lowerRight" };
  lbContinue.text = _u("savedlg_continue");

  var btnOk = w.addTexturedButton(265, w.h-40, 39, 26);
  btnOk.states = { rc: "paneling", normal: 239, hover: 240, pressed: 241, disabled: 242 };
  btnOk.tooltip = _u("save_game_here");
  btnOk.callback = function() {
    var rpath = g_fs.getPath(w.realFilename);
    engine.log(rpath.str);
    if (rpath.exist()) {
      var dialog = g_ui.addConfirmationDialog( _u("warning"), _u("save_already_exist") );
      dialog.onYesCallback = function() {
        g_session.save(rpath.str);
        w.deleteLater();
      }
    } else {
      g_session.save(rpath.str);
      w.deleteLater();
    }
  }

  var btnCancel = w.addTexturedButton(306, w.h-40, 39, 26);
  btnCancel.states = { rc: "paneling", normal: 243, hover: 244, pressed: 245, disabled: 246 };
  btnCancel.callback = function() {
    w.deleteLater();
  }

  var flist = g_fs.getFiles(w.directory, w.extension);
  lbxSaves.items = flist;

  lbxSaves.onSelectedCallback = function(index) {
    w.edFilename.text = g_fs.getPath(flist[index]).baseName().str;
    w.realFilename = flist[index];
  }

  w.moveToCenter();
  w.mayMove = false;
  w.setModal();
  w.closeAfterKey( {escape:true, rmb:true} );
  w.pauseGame = true;
}
