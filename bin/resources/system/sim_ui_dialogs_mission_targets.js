function OnShowMissionTargetsWindow()
{
    sim.ui.dialogs.showMissionTargets()
}

sim.ui.dialogs.showMissionTargets = function()
{
    var wint = g_session.winConditions();
    for (var i in wint)
      engine.log(i + "  :  " + wint[i]);

    var missionTitle = wint.caption;
    if(missionTitle.lenght===0) missionTitle = "##build_your_rome##";

    var w = g_ui.addWindow(0, 0, 610, 430);
    w.internalName = "MissionTargetsWindow";
    w.pauseGame = true
    w.title = missionTitle
    w.titleFont = "FONT_5"
    w.setTextAlignment("upperLeft", "center")
    w.closeAfterKey( {escape:true, rmb:true} )
    w.moveToCenter()
    w.mayMove = false
    w.setModal()

    var lbToCity = w.addLabel(w.w-300, w.h-40, 290, 30)
    lbToCity.textAlign = { v:"center", h:"center" }
    lbToCity.font = "FONT_2"
    lbToCity.text = "##mission_wnd_tocity##"

    var btnExit = w.addTexturedButton(w.w-110, w.h-40, 27, 27)
    btnExit.states = { rc:"paneling", normal:179, hover:180, pressed:181, disabled:179 };
    btnExit.callback = function() { w.deleteLater() }

    var gbox = w.addGroupbox(16, 64, w.w-80, 80)
    var lbTargets = gbox.addLabel(15, 0, w.w-30, 28)
    lbTargets.text = "##mission_wnd_targets_title##"
    lbTargets.font = "FONT_1_WHITE"
    lbTargets.textAlign = { h:"upperLeft", v:"center" }

    var lbPopulation = gbox.addLabel(16, 32, 240, 26)
    lbPopulation.text = _format( "{0}:{1}", _t("##mission_wnd_population##"), wint.population )
    lbPopulation.style = "smallBrown"
    lbPopulation.textOffset = {x:10, y:0}
    lbPopulation.font = "FONT_1_RED"

    if (wint.prosperity>0)
    {
        var lbProsperity = gbox.addLabel(16, 54, 240, 20)
        lbProsperity.text = _format( "{0}:{1}", _t("##senatepp_prsp_rating##"), wint.prosperity )
        lbProsperity.style = "smallBrown"
        lbProsperity.textOffset = {x:10, y:0}
        lbProsperity.font = "FONT_1_RED"
    }

    if (wint.favour>0)
    {
        var lbFavour = gbox.addLabel(270, 10, 240, 20)
        lbFavour.text =  _format( "{0}:{1}", _t("##senatepp_favour_rating##"), wint.favour )
        lbFavour.style = "smallBrown"
        lbFavour.textOffset = {x:10, y:0 }
        lbFavour.font = "FONT_1_RED"
    }

    if (wint.culture>0)
    {
        var lbCulture = gbox.addLabel(270, 32, 240, 20)
        lbCulture.text = _format( "{0}:{1}", _t("##senatepp_clt_rating##"), wint.culture )
        lbCulture.style = "smallBrown"
        lbCulture.textOffset = { x:10, y:0 }
        lbCulture.font = "FONT_1_RED"
    }

    if (wint.peace>0)
    {
        var lbPeace = gbox.addLabel(270, 54, 240, 20)
        lbPeace.text = _format( "{}:{}", _t("##senatepp_peace_rating##"), wint.peace );
        lbPeace.style = "smallBrown"
        lbPeace.textOffset = { x:10, y:0 }
        lbPeace.font = "FONT_1_RED"
    }

    if (wint.shortDesc.length>0)
    {
        var lbShortDesc = gbox.addLabel(16, 54, 480, 20)
        lbShortDesc.text = _t(wint.shortDesc)
        lbShortDesc.style = "smallBrown"
        lbShortDesc.textOffset = { x:10, y:0 }
        lbShortDesc.font = "FONT_1_RED"
    }

    var lbxHelp = w.addListbox(16, 152, w.w-32, w.h-195)
    var items = wint.overview;
    lbxHelp.itemHeight = 16
    lbxHelp.background = true
    lbxHelp.margin = { left:10, top:10 }
    lbxHelp.itemColor = { simple:"0xffe0e0e0", hovered:"0xff000000" }

    for (var i in items)
      lbxHelp.fitText( _t(items[i]) )

    var startSound = wint.startSound;
    if (startSound.lenght>0)
    {
      w.addSoundMuter(5)
      w.addSoundEmitter(startSound, 100, g_config.audio.speech)
    }
}
