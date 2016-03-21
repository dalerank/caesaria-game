function OnShowTempleInfobox(location) {

}

function InfobboxWindow (rx,ry,rw,rh) {
  this.window = g_ui.addSimpleWindow(rx,ry,rw,rh)
}

InfobboxWindow.prototype.updateWorkersLabel = function(x, y, picId, need, have)
{
  this.lbBlackFrame.setVisible(need > 0)
  if (0 == need)
    return;

  // number of workers
  this.lbBlackFrame.icon = {rc:g_config.rc.panel, index:picId}
  this.lbBlackFrame.setIconOffset(20, 10);

  this.lbBlackFrame.text = _format( "{0} {1} ({2} {3})",
                                  have, _t("##employers##"),
                                  need, _t("##requierd##"))
}

game.ui.infobox.simple = function(rx,ry,rw,rh) {
  var ibox = new InfobboxWindow(rx,ry.rw,rh)
  ibox.title = "FONT_5"

  ibox.autoPosition = true
  ibox.blackFrame = ibox.addLabel(0, 0, 0, 0)
  ibox.style = "blackFrame"
  /*{
    bgtype :
    font : "FONT_2"
    textAlign : [ "upperLeft", "upperLeft" ]
    text.offset : [ 50, 15 ]
  }*/

  ibox.btnHelp = ibox.addHelpButton(16, ibox.h-40)
  ibox.btnHelp.text = "##infobox_tooltip_help##"

  ibox.btnExit = ibox.addExitButotn(ibox.w-40, ibox.h-40)
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
  ibox.blackFrame.geometry = {x:16, y:56, w:510, h:62}

  var overlay = g_session.city.getOverlay(location)
  if (overlay.type == "oracle") {
    ibox.title = "##oracle##"
    ibox.lbText.text = "##oracle_info##"
  } else {
    var divn = temple.divinity()
    var bigTemple = temple.size().w > 2;
    var shortDesc = divn.shortDescription()
    var text = _format( "##{}_{}_temple##",
                        bigTemple ? "big" : "small",
                        divn.internalName())
    ibox.title = text + " ( " + shortDesc + " ) "

    var goodRelation = divn.relation() >= 50;

    var longDescr = _format( "##{}_{}_info##",
                             divn.internalName(),
                            goodRelation ? "goodmood" : "badmood" );

    var img = ibox.addImage(192, 140, divn.picture() )
    img.setTooltipText( _(longDescr) );
  }

  ibox.setModal()
}
