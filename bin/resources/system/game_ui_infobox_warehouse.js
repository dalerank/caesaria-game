game.ui.infobox.ruleNames = ["accept", "reject", "deliver", "none"];

game.ui.infobox.baseSpecialOrdersWindow = function (top, left, height) {
    var ibox = this.simple(0, 0, 510, height);

    ibox.position = {
        x: top,
        y: left
    };
    ibox.gbOrders = ibox.addGroupbox(17, 42, ibox.w - 35, ibox.h - 120);
    ibox.gbOrders.buttons = [];

    ibox.show = function () {
        if (ibox.overlay)
            ibox.btnHelp.uri = ibox.overlay.typename;

        ibox.setFocus();
        ibox.setModal();
    }

    ibox.gbOrders.updateRuleText = function (btn, gtype, storage) {
        var rule = storage.getOrder(gtype);
        btn.font = (rule == g_config.order.reject ? "FONT_1_RED" : "FONT_1_WHITE");
        btn.text = _u(game.ui.infobox.ruleNames[rule]);
    }

    ibox.gbOrders.getStepVolume = function (gtype, storage) {
        var step;
        var prc = storage.capacity(gtype) / storage.capacity();
        if (prc <= 0.25) step = 1;
        else if (prc <= 0.50) step = 2;
        else if (prc <= 0.75) step = 3;
        else step = 0;
        return step % 4;
    }

    ibox.gbOrders.updateCapacityText = function (btn, gtype, storage) {
        var step = ibox.gbOrders.getStepVolume(gtype, storage);
        btn.text = (step == 0 ? "Any" : _format("{0}/4", step));
    }

    ibox.gbOrders.changeCapacity = function (btn, gtype, storage) {
        var capacity = storage.capacity(gtype);
        var step = ibox.gbOrders.getStepVolume(gtype, storage);
        step = (step + 1) % 4;
        storage.setCapacity(gtype, step == 0 ? 9999 : storage.capacity() * step / 4);
        ibox.gbOrders.updateCapacityText(btn, gtype, storage);
    }

    ibox.gbOrders.changeRule = function (btn, gtype, storage) {
        var rule = storage.getOrder(gtype);
        storage.setOrder(gtype, (rule + 1) % g_config.order.none);
        ibox.gbOrders.updateRuleText(btn, gtype, storage);
    }

    ibox.gbOrders.addProduct = function (gtype, storage) {
        engine.log(gtype);
        var ginfo = g_config.good.getInfo(gtype);

        var ry = 5 + 25 * ibox.gbOrders.buttons.length;
        var lb = ibox.gbOrders.addLabel(10, ry, 140, 25);
        lb.text = _u(ginfo.name);
        lb.icon = ginfo.picture.local;
        lb.textOffset = {
            x: 30,
            y: 0
        };
        lb.font = "FONT_1_WHITE";
        lb.textAlign = {
            h: "upperLeft",
            v: "center"
        };

        var btnOrder = ibox.gbOrders.addButton(140, ry, 240, 25);
        btnOrder.style = "blackBorderUp";
        btnOrder.font = "FONT_1_WHITE";
        btnOrder.callback = function () {
            ibox.gbOrders.changeRule(btnOrder, gtype, storage);
        }

        var img = new Image(ibox.gbOrders);
        img.geometry = {
            x: ibox.gbOrders.w - 35,
            y: ry,
            w: 25,
            h: 25
        };
        img.picture = ginfo.picture.local;

        var btnVolume = ibox.gbOrders.addButton(btnOrder.right(), ry, 40, 25);
        btnVolume.font = "FONT_2_WHITE";
        btnVolume.style = "blackBorderUp"
        btnVolume.callback = function () {
            ibox.gbOrders.changeCapacity(btnVolume, gtype, storage);
        }

        ibox.gbOrders.updateRuleText(btnOrder, gtype, storage);
        ibox.gbOrders.updateCapacityText(btnVolume, gtype, storage);

        ibox.gbOrders.buttons.push(btnOrder);
    }

    return ibox;
}

