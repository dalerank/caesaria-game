function InfoboxWindow (rx,ry,rw,rh) {
  return g_ui.addSimpleWindow(rx,ry,rw,rh)
}

game.ui.infobox.updateWorkersLabel = function(w, x, y, picId, need, have) {
  w.blackFrame.setVisible(need > 0)
  if (0 == need)
    return;

  // number of workers
  w.blackFrame.icon = {rc:"paneling", index:picId}
  w.blackFrame.iconOffset = {x:20, y:10};

  w.blackFrame.text = _format( "{0} {1} ({2} {3})",
                                  have, _t("##employers##"),
                                  need, _t("##requierd##"))
}

game.ui.infobox.setAutoPosition = function(w) {
    var resolution = g_session.resolution
    var ry = ( g_ui.cursor.y < resolution.h / 2 )
                ? resolution.h - w.h - 5
                : 30;

    w.position = {x:(resolution.w-w.w)/2, y:ry}
    w.mayMove = g_session.getAdvflag("lockwindow")
}

game.ui.infobox.simple = function(rx,ry,rw,rh) {
  var ibox = new InfoboxWindow(rx,ry,rw,rh)
  ibox.title = "FONT_5"

  ibox.blackFrame = ibox.addLabel(0, 0, 0, 0)
  ibox.blackFrame.style = "blackFrame"
  ibox.blackFrame.textOffset = {x:50, y:15}
  /*{
    bgtype :
    font : "FONT_2"
    textAlign : [ "upperLeft", "upperLeft" ]
    text.offset : [ 50, 15 ]
  }*/

  ibox.btnHelp = ibox.addHelpButton(12, ibox.h-36)
  ibox.btnHelp.text = "##infobox_tooltip_help##"

  ibox.lbText = ibox.addLabel(32, 64, ibox.w-54, 64)

  return ibox
}

game.ui.infobox.aboutConstruction = function(rx,ry,rw,rh) {
  var ibox = this.simple(rx,ry,rw,rh)

  ibox.btnNext = ibox.addButton(ibox.w-36,12,24,24)
  ibox.btnNext.text = ">"
  ibox.style = "whiteBorderUp"
  ibox.tooltip = "##infobox_construction_comma_tip##"

  ibox.btnPrev = ibox.addButton(ibox.w-60,12,24,24)
  ibox.btnPrev.text = "<"
  ibox.btnPrev.bgtype = "whiteBorderUp"
  ibox.btnPrev.tooltip = "##infobox_construction_comma_tip##"

  return ibox
}

game.ui.infobox.aboutTemple = function(location) {
  var ibox = this.aboutConstruction(0,0,510,256)
  ibox.blackFrame.geometry = {x:16, y:56, w:ibox.w-32, h:56}

  var temple = g_session.city.getOverlay(location).as(Temple)
  engine.log(temple.typename)

  if (temple.typename == "oracle") {
    ibox.title = _u("oracle")
    ibox.lbText.text = _u("oracle_info")
  } else {
    var divn = temple.divinity()
    var shortDesc = divn.shortDescription()
    var text = _format( "{0}_{1}_temple",
                        temple.big ? "big" : "small",
                        divn.internalName())
    ibox.title = _ut(text) + " (" + _t(shortDesc) + ") "

    var goodRelation = divn.relation() >= 50;

    var longDescr = _format( "{0}_{1}_info",
                             divn.internalName(),
                             goodRelation ? "goodmood" : "badmood" );

    var pic = divn.picture()
    var img = ibox.addImage( (ibox.w-pic.width())/2, 140, pic)
    img.tooltip = _u(longDescr);
  }

  game.ui.infobox.updateWorkersLabel(ibox, 32, 56+12, 542, temple.maximumWorkers(), temple.numberWorkers())
  game.ui.infobox.setAutoPosition(ibox)

  ibox.setModal()
  ibox.setFocus()
}
