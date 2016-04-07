game.ui.infobox.wshow = function(walkers, location) {
  var typename = walkers[0].typename;
  engine.log(typename);
  switch(typename) {
    case "merchant": case "seaMerchant":
      game.ui.infobox.aboutMerchant(walkers);
    break;

    case "patrolPoint":
      game.ui.infobox.aboutLegion(walkers);
    break;

    default:
      game.ui.infobox.aboutWalker(walkers);
    break;
  }
}

game.ui.infobox.wsimple = function(walkers, x,y,w,h) {
  var ibox = game.ui.infobox.simple(x,y,w,h);

  ibox.lbCurrentAction = ibox.addLabel(40, ibox.h-80, ibox.w-60, 20);
  ibox.lbBaseBuilding = ibox.addLabel(40, ibox.h-60, ibox.w-60, 20);

  ibox.walkers = walkers;
  ibox.walker = walkers[0];

  ibox.lbName = ibox.addLabel(90, 108, ibox.w-120, 20);
  ibox.lbType = ibox.addLabel(90, 128, ibox.w-120, 20);
  ibox.lbType.font = "FONT_1";

  ibox.lbThinks = ibox.addLabel(90, 148, ibox.w-120, ibox.h-250);
  ibox.lbThinks.text = _u("citizen_thoughts_will_be_placed_here");
  ibox.lbThinks.multiline = true

  ibox.lbCitizenPic = ibox.addLabel(30, 112, 55, 80);
  ibox.screenshots = [];

  ibox.btnMove2dst = ibox.addButton(16, ibox.h-80, 20, 20);
  ibox.btnMove2dst.style = "whiteBorderUp";
  ibox.btnMove2dst.text = ">";
  ibox.btnMove2dst.callback = function() {
    if (ibox.destinationPos != null) {
      g_session.moveCamerae(ibox.destinationPos);
    }

    if (ibox.walker != null) {
      ibox.walker.setFlag(g_config.walker.showPath,true);
    }
  }

  ibox.btnMove2base = ibox.addButton(16, ibox.h-60, 20, 20);
  ibox.btnMove2base.style = "whiteBorderUp";
  ibox.btnMove2base.text = ">";
  ibox.btnMove2base.callback = function() {
    if (ibox.baseBuildingPos != null) {
       g_session.moveCamera(ibox.baseBuildingPos);
    }
  }

  ibox.updateTitle = function() {
    if (ibox.walker == null)
      return;

    var a = ibox.walker.as(EnemySoldier);
    if (a != null) {
      var nation = a.getProperty("nation")
      ibox.title = _u( nation.substring(0,nation.length-2) + "_soldier##" );
      return;
    }

    a = ibox.walker.getProperty("vividly");
    if (!a) {
      ibox.title = _u(ibox.walker.typename);
      ibox.lbName.visible = false;
      ibox.lbType.visible = false;
      ibox.lbCitizenPic.visible = false;

      ibox.lbThinks.geometry = ibox.blackFrame.geometry;
      ibox.lbThinks.text = _u(ibox.walker.typename + "_info");
      ibox.btnMove2base.visible = false;
      ibox.btnMove2dst.visible = false;
      return;
    }

    a = ibox.walker.as(Animal);
    if (a != null) {
      ibox.title = _u(ibox.walker.typename);
      return;
    }

    ibox.title = _u("citizen");
  }

  return ibox;
}