game.ui.infobox.showGrSpecialOrdersWindow = function (parent, grStore) {
    var ibox = game.ui.infobox.baseSpecialOrdersWindow(parent.left(), parent.bottom() - 250, 245);
    ibox.title = _u("granary_orders");

    for (var i in g_config.good.grararable) {
        var gtype = g_config.good.grararable[i];
        var rule = grStore.getOrder(gtype);
        ibox.gbOrders.addProduct(gtype, grStore);
    }

    ibox.update = function () {
        ibox.btnToggleDevastation.text = grStore.isDevastation() ? _u("stop_granary_devastation") : _u("devastate_granary");
    }


    ibox.btnToggleDevastation = ibox.addButton(80, ibox.h - 45, ibox.w - 160, 20);
    ibox.btnToggleDevastation.style = "whiteBorderUp";
    ibox.btnToggleDevastation.font = "FONT_2"
    ibox.btnToggleDevastation.text = _u("unknown_text");
    ibox.btnToggleDevastation.callback = function () {
        grStore.setDevastation(!grStore.isDevastation());
        ibox.update();
    }

    ibox.update();
    ibox.show();
}

game.ui.infobox.showWhSpecialOrdersWindow = function (parent, wh) {
    var ibox = game.ui.infobox.baseSpecialOrdersWindow(parent.left(), parent.bottom() - 560, 560);

    ibox.title = _u("warehouse_orders");

    ibox.update = function () {
        ibox.btnToggleDevastation.text = wh.store().isDevastation() ? _u("stop_warehouse_devastation") : _u("devastate_warehouse");

        ibox.btnTradeCenter.text = wh.isTradeCenter() ? _u("become_warehouse") : _u("become_trade_center");
    }

    ibox.btnToggleDevastation = ibox.addButton(80, ibox.h - 45, ibox.w - 160, 20);
    ibox.btnToggleDevastation.style = "whiteBorderUp";
    ibox.btnToggleDevastation.font = "FONT_2"
    ibox.btnToggleDevastation.text = _u("unknown_text");
    ibox.btnToggleDevastation.callback = function () {
        wh.store().setDevastation(!wh.store().isDevastation());
        ibox.update();
    }

    ibox.btnTradeCenter = ibox.addButton(80, ibox.h - 70, ibox.w - 160, 20);
    ibox.btnTradeCenter.style = "whiteBorderUp";
    ibox.btnTradeCenter.font = "FONT_2";
    ibox.btnTradeCenter.text = _u("unknown_text");
    ibox.btnTradeCenter.callback = function () {
        wh.tradeCenter = !wh.tradeCenter;
        ibox.update();
    }

    var whStore = wh.store();
    for (var i in g_config.good.storable) {
        var gtype = g_config.good.storable[i];
        var rule = whStore.getOrder(gtype);
        ibox.gbOrders.addProduct(gtype, whStore);
    }

    ibox.update();
    ibox.show();
}

game.ui.infobox.aboutStorage = function (x, y, w, h) {
    var ibox = this.aboutConstruction(x, y, w, h);

    ibox.addProductButton = function (goodType, col, paintY, offset, qty) {
        var ginfo = g_config.good.getInfo(goodType);

        var btn = ibox.addButton(col * offset + 15, paintY, 150, 24);
        btn.font = "FONT_2";
        btn.style = "noBackground";
        btn.textAlign = {
            h: "upperLeft",
            v: "center"
        };
        btn.icon = ginfo.picture.local;
        btn.iconOffset = {
            x: 0,
            y: 4
        };
        btn.text = _format("{0} {1} {2}", g_config.metric.convQty(qty / 100), _ut(ginfo.name), g_config.metric.modeShort);
        btn.textOffset = {
            x: 25,
            y: 0
        };

        return btn;
    }

    return ibox;
}

