game.ui.infobox.aboutObject = function(typename,info) {
  var ibox = game.ui.infobox.simple(0,0,510,300)

  ibox.title = _u(typename)

  if (!info)
    info = _u(typename + "_info")

  ibox.setInfoText(info)
  ibox.setAutoPosition()
  ibox.setModal()
  ibox.setFocus()

  return ibox
}

game.ui.infobox.aboutRuins = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)

  var ruins = g_session.city.getOverlay(location).as(Ruins)
  engine.log(ruins.typename)

  ibox.title = _u(ruins.typename)
  ibox.setInfoText(ruins.getProperty("pinfo"))
}

game.ui.infobox.aboutBarracks = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)
  ibox.blackFrame.geometry = {x:16, y:80, w:ibox.w-32, h:56}

  var barracks = g_session.city.getOverlay(location).as(Barracks)
  engine.log(barracks.typename)

  ibox.title = _u(barracks.typename)
  ibox.setInfoText(_u("barracks_info"))

  var lbWeaponQty = ibox.addLabel(20, ibox.lbText.bottom(), ibox.w-32, 24)
  lbWeaponQty.font = "FONT_3"
  lbWeaponQty.text = _format( "{0} {1}", _u("weapon_store_of"), barracks.goodQty("weapon") )

  ibox.setWorkersStatus(32, 56+12, 542, barracks.maximumWorkers(), barracks.numberWorkers())
  ibox.setWorkingStatus(barracks.active)
  ibox.setAutoPosition()

  ibox.btnToggleWorks.callback = function() {
    barracks.active = !barracks.active
    ibox.setWorkingStatus(barracks.active)
  }

  ibox.help_uri = barracks.typename;
  ibox.setModal()
  ibox.setFocus()
}

game.ui.infobox.aboutRaw = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)
  ibox.blackFrame.geometry = {x:16, y:170, w:ibox.w-32, h:64}

  var lbProgress = ibox.addLabel(16, 40, ibox.w-32,24)

  var lbAbout = ibox.addLabel(16, 70, ibox.w-32, 24)
  lbAbout.multiline = true

  var lbProductivity = ibox.addLabel( 16, 70, ibox.w-32, 24)
  lbProductivity.multiline = true
  lbProductivity.textAlign = { v:"upperLeft", h:"upperLeft" }
}
