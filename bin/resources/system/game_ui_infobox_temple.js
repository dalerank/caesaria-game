game.ui.infobox.simple = function(rx,ry,rw,rh) {
  var ibox = g_ui.addSimpleWindow(rx,ry,rw,rh)
  ibox.title = "FONT_5"

  ibox.blackFrame = ibox.addLabel(0, 0, 0, 0)
  ibox.blackFrame.style = "blackFrame"
  ibox.blackFrame.textOffset = {x:50, y:15}

  ibox.btnHelp = ibox.addHelpButton(12, ibox.h-36)
  ibox.btnHelp.text = "##infobox_tooltip_help##"

  //ibox.lbText = ibox.addLabel(32, 64, ibox.w-54, 64)

  ibox.setAutoPosition = function() {
      var resolution = g_session.resolution
      var ry = ( g_ui.cursor.y < resolution.h / 2 )
                  ? resolution.h - ibox.h - 5
                  : 30;

      ibox.position = {x:(resolution.w-ibox.w)/2, y:ry}
      ibox.mayMove = g_session.getAdvflag("lockwindow")
  }

  return ibox
}

game.ui.infobox.updateWorkingLabel = function(ibox,active) {
  if (!ibox.btnToggleWorks)
  {
    ibox.btnToggleWorks = new Button(ibox.blackFrame);
    ibox.btnToggleWorks.geometry = { x:ibox.blackFrame.w-110, y:(ibox.blackFrame.h-25)/2, w:100, h:25 }
    ibox.btnToggleWorks.style = "blackBorderUp"
    ibox.btnToggleWorks.font = "FONT_1"
  }

  ibox.btnToggleWorks.text = active ? _u("abwrk_working") : _u("abwrk_not_working")
}

game.ui.infobox.updateWorkersLabel = function(ibox, x, y, picId, need, have) {
  ibox.blackFrame.setVisible(need > 0)
  if (0 == need)
    return;

  // number of workers
  ibox.blackFrame.icon = {rc:"paneling", index:picId}
  ibox.blackFrame.iconOffset = {x:20, y:10};

  ibox.blackFrame.text = _format( "{0} {1} ({2} {3})",
                                  have, _ut("employers"),
                                  need, _ut("requierd"))
}


game.ui.infobox.aboutConstruction = function(rx,ry,rw,rh) {
  var ibox = game.ui.infobox.simple(rx,ry,rw,rh)

  ibox.btnNext = ibox.addButton(ibox.w-36,12,24,24)
  ibox.btnNext.text = ">"
  ibox.btnNext.style = "whiteBorderUp"
  ibox.tooltip = "##infobox_construction_comma_tip##"

  ibox.btnPrev = ibox.addButton(ibox.w-60,12,24,24)
  ibox.btnPrev.text = "<"
  ibox.btnPrev.style = "whiteBorderUp"
  ibox.btnPrev.tooltip = _u("infobox_construction_comma_tip")

  ibox.setWorkingStatus = function(active) { game.ui.infobox.updateWorkingLabel(ibox,active) }
  ibox.setWorkersStatus = function(x,y,pic,need,have) { game.ui.infobox.updateWorkersLabel(ibox,x,y,pic,need,have)}
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

  ibox.setWorkersStatus(32, 56+12, 542, temple.maximumWorkers(), temple.numberWorkers())
  ibox.setWorkingStatus(temple.active)
  ibox.setAutoPosition()

  ibox.btnToggleWorks.callback = function() {
    temple.active = !temple.active
    ibox.setWorkingStatus(temple.active)
  }

  ibox.setModal()
  ibox.setFocus()
}
