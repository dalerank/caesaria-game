game.ui.dialogs.showMissionTargets = function()
{
    var w = g_ui.addWindow(0, 0, 610, 430);
    w.title = "##player_name##";

    var lbToCity = w.addLabel(w.w-300, w.h-40, 290, 30)
    lbToCity.textAlign = { v:"center", h:"center" }
    lbToCity.font = "FONT_2"
    lbToCity.text = "##mission_wnd_tocity##"

    var btnExit = w.addTExturedButton(w.w-110, w.h-40, 27, 27)
    btnExit.states = { rc:"paneling", normal:179, hover:180, pressed:181, disabled:179 };

    var gbox = w.addGroupbox(16, 64, w.w-80, 80)
    var lbTargets = gbox.addLabel(15, 0, w.w-30, 28)
    lbTargets.text = "##mission_wnd_targets_title##"
    lbTargets.font = "FONT_1_WHITE"
    lbTargets.textAlign = { h:"upperLeft", v:"center" }


    var lbPopulation = gbox.addLabel(16, 32, 240, 26)
    lbPopulation.text = "##mission_wnd_population##"
    lbPopulation.style = "smallBrown"
    lbPopulation.textOffset = {x:10, y:0}
    lbPopulation.font = "FONT_1_RED"

    var lbProsperity = gbox.addLabel(16, 54, 240, 20)
    lbProsperity.text = "##mission_wnd_prosperity##"
    lbProsperity.style = "smallBrown"
    lbProsperity.textOffset = {x:10, y:0}
    lbProsperity.font = "FONT_1_RED"

    var lbFavour = gbox.addLabel(270, 10, 240, 20)
    lbFavour.text =  "##mission_wnd_favour##"
    lbFavour.style = "smallBrown"
    lbFavour.textOffset = {x:10, y:0 }
    lbFavour.font = "FONT_1_RED"

    var lbCulture = gbox.addLabel(270, 32, 240, 20)
    lbCulture.text = "##mission_wnd_culture##"
    lbCulture.style = "smallBrown"
    lbCulture.textOffset = { x:10, y:0 }
    lbCulture.font = "FONT_1_RED"

    var lbPeace = gbox.addLabel(270, 54, 240, 20)
    lbPeace.text = "##mission_wnd_peace##"
    lbPeace.style = "smallBrown"
    lbPeace.textOffset = { x:10, y:0 }
    lbPeace.font = "FONT_1_RED"

    var lbShortDesc = gbox.addLabel(16, 54, w.w-32, 20)
    lbShortDesc.text = "##mission_wnd_short##"
    lbShortDesc.style = "smallBrown"
    lbShortDesc.textOffset = { x:10, y:0 }
    lbShortDesc.font = "FONT_1_RED"

    var lbxHelp = w.addListbox(16, 152, w.w-32, w.h-190)
    lbShortDesc.itemsFont = "FONT_2_WHITE"
    lbShortDesc.itemsHeight = 16
    lbShortDesc.marginLeft = 10
    lbShortDesc.marginTop = 10
}
