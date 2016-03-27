function OnShowTempleInfobox(location) {
  game.ui.infobox.aboutTemple(location)
}

function InfoboxWindow (rx,ry,rw,rh) {
  return g_ui.addSimpleWindow(rx,ry,rw,rh)
}

game.ui.infobox.simple = function(rx,ry,rw,rh) {
  var ibox = new InfoboxWindow(rx,ry,rw,rh)
  ibox.title = "FONT_5"

  ibox.autoPosition = true
  ibox.blackFrame = ibox.addLabel(0, 0, 0, 0)
  ibox.blackFrame.style = "blackFrame"
  ibox.blackFrame.textOffset = {x:50, y:15}


  ibox.btnHelp = ibox.addHelpButton(16, ibox.h-40);
  ibox.btnHelp.text = "##infobox_tooltip_help##";
  ibox.help_uri = "unknown";
  ibox.btnHelp.callback = function() {
    engine.log("Show caesapedia for " + ibox.help_uri);
    game.ui.caesopedia.show(ibox.help_uri);
  }

  ibox.setInfoText = function(text) {
    if (!ibox.lbText)
    {
      ibox.lbText = ibox.addLabel(32, 64, ibox.w-54, ibox.h-128)
      ibox.lbText.multiline = true
      ibox.lbText.textAlign = {h:"center",v:"center"}
    }

    ibox.lbText.text = text
  }

  ibox.setAutoPosition = function() {
      var resolution = g_session.resolution
      var ry = ( g_ui.cursor.y < resolution.h / 2 )
                  ? resolution.h - ibox.h - 5
                  : 30;

      ibox.position = {x:(resolution.w-ibox.w)/2, y:ry}
      ibox.mayMove = g_session.getAdvflag("lockwindow")
  }

  return ibox;
}

game.ui.infobox.aboutConstruction = function(rx,ry,rw,rh) {
  var ibox = this.simple(rx,ry,rw,rh)

  ibox.btnNext = ibox.addButton(ibox.w-36,12,24,24)
  ibox.btnNext.text = ">"
  ibox.btnNext.style = "whiteBorderUp"
  ibox.tooltip = "##infobox_construction_comma_tip##"

  ibox.btnPrev = ibox.addButton(ibox.w-60,12,24,24)
  ibox.btnPrev.text = "<"
  ibox.btnPrev.style = "whiteBorderUp"
  ibox.btnPrev.tooltip = "##infobox_construction_comma_tip##"

  ibox.setWorkingStatus = function(active) {
    if (!ibox.btnToggleWorks)
    {
      ibox.btnToggleWorks = new Button(ibox.blackFrame);
      ibox.btnToggleWorks.geometry = { x:ibox.blackFrame.w-110, y:(ibox.blackFrame.h-25)/2, w:100, h:25 }
      ibox.btnToggleWorks.style = "blackBorderUp"
      ibox.btnToggleWorks.font = "FONT_1"
    }

    ibox.btnToggleWorks.text = active ? _u("abwrk_working") : _u("abwrk_not_working")
  }

  ibox.setWorkersStatus = function(x, y, picId, need, have) {
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


  return ibox
}

game.ui.infobox.aboutReservoir = function(location) {
  var ibox = this.aboutConstruction(0, 0, 480, 320);
  ibox.title = _u("reservoir");

  var reservoir = g_session.city.getOverlay(location).as(Reservoir);
  var text = reservoir.haveWater()
                      ? "reservoir_info"
                      : "reservoir_no_water";
  ibox.help_uri = reservoir.typename;
  ibox.setInfoText( _u(text) );
}

game.ui.infobox.aboutTemple = function(location) {
  var ibox = this.aboutConstruction(0,0,510,256)
  ibox.blackFrame.geometry = {x:16, y:56, w:ibox.w-32, h:56}

  var temple = g_session.city.getOverlay(location).as(Temple)
  if (temple.typename == "oracle") {
    ibox.title = _u("oracle")
    ibox.setInfoText( _u("oracle_info") )
  } else {
    var divn = temple.divinity()
    var shortDesc = divn.internalName()+"_desc"
    var text = _format( "{0}_{1}_temple",
                        temple.big ? "big" : "small",
                        divn.internalName())
    ibox.title = _ut(text) + " ( " + _ut(shortDesc) + " ) "

    var goodRelation = divn.relation() >= 50;

    var longDescr = _format( "{}_{}_info",
                             divn.internalName(),
                            goodRelation ? "goodmood" : "badmood" );

    var img = ibox.addImage(192, 140, divn.picture() )
    img.tooltip = _u(longDescr);
  }

  ibox.setWorkersStatus(32, 56+12, 542, temple.maximumWorkers(), temple.numberWorkers())
  ibox.setWorkingStatus(temple.active)
  ibox.setAutoPosition()

  ibox.btnToggleWorks.callback = function() {
    temple.active = !temple.active
    ibox.setWorkingStatus(temple.active)
  }

  ibox.setFocus()
  ibox.setModal()
}