game.ui.infobox.aboutLegion = function(walkers) {
  var ibox = game.ui.infobox.wsimple(walkers, 0, 0, 460, 350);

  var gbox = ibox.addGroupbox(0, 50, 445, 240);
  var lbNumberTitle = gbox.addLabel(90, 0, 200, 20);
  lbNumberTitle.textAlign = {h:"upperLeft", v:"center"};
  lbNumberTitle.font = "FONT_2";
  lbNumberTitle.text = _u("soldiers_in_legion")

  var lbNumberValue = gbox.addLabel(290, 0, 150, 20);
  lbNumberValue.textAlign = {h:"upperLeft", v:"center"};
  lbNumberValue.font = "FONT_2";
  lbNumberValue.text = _u("value");

  var lbHeathTitle = gbox.addLabel(90, 20, 200, 20);
  lbHeathTitle.textAlign = {h:"upperLeft", v:"center"};
  lbHeathTitle.font = "FONT_2";
  lbHeathTitle.text = _u("soldiers_health");

  var lbHealthValue = gbox.addLabel(290, 20, 150, 20);
  lbHealthValue.textAlign = {h:"upperLeft", v:"center"};
  lbHealthValue.font = "FONT_2";
  lbHealthValue.text = _u("health");

  var lbTrainedTitle = gbox.addLabel(90, 40, 200, 20);
  lbTrainedTitle.textAlign = {h:"upperLeft", v:"center"};
  lbTrainedTitle.font = "FONT_2";
  lbTrainedTitle.text = _u("soldiers_trained_in_academy");

  var lbTrainedValue = gbox.addLabel(290, 40, 150, 20);
  lbTrainedValue.textAlign = {h:"upperLeft", v:"center"};
  lbTrainedValue.font = "FONT_2";
  lbTrainedValue.text = _u("trained");

  var lbMoraleTitle = gbox.addLabel(90, 60, 200, 20);
  lbMoraleTitle.textAlign = {h:"upperLeft", v:"center"};
  lbMoraleTitle.font = "FONT_2";
  lbMoraleTitle.text = _u("soldiers_morale");

  var lbMoraleValue = gbox.addLabel(290, 60, 150, 20);
  lbMoraleValue.textAlign = {h:"upperLeft", v:"center"};
  lbMoraleValue.font = "FONT_2";
  lbMoraleValue.text = _u("morale");

  var lbFormation = gbox.addLabel(15, 180, 430, 60);
  lbFormation.textAlign = {h:"upperLeft", v:"center"};
  lbFormation.font = "FONT_2";
  lbFormation.text = _u("formation_info");
  lbFormation.style = "blackFrame";
  lbFormation.textOffset = {x:0, y:20};

  var lbFormationTitle = new Label(lbFormation);
  lbFormationTitle.geometry = {x:0, y:0, w:430, h:20};
  lbFormationTitle.textAlign = {h:"upperLeft", v:"center"};
  lbFormationTitle.font = "FONT_2_WHITE";
  lbFormationTitle.text = _u("formation_title");

  var btnReturn = gbox.PushButton(60, gbox.h-40, 140, 20);
  btnReturn.style = "whiteBorderUp";
  btnReturn.text = _u("return_to_fort");
  btnReturn.callback = function() {
    ibox.fort.returnSoldiers();
    deleteLater();
  }

  var btnReturn = gbox.PushButton(60, gbox.h-60, 140, 20);
  btnReturn.style = "whiteBorderUp";
  btnReturn.text = _u("attacK_animals_off");

  var gbox2 = ibox.addGroupbox(10, 10, 32, 100);
  var lbIcon = gbox2.addLabel(0, 0, 32, 18);
  var lbFlag = gbox2.addLabel(0, 18, 32, 47);
  var lbMoraleStandart = gbox2.addLabel(0, 47, 32, 109);

  var walkers = g_session.city.walkers(pos);
  for(var i in walkers)
  {
    var walker = walkers[i];
    var rs = walker.as(RomeSoldier);
    if (rs != null)
    {
      ibox.fort = rs.base();
      break;
    }

    var pp = walker.as(PatrolPoint);
    if (pp != null)
    {
      ibox.fort = pp.base();
      break;
    }
  }

  var fortTitle = _u("unknown_legion");
  if (ibox.fort != null)
  {
    var soldiers = ibox.fort.soldiers();

    if (soldiers.length==0)
    {
      gbox.visible = false;
      btnAttackAnimals.visible = false;
      btnReturn.visible = false;

      var barracks_n = g_session.statistic.objects.count(g_config.overlay.barracks);

      var text = barracks_n == 0
                          ? "legion_haveho_soldiers_and_barracks"
                          : "legion_haveho_soldiers";

      ibox.initInfoLabel(20, 20, ibox.w-40, ibox.h-60);
      ibox.setInfoText(_u(text));
      return;
    }
    else
    {
      _addAvailalbesFormation();
    }

    fortTitle = ibox.fort.legionName();
  }

  ibox.title = fortTitle;

  lbNumberValue.text = ibox.fort.soldiers().length;

  var health = [ "sldh_health_low", "sldh_health_sparse", "sldh_health_middle","sldh_health_strong", "sldh_health_strongest" ];
  var index = math.clamp(ibox.fort.legionHealth() / 20, 0, 4);
  lbHealthValue.text = _u(health[index]);

  var morale = [ "sldr_totally_distraught", "sldr_terrified", "sldr_very_frightened",
                 "sldr_badly_shaken", "sldr_shaken",
                 "sldr_extremely_scared", "sldr_daring", "sld_quite_daring", "sldr_encouraged", "sdlr_bold" ,"sldr_very_bold" ];

  var index = math.clamp( ibox.fort.legionMorale() / 9, 0, 9 );
  lbMoraleValue.text = _u(morale[ index ]);

  lbTrainedValue.text = ibox.fort.legionTrained();

  var text = _format( "attack_animals_{}", ibox.isAttackAnimals() ? "on" : "off" );
  btnAttackAnimals.text = _u(text);

  lbIcon.icon = ibox.fort.legionEmblem();

  var pic = g_render.picture("sprites",ibox.fort.flagIndex());
  pic.offset = {x:0, y:0};
  lbFlag.icon = pic;

  var mIndex = 20 - math.clamp( ibox.fort.legionMorale() / 5, 0, 20 );
  var pic = g_render.picture("sprites", mIndex+48);
  pic.offset = {x:0, y:0};
  lbMoraleStandart.icon = pic;

  var formations = ibox.fort.legionFormations();

  var index = 0;
  var formationPicId = [ 33, 34, 30, 31, 29, 28 ];
  var texts = [ "line_formation", "line_formation", "mopup_formation", "mopup_formation", "open_formation" ];
  for (var i in formations) {
    var formation = formations[i];
    var offset = {x:83, y:0};
    var rect = {x:16+offset.x*index, y:140, w:83, h:83};
    var btn = ibox.addButton(rect.x, rect.y, rect.w, rect.h);
    btn.style = "whiteBorderUp";
    btn.icon = {rc:"panelwindows", index:formationPicId[formation]};
    btn.iconOffset = {x:1, y:1};
    btn.tooltipText = _u("legion_formation_tooltip");
    btn.callback = function() {
      ibox.lbFormationTitle.text = _u(texts[i] + "_title");
      ibox.lbFormation.text = _u(texts[i] + "_text");
      ibox.fort.formation = i;
    }
  }
}