game.ui.infobox.aboutGranary = function (location) {
    var granary = g_session.city.getOverlay(location).as(Granary);

    var ibox = this.aboutStorage(0, 0, 510, 280);
    ibox.initBlackframe(16, 150, ibox.w - 32, 58);

    ibox.title = _u(granary.typename);

    ibox.drawGrGood = function (goodType, col, paintY) {
        var qty = granary.store().qty(goodType);
        var btn = ibox.addProductButton(goodType, col, paintY, 250, qty);
    }

    var lbUnits = ibox.addLabel(16, 45, ibox.w - 32, 25)
    var grStore = granary.store();
        // summary: total stock, free capacity
    var capacity = grStore.qty();
    var freeQty = grStore.free();
    var desc = _format("{0} {1}, {2} {3} ({4})",
        g_config.metric.convQty(capacity),
        _ut("units_in_stock"),
        _ut("freespace_for"),
        g_config.metric.convQty(freeQty),
        g_config.metric.modeShort);
    lbUnits.text = desc;

    ibox.drawGrGood(g_config.good.wheat, 0, lbUnits.bottom());
    ibox.drawGrGood(g_config.good.meat, 0, lbUnits.bottom() + 25);
    ibox.drawGrGood(g_config.good.fruit, 1, lbUnits.bottom());
    ibox.drawGrGood(g_config.good.vegetable, 1, lbUnits.bottom() + 25);

    var btnOrders = ibox.addButton(65, ibox.h - 37, ibox.w - 125, 25)
    btnOrders.text = _u("granary_orders");
    btnOrders.style = "whiteBorderUp";
    btnOrders.callback = function () {
        game.ui.infobox.showGrSpecialOrdersWindow(ibox, grStore);
    }

    ibox.setWorkersStatus(32, 130, 542, granary.maximumWorkers(), granary.numberWorkers());
    ibox.setWorkingStatus(granary.active);
    ibox.show();
}

game.ui.infobox.aboutWarehouse = function (location) {
    var wh = g_session.city.getOverlay(location).as(Warehouse);

    var ibox = this.aboutStorage(0, 0, 510, 360);

    ibox.initBlackframe(16, 225, ibox.w - 32, 58);
    ibox.title = _u(wh.typename);
    ibox.overlay = wh;

    var btnOrders = ibox.addButton(65, ibox.h - 37, ibox.w - 125, 25);
    btnOrders.style = "whiteBorderUp";
    btnOrders.font = "FONT_2";
    btnOrders.text = _u("special_orders");
    btnOrders.callback = function () {
        game.ui.infobox.showWhSpecialOrdersWindow(ibox, wh);
    }

    var isTradeCenter = wh.getProperty("isTradeCenter");
    if (isTradeCenter)
        ibox.title = _u("trade_center");

    ibox.drawWhGood = function (goodType, col, paintY) {
            var qty = wh.store().qty(goodType);
            var btn = ibox.addProductButton(goodType, col, paintY, 150, qty);
        }
        /*StringArray warnings;
        if( _warehouse->onlyDispatchGoods() )  { warnings << "##warehouse_low_personal_warning##";  }
        if( _warehouse->getGoodStore().freeQty() == 0 ) { warnings << "##warehouse_full_warning##";  }

        if( !warnings.empty() )
        {
          Label* lb = new Label( this, Rect( 20, height() - 54, width() - 20, height() - 34 ), _(warnings.rand()) );
          lb->setTextAlignment( alignCenter, alignCenter );
        }*/
        // summary: total stock, free capacity
    var _paintY = 50;

    ibox.drawWhGood(g_config.good.wheat, 0, _paintY + 0);
    ibox.drawWhGood(g_config.good.vegetable, 0, _paintY + 25);
    ibox.drawWhGood(g_config.good.fruit, 0, _paintY + 50);
    ibox.drawWhGood(g_config.good.olive, 0, _paintY + 75);
    ibox.drawWhGood(g_config.good.grape, 0, _paintY + 100);
    ibox.drawWhGood(g_config.good.fish, 0, _paintY + 125);

    ibox.drawWhGood(g_config.good.meat, 1, _paintY + 0);
    ibox.drawWhGood(g_config.good.wine, 1, _paintY + 25);
    ibox.drawWhGood(g_config.good.oil, 1, _paintY + 50);
    ibox.drawWhGood(g_config.good.iron, 1, _paintY + 75);
    ibox.drawWhGood(g_config.good.timber, 1, _paintY + 100);

    ibox.drawWhGood(g_config.good.clay, 2, _paintY + 0);
    ibox.drawWhGood(g_config.good.marble, 2, _paintY + 25);
    ibox.drawWhGood(g_config.good.weapon, 2, _paintY + 50);
    ibox.drawWhGood(g_config.good.furniture, 2, _paintY + 75);
    ibox.drawWhGood(g_config.good.pottery, 2, _paintY + 100);

    ibox.setWorkersStatus(32, 56 + 12, 542, wh.maximumWorkers(), wh.numberWorkers());
    ibox.setWorkingStatus(wh.active);
    ibox.show();
}
