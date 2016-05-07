game.ui.infobox.aboutMarket = function(location) {
  var market = g_session.city.getOverlay(location).as(Market);

  var ibox = this.aboutConstruction(0, 0, 510, 256);
  ibox.initBlackframe(16, 155, ibox.w-32, 52);
  ibox.overlay = market;
  ibox.title = _u(market.typename);

  var lbAbout = ibox.addLabel(20, 40, ibox.w-30, 50);
  lbAbout.multiline = true;
  lbAbout.font = "FONT_1";
  lbAbout.textAlign = { v:"upperLeft", h:"upperLeft" };

  ibox.drawGood = function(store, goodType, index, paintY) {
    var startOffset = 25;

    var offset = ( ibox.w - startOffset * 2 ) / 5;
    var order = store.getOrder(goodType);

    // pictures of goods
    var pos = {x:index * offset + startOffset, y:paintY};

    var btn = ibox.addButton(pos.x, pos.y, 100, 24);
    btn.setProperty("gtype", goodType)
    btn.text = store.qty(goodType);
    btn.style = "noBackground";
    btn.icon = g_config.good.getInfo(goodType).picture.local;
    if (order == g_config.order.reject)
      btn.iconMask = 0xff606060;

    btn.callback = function() {
      var order = store.getOrder(goodType);
      order = (order == g_config.order.reject)
                ? g_config.order.accept
                : g_config.order.reject;

      store.setOrder(goodType, order);
      btn.iconMask = (order == g_config.order.reject)
                        ? 0xff606060
                        : 0x00000000;
    }
  }

  var goods = market.goodStore();
  if (market.numberWorkers > 0) {
    var furageSum = 0;
    // for all furage types of good
    for( var gtype in g_config.good.basicfood) {
      furageSum += goods.qty(gtype);
    }

    var paintY = 100;
    if( 0 > furageSum )
    {
      ibox.drawGood( goods, g_config.good.wheat, 0, paintY );
      ibox.drawGood( goods, g_config.good.fish, 1, paintY);
      ibox.drawGood( goods, g_config.good.meat, 2, paintY);
      ibox.drawGood( goods, g_config.good.fruit, 3, paintY);
      ibox.drawGood( goods, g_config.good.vegetable, 4, paintY);
      lbAbout = 60;
    }
    else
    {
      lbAbout.h = 90;
      lbAbout.textAlign = { h:"upperLeft", v:"center" };
    }

    paintY += 24;
    ibox.drawGood( goods, g_config.good.pottery, 0, paintY);
    ibox.drawGood( goods, g_config.good.furniture, 1, paintY);
    ibox.drawGood( goods, g_config.good.oil, 2, paintY);
    ibox.drawGood( goods, g_config.good.wine, 3, paintY);

    lbAbout.text = (0 == furageSum ? _u("market_search_food_source") : _u("market_about"));
  }
  else
  {
    lbAbout.h = 50;
    lbAbout.text = _u("market_no_workers");
  }

  ibox.setWorkersStatus(32, 8, 542, market.maximumWorkers, market.numberWorkers;
  ibox.setWorkingStatus(market.active);

  ibox.show();
}
