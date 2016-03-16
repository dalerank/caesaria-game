sim.ui.topmenu.options.showSpeedOptions = function()
{
    var w = g_ui.addWindow(0, 0, 350, 225);
    w.title = "##game_speed_options##";

    var wasChanged = false;
    var saveGameSpeed = sim.timescale.value
    var saveScroolSpeed = g_session.getAdvflag("scrollSpeed")
    var saveAutosaveInterval = engine.getOption("autosaveInterval")

    var spnGameSpeed = w.addSpinbox(28, 60, w.w-28*2, 24);
    spnGameSpeed.text = "##gmspdwnd_game_speed##";
    spnGameSpeed.postfix = " %";
    spnGameSpeed.range = { min:10, max:300, step:10 }
    spnGameSpeed.value = saveGameSpeed;
    spnGameSpeed.font = "FONT_2";
    spnGameSpeed.textAlign = { h:"center", v:"center" };
    spnGameSpeed.callback = function(value) {
        wasChanged = true
        sim.timescale.value = value
    }

    var spnScrollSpeed = w.addSpinbox(28, 60, w.w-28*2, 24);
    spnScrollSpeed.text = "##gmspdwnd_scroll_speed##";
    spnScrollSpeed.postfix = " %";
    spnScrollSpeed.value = saveScroolSpeed;
    spnScrollSpeed.range = { min: 10, max: 100, step:10 }
    spnScrollSpeed.font = "FONT_2";
    spnScrollSpeed.textAlign = { h: "center", v: "center" };
    spnScrollSpeed.callback = function (value) {
        wasChanged = true;
        g_session.setAdvflag("scrollSpeed",value)
    }

    var spnAutpsaveInterval = w.addSpinbox(28, 60, w.w-28*2, 24);
    spnAutpsaveInterval.text = "##gmspdwnd_autosave_interval##";
    spnAutpsaveInterval.postfix = " m.";
    spnAutpsaveInterval.range = { min: 0, max: 12, step: 1 }
    spnAutpsaveInterval.value = saveAutosaveInterval;
    spnAutpsaveInterval.font = "FONT_2";
    spnAutpsaveInterval.textAlign = { h: "center", v: "center" };
    spnAutpsaveInterval.callback = function (value) {
        wasChanged = true;
        engine.setOption("autosaveInterval", value)
    }

    var btnOk = wnd.addButton(w.w*0.25, w.h-60, w.w*0.5,  22);
    btnOk.style = "smallGrayBorderLine";
    btnOk.text = "##ok##";
    btnOk.callback = function() { w.deleteLater();	};

    var btnCancel = wnd.addButton(w.w*0.25, w.h-35, w.w*0.5,  22);
    btnCancel.style = "smallGrayBorderLine";
    btnCancel.text = "##cancel##";
    btnCancel.callback = function() {
        if (wasChanged)
        {
            sim.timescale.value = saveGameSpeed;
            g_session.setAdvflag("scrollSpeed",saveScroolSpeed);
            engine.setOption("autosaveInterval",saveAutosaveInterval);
        }
        w.deleteLater();
    };

    w.moveToCenter();
    w.setModal();
}
