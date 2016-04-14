game.ui.dialogs.playerSalarySettings = {}

function ShowPlayerSalarySettings() {
    game.ui.dialogs.playerSalarySettings.show()
}

game.ui.dialogs.playerSalarySettings.show = function () {
    var wint = g_session.city.victoryConditions();
    if (wint.reignYears > 0) {
        g_ui.addInformationDialog("", _u("disabled_draw_salary_for_free_reign"))
        return;
    }

    var w = g_ui.addWindow(0, 0, 510, 350)
    w.title = _u("governor_salary_title")
    w.font = "FONT_5"
    w.closeAfterKey({ escape: true, rmb: true });

    var playerSalary = g_session.player.salary();
    var wantSalary = playerSalary;

    var lbxTitles = w.addListbox(16, 50, w.w - 32, w.h - 100)
    lbxTitles.setTextAlignment("center", "center")
    lbxTitles.itemsFont = "FONT_2_WHITE"
    lbxTitles.background = true;
    lbxTitles.itemHeight = 22

    for (var i in g_config.ranks) {
        var rank = g_config.ranks[i];
        var str = _ut(rank.name + "_salary");
        var index = lbxTitles.addLine(str + "   " + rank.salary)
        if (rank.salary === playerSalary)
            lbxTitles.selectedIndex = i;
    }

    lbxTitles.onSelectedCallback = function (index) {
        wantSalary = g_config.ranks[index].salary
    }

    var btnOk = w.addButton(w.w * 0.25, w.h - 45, w.w * 0.25, 24);
    btnOk.text = _u("ok");
    btnOk.style = "whiteBorderUp";
    btnOk.callback = function () {
        g_session.player.setSalary(wantSalary);
        var current = g_session.player.rank();
        var availableSalary = g_config.ranks[current].salary;

        if (wantSalary > availableSalary) {
            g_ui.addInformationDialog(_u("changesalary_warning"), _u("changesalary_greater_salary"));
        }
    }

    var btnCancel = w.addButton(w.w * 0.5, w.h - 45, w.w * 0.25, 24)
    btnCancel.text = _u("cancel");
    btnCancel.style = "whiteBorderUp";
    btnCancel.tooltip = _u("exit_salary_window");
    btnCancel.callback = function () {
        w.deleteLater()
    }

    w.moveToCenter();
    w.setModal();
}