game.ui.infobox.aboutMerchant = function(walkers) {
  var ibox = game.ui.infobox.wsimple(walkers, 0, 0, 460, 380);
  ibox.initBlackframe(18, 40, ibox.w-36, 120);

  ibox.drawGood = function(goodType, qty, index, paintY)
  {
    var startOffset = 100;
    var offset = ( ibox.w - startOffset * 2 ) / 4;
    var outText = g_config.metric.convQty(qty);

    // pictures of goods
    var pos = {x:index * offset + startOffset, y:paintY};

    var label = ibox.addLabel(pos.x, pos.y, 100, 24);
    label.font = "FONT_2";
    label.icon = g_config.good.getInfo(goodType).picture.local;
    label.text = outText;
    label.textOffset = {x:30, y:0};
  }

  var merchant = ibox.walker.as(Merchant);
  if (merchant!=null) {
    ibox.title = _ut("trade_caravan_from") + " " + merchant.getProperty("parentCity");
    return;
  }

  var smerchant = ibox.walker.as(SeaMerchant);
  if (smerchant!=null) {
    ibox.title = _ut("trade_ship_from") + " " + smerchant.getProperty("parentCity");
    return
  }

  var bmap = merchant.mayBuy();

  ibox.addLabel(16, ibox.blackFrame.bottom() + 2, 84, 24, _u("bougth"));
  var index=0;
  for (var i in bmap) {
    ibox.drawGood(i, bmap[i], index++, ibox.blackFrame.bottom() + 2 );
  }

  index=0;
  var mmap = merchant.sold();
  ibox.addLabel(16, ibox.blackFrame.bottom() + 26, 84, 24, _u("sold"));
  for (var i in mmap) {
    if (mmap[i] > 0)
      ibox.drawGood(i,mmap[i], index++, ibox.blackFrame.bottom() + 26);
  }
}

