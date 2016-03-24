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
