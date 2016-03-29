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

game.ui.infobox.aboutRuins = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)

  var ruins = g_session.city.getOverlay(location).as(Ruins)
  engine.log(ruins.typename)

  ibox.title = _u(ruins.typename)
  ibox.setInfoText(ruins.getProperty("pinfo"))
}

game.ui.infobox.aboutBarracks = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350)
  ibox.blackFrame.geometry = {x:16, y:80, w:ibox.w-32, h:56}

  var barracks = g_session.city.getOverlay(location).as(Barracks)
  engine.log(barracks.typename)

  ibox.title = _u(barracks.typename)
  ibox.setInfoText(_u("barracks_info"))

  var lbWeaponQty = ibox.addLabel(20, ibox.lbText.bottom(), ibox.w-32, 24)
  lbWeaponQty.font = "FONT_3"
  lbWeaponQty.text = _format( "{0} {1}", _u("weapon_store_of"), barracks.goodQty("weapon") )

  ibox.setWorkersStatus(32, 56+12, 542, barracks.maximumWorkers(), barracks.numberWorkers())
  ibox.setWorkingStatus(barracks.active)
  ibox.setAutoPosition()

  ibox.btnToggleWorks.callback = function() {
    barracks.active = !barracks.active
    ibox.setWorkingStatus(barracks.active)
  }

  ibox.btnHelp.uri = barracks.typename;
  ibox.setAutoPosition()
  ibox.setModal()
  ibox.setFocus()
}

game.ui.infobox.aboutFountain = function(location) {
  var ibox = this.aboutConstruction(0,0,480,320);

  var fountain = g_session.city.getOverlay(location);
  ibox.title = _u(fountain.typename);
  ibox.overlay = fountain;

  var reservoirAccess = fountain.getProperty("reservoirAccess");
  var reservoirWater = fountain.getProperty("reservoirWater");
  var text = "fountain_unknown_status";

  if (reservoirAccess && reservoirWater<=0) {
    text = "fountain_will_soon_be_hooked";
  } if (fountain.active) {
    text = fountain.mayWork() ? "fountain_info" : "fountain_not_work";
  } else {
    text = reservoirAccess ? "need_full_reservoir_for_work" : "need_reservoir_for_work";
  }

  ibox.setInfoText(_u(text));
}

game.ui.infobox.aboutFactory = function(location) {
  var ibox = this.aboutConstruction(0,0,510,256);
  ibox.blackFrame.geometry = {x:16, y:160, w:ibox.w-32, h:52}
  var factory = g_session.getOverlay(location).as(Factory);
  ibox.overlay = factory;
  ibox.title = _u(factory.typename);

  // paint progress
  var text = _format( "{0} {1}%", _ut("rawm_production_complete_m"), factory.progress() );

  Size lbSize( (width() - 20) / 2, 25 );
  ibox.lbProduction = ibox.addLabel(20, 40, ibox.w-20, 25);
  ibox.lbProduction.text = text

  var effciencyText = _format("{0} {1}%", _ut("effciency"), factory.effciency);
  var lbEffciency = ibox.addLabel( 20, ibox.lbProductivity.bottom(), ibox.w-20, 25), effciencyText );

if( factory->produce().type() != good::none )
{
  add<Image>( Point( 10, 10), factory->produce().picture() );
}

// paint picture of in good
if( factory->inStock().type() != good::none )
{
  Label& lbStockInfo = add<Label>( Rect( _lbTitle()->leftbottom() + Point( 0, 25 ), Size( width() - 32, 25 ) ) );
  lbStockInfo.setIcon( good::Info( factory->inStock().type() ).picture() );

  std::string whatStock = fmt::format( "##{0}_factory_stock##", factory->consume().name() );
  std::string typeOut = fmt::format( "##{0}_factory_stock##", factory->produce().name() );
  std::string text = utils::format( 0xff, "%d %s %d %s",
                                    factory->inStock().qty() / 100,
                                    _(whatStock),
                                    factory->outStock().qty() / 100,
                                    _(typeOut) );

  lbStockInfo.setText( text );
  lbStockInfo.setTextOffset( Point( 30, 0 ) );

  _lbText()->setGeometry( Rect( lbStockInfo.leftbottom() + Point( 0, 5 ),
                                _lbBlackFrame()->righttop() - Point( 0, 5 ) ) );
  _lbText()->setFont( "FONT_1" );
}

std::string workInfo = factory->workersProblemDesc();
std::string cartInfo = factory->cartStateDesc();
setText( utils::format( 0xff, "%s %s", _(workInfo), _( cartInfo ) ) );

_updateWorkersLabel( Point( 32, 157 ), 542, factory->maximumWorkers(), factory->numberWorkers() );
}

game.ui.infobox.aboutRaw = function(location) {
  var ibox = this.aboutConstruction(0,0,510,350);
  ibox.blackFrame.geometry = {x:16, y:170, w:ibox.w-32, h:64};

  var factory = g_session.city.getOverlay(location).as(Factory);
  ibox.title = _u(factory.typename);
  ibox.btnHelp.uri = factory.typename;

  var lbProgress = ibox.addLabel(20, 45, ibox.w-32,24);
  lbProgress.text = _format("{0} {1}%", _ut("rawm_production_complete_m"), factory.progress());

  var lbAbout = ibox.addLabel(16, 70, ibox.w-32, 24);
  lbAbout.multiline = true;

  var lbProductivity = ibox.addLabel(20, 75, ibox.w-32, 48);
  lbProductivity.multiline = true;
  lbProductivity.textAlign = { v:"upperLeft", h:"upperLeft" };

  var problemText = factory.workersProblemDesc();
  var cartInfo = factory.cartStateDesc();

  lbProductivity.text = _t(problemText) + _t(cartInfo);

  var ginfo = factory.getProperty("produce");
  ginfo = g_config.good.getInfo(ginfo);
  engine.log(ginfo.name)
  ibox.addImage(10, 10, factory.produce.picture.local);

  ibox.setWorkersStatus(32, 160, 542, factory.maximumWorkers(), factory.numberWorkers())
  ibox.setWorkingStatus(factory.active)

  ibox.btnToggleWorks.callback = function() {
    factory.active = !factory.active
    ibox.setWorkingStatus(factory.active)
  }

  ibox.setAutoPosition()
  ibox.setModal()
  ibox.setFocus()
}