game.ui.infobox.aboutWalker = function(walkers) {
  var ibox = game.ui.infobox.wsimple(walkers, 0, 0, 460, 350);
  ibox.initBlackframe(25, 100, ibox.w-50, 120);
  //ibox.blackFrame.style = "whiteBorder";

  ibox.updateNeighbors = function()
  {
    if (ibox.walker == null)
      return;

    for (var i in ibox.screenshots)
      ibox.screenshots[i].deleteLater();

    ibox.screenshots = [];
    var tiles = g_session.city.map.getNeighbors(ibox.walker.pos(), g_config.tilemap.AllNeighbors);
    engine.log( "g_session.city.map.tiles.length is " + tiles.length);
    var lbRect = {x:25, y:45, w:52, h:52};
    for (var i in tiles)
    {
      var tile = tiles[i];
      var tileWalkers = g_session.city.walkers(tile.pos());
      if (tileWalkers.lenth > 0)
      {
        //mini screenshot from citizen pos need here
        var ctzScreenshot = new WalkerImage(ibox);
        ctzScreenshot.geometry = lbRect;
        ctzScreenshot.style = "blackFrame";
        ctzScreenshot.tooltip = _u("click_here_to_talk_person");
        ctzScreenshot.walker = tileWalkers[i];
        ctzScreenshot.callback = function() {
          ibox.setWalker(tileWalkers[i]);
        }
        ibox.screenshots.push(ctzScreenshot);
        lbRect.x += 60;
      }
    }
  }

  ibox.updateCurrentAction = function() {
    ibox.destinationPos = ibox.walker.getProperty("plDestination");
    var ov = g_session.city.getOverlay(ibox.destinationPos);
    var action = ibox.walker.getProperty( "thoughts_action" );
    ibox.btnMove2dst.visible = (ov != null);
    if (action)
    {
      ibox.lbCurrentAction.prefixText = _u("wlk_state");
      ibox.lbCurrentAction.text = _t(action) + "(" + _ut(text) + ")";
    }
  }

  ibox.updateBaseBuilding = function() {
    var pos = ibox.walker.getProperty("plOrigin");

    ibox.baseBuildingPos = pos;
    var ov = g_session.city.getOverlay(pos);

    ibox.lbBaseBuilding.text = _u(ov.typename)
    ibox.btnMove2base.visible = ov != null;
  }

  ibox.setWalker = function(walker) {
    if (ibox.walker) {
      ibox.walker.setFlag(g_config.walker.showPath, false);
    }

    if( walker == null )
      return;

    ibox.walker = walker;
    ibox.lbName.text = walker.name();

    ibox.lbType.text = _u(walker.typename);
    ibox.lbCitizenPic.icon = g_config.walker.getBigPicture(walker.typename);

    var thinks = walker.getProperty("current_thoughts")
    ibox.lbThinks = _u(thinks);

    if (thinks.length>0) {
      var sound = thinks.substring(2, thinks.length - 4);
      g_session.playAudio(sound, 100, g_config.audio.infobox);
    }

    ibox.updateTitle();
    ibox.updateNeighbors();
    ibox.updateCurrentAction();
    ibox.updateBaseBuilding();
  }

  ibox.setWalker(ibox.walker);
  ibox.show();
}
