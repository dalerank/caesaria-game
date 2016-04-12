function OnShowTempleInfobox(location) {
    game.ui.infobox.aboutTemple(location)
}

function InfoboxWindow(rx, ry, rw, rh) {
    return g_ui.addSimpleWindow(rx, ry, rw, rh)
}

game.ui.infobox.simple = function (rx, ry, rw, rh) {
    var ibox = new InfoboxWindow(rx, ry, rw, rh)
    ibox.title = _u("unknown");
    ibox.pauseGame = true;

    ibox.autoPosition = true
    ibox.blackFrame = ibox.addLabel(0, 0, 0, 0)
    ibox.blackFrame.style = "blackFrame"
    ibox.blackFrame.textOffset = {
        x: 50,
        y: 15
    }

    ibox.btnHelp = ibox.addHelpButton(12, ibox.h - 36);
    ibox.btnHelp.tooltip = _u("infobox_tooltip_help");

    ibox.initInfoLabel = function (rx, ry, rw, rh) {
        if (ibox.lbText == undefined) {
            ibox.lbText = ibox.addLabel(rx, ry, rw, rh);
            ibox.lbText.multiline = true;
            ibox.lbText.textAlign = {
                h: "center",
                v: "center"
            };
        } else {
            ibox.lbText.geometry = {
                x: rx,
                y: ry,
                w: rw,
                h: rh
            };
        }
    }

    ibox.initBlackframe = function (rx, ry, rw, rh) {
        ibox.blackFrame.geometry = {
            x: rx,
            y: ry,
            w: rw,
            h: rh
        };
        ibox.initInfoLabel(32, 45, ibox.w - 54, ibox.blackFrame.top() - 50);
        ibox.lbText.textAlign = {
            h: "upperLeft",
            v: "upperLeft"
        };
    }

    ibox.setInfoText = function (text) {
        ibox.lbText.text = text;
    }

    ibox.show = function () {
        if (ibox.overlay)
            ibox.btnHelp.uri = ibox.overlay.typename;

        ibox.setAutoPosition();
        ibox.setFocus();
        ibox.setModal();
    }

    ibox.setAutoPosition = function () {
        var resolution = g_session.resolution
        var ry = (g_ui.cursor.y < resolution.h / 2) ? resolution.h - ibox.h - 5 : 30;

        ibox.position = {
            x: (resolution.w - ibox.w) / 2,
            y: ry
        }
        ibox.mayMove = !engine.getOption("lockInfobox")
    }

    ibox.setWorkersStatus = function (x, y, picId, need, have) {
        ibox.blackFrame.setVisible(need > 0)
        if (0 == need)
            return;

        // number of workers
        ibox.blackFrame.icon = {
            rc: "paneling",
            index: picId
        }
        ibox.blackFrame.iconOffset = {
            x: 20,
            y: 10
        };

        ibox.blackFrame.text = _format("{0} {1} ({2} {3})",
            have, _ut("employers"),
            need, _ut("requierd"))
    }

    return ibox;
}

game.ui.infobox.aboutConstruction = function (rx, ry, rw, rh) {
    var ibox = this.simple(rx, ry, rw, rh)

    ibox.btnNext = ibox.addButton(ibox.w - 36, 12, 24, 24)
    ibox.btnNext.text = ">"
    ibox.btnNext.style = "whiteBorderUp"
    ibox.tooltip = _u("infobox_construction_comma_tip")

    ibox.btnPrev = ibox.addButton(ibox.w - 60, 12, 24, 24)
    ibox.btnPrev.text = "<"
    ibox.btnPrev.style = "whiteBorderUp"
    ibox.btnPrev.tooltip = _u("infobox_construction_comma_tip");

    ibox.btnInfo = ibox.addButton(38, ibox.h - 36, 24, 24);
    ibox.btnInfo.text = "i";
    ibox.btnInfo.style = "whiteBorderUp";
    ibox.btnInfo.callback = function () {
        ibox.showAdvInfo();
    }

    ibox.showAdvInfo = function () {
        var state = _format("Damage={0}\nFire={1}\n",
            ibox.overlay.state(g_config.overlay.params.damage),
            ibox.overlay.state(g_config.overlay.params.fire));

        g_ui.addInformationDialog(_u("overlay_status"), state);
    }

    ibox.changeOverlayActive = function () {
        ibox.overlay.active = !ibox.overlay.active
        ibox.setWorkingStatus(ibox.overlay.active)
    }

    ibox.setWorkingStatus = function (active) {
        if (!ibox.btnToggleWorks) {
            ibox.btnToggleWorks = new Button(ibox.blackFrame);
            ibox.btnToggleWorks.geometry = {
                x: ibox.blackFrame.w - 110,
                y: (ibox.blackFrame.h - 25) / 2,
                w: 100,
                h: 25
            }
            ibox.btnToggleWorks.style = "blackBorderUp"
            ibox.btnToggleWorks.font = "FONT_1"

            ibox.btnToggleWorks.callback = function () {
                ibox.changeOverlayActive()
            }
        }

        ibox.btnToggleWorks.text = active ? _u("abwrk_working") : _u("abwrk_not_working")
    }

    return ibox
}

game.ui.infobox.aboutReservoir = function (location) {
    var ibox = this.aboutConstruction(0, 0, 480, 320);
    ibox.initInfoLabel(20, 20, ibox.w - 40, ibox.h - 60);
    ibox.title = _u("reservoir");

    var reservoir = g_session.city.getOverlay(location).as(Reservoir);
    var text = reservoir.haveWater() ? "reservoir_info" : "reservoir_no_water";
    ibox.overlay = reservoir;
    ibox.setInfoText(_u(text));
    ibox.show();
}

game.ui.infobox.aboutTemple = function (location) {
    var ibox = this.aboutConstruction(0, 0, 510, 256)
    ibox.initBlackframe(16, 56, ibox.w - 32, 56);

    var temple = g_session.city.getOverlay(location).as(Temple);
    ibox.overlay = temple;
    if (temple.typename == "oracle") {
        ibox.title = _u("oracle")
        ibox.setInfoText(_u("oracle_info"))
    } else {
        var divn = temple.divinity()
        var shortDesc = divn.internalName() + "_desc"
        var text = _format("{0}_{1}_temple",
            temple.big ? "big" : "small",
            divn.internalName())
        ibox.title = _ut(text) + " ( " + _ut(shortDesc) + " ) "

        var goodRelation = divn.relation() >= 50;

        var longDescr = _format("{}_{}_info",
            divn.internalName(),
            goodRelation ? "goodmood" : "badmood");

        var img = ibox.addImage(192, 140, divn.picture())
        img.tooltip = _u(longDescr);
    }

    ibox.setWorkersStatus(32, 56 + 12, 542, temple.maximumWorkers(), temple.numberWorkers());
    ibox.setWorkingStatus(temple.active);

    ibox.btnToggleWorks.callback = function () {
        temple.active = !temple.active;
        ibox.setWorkingStatus(temple.active);
    }

    ibox.show();
}