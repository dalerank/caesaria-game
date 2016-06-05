game.ui.infobox.aboutHouse = function(location) {
  var house = g_city.getOverlay(location).as(House);
  var habitantsCount = house.getProperty("habitantsCount");
  if (habitantsCount==0) {
    return game.ui.infobox.aboutFreeHouse(house);
  }

  g_session.playAudio("bmsel_house_00001", 100, g_config.audio.infobox);
  var ibox = this.aboutConstruction(0, 0, 510, 360);
  ibox.overlay = house;
  ibox.initBlackframe(16, 150, ibox.w - 32, ibox.h - 200);
  ibox.title = house.levelName();

  ibox.drawGood = function(house,goodType,col,row,startY)
  {
    var qty = house.store().qty(goodType);
    var lb = this.addLabel(30 + 100 * col, startY + 2 + 30 * row, 80, 50);
    lb.font = "FONT_2";
    lb.icon = g_config.good.getInfo(goodType).picture.local;
    lb.text = qty;
    lb.textOffset = {x:30, y:0};
  }

  ibox.keyPressedCallback = function(kbevent) {
    if (kbevent.control && kbevent.shift && !kbevent.pressed)
    {
      switch( kbevent.key )
      {
      case g_config.key.KEY_COMMA:
      case g_config.key.KEY_PERIOD:
        this.overlay.__debugChangeLevel(kbevent.key == g_config.key.KEY_COMMA ? +1 : -1);
      return true;
      default:
      break;
      }
    }
  }

  var lbHouseInfo = ibox.addLabel(30, 40, ibox.w-60, 100);
  lbHouseInfo.multiline = true;

  var text = house.evolveInfo();
  engine.log(text);
  house.level();
  if (house.level() >= g_config.house.level.greatPalace && text.length == 0) {
    text =  _u("greatPalace_info");
  } else {
    if (text == "##nearby_building_negative_effect##") {
      var unwishedBuildingPos = house.getProperty("unwishedBuildingPos");

      text = _t(text);
      var overlay = g_city.getOverlay(unwishedBuildingPos);
      if (overlay != null) {
        var unwishedBuildingType = overlay.typename;
        if( overlay.typename == "house" ) {
          unwishedBuildingType = overlay.as(House).levelName();// or "##unknown_house_type##";
        }

        text = _format( text, "{0}", "( " + _t(unwishedBuildingType) + " )");
      }
    }
  }

  if (text.length == 0)
    text = _u("unknown_house_state");

  lbHouseInfo.text = text;

  // citizen or patrician picture
  var picId = house.getProperty("isPatrician") ? 541 : 542;
  ibox.addImage(ibox.blackFrame.left()+15, ibox.blackFrame.top()+5, g_render.picture(g_config.rc.panel,picId));

  // number of habitants
  var freeRoomText;
  var freeRoom = house.capacity() - habitantsCount;
  if (freeRoom > 0) {
    // there is some room for new habitants!
    freeRoomText = _format( "{0} {1} {2}", habitantsCount, _ut("citizens_additional_rooms_for"), freeRoom);
  } else if (freeRoom == 0) {
    // full house!
    freeRoomText = _format( "{0} {1}", habitantsCount, _ut("occupants"));
  } else if (freeRoom < 0) {
    // too many habitants!
    freeRoomText = _format( "{0} {1} {2}", habitantsCount, _ut("no_room_for_citizens"),-freeRoom);
    lbHabitants.font = "FONT_2_RED";
  }

  var lbHabitants = ibox.addLabel(60, 157, ibox.w-90, 24);
  lbHabitants.text = freeRoomText;

  var taxes = house.getProperty("taxRate");
  engine.log(taxes);
  var taxesStr = _u("house_not_taxation");
  if (taxes > 0) {
    if (house.getServiceValue(g_config.service.forum) > 0)  {
      var size = house.size();
      taxesStr = _format( "{0} {1}", taxes * (size.w*size.h), _ut("house_pay_tax") );
    } else {
      var lastTax = house.lastTaxationDate();
      if( g_session.date.year > lastTax.year ) {
        taxesStr = _u("no_tax_in_this_year");
      } else {
        taxesStr = _u("no_visited_by_taxman");
      }
    }
  }

  ibox.addLabel(16 + 35, 177, ibox.w-80, 20, taxesStr);

  var aboutCrimes = _u("house_not_report_about_crimes");
  var lbCrime = ibox.addLabel(16 + 35, 200, ibox.w-80, 20, aboutCrimes);

  var startY = lbCrime.bottom() + 10;
  if( house.level() > g_config.house.level.tent )
  {
    ibox.drawGood(house, g_config.good.wheat, 0, 0, startY);
    ibox.drawGood(house, g_config.good.fish, 1, 0, startY );
    ibox.drawGood(house, g_config.good.meat, 2, 0, startY );
    ibox.drawGood(house, g_config.good.fruit, 3, 0, startY );
    ibox.drawGood(house, g_config.good.vegetable, 4, 0, startY );
  }
  else
  {
    var lb = ibox.addLabel(lbCrime.left(), lbCrime.bottom() + 5, ibox.w-80, 40);
    lb.font = "FONT_1"
    lb.text = _u("house_provide_food_themselves");
    lb.multiline = true;
    startY = lb.top();
  }

  ibox.drawGood(house, g_config.good.pottery, 0, 1, startY );
  ibox.drawGood(house, g_config.good.furniture, 1, 1, startY );
  ibox.drawGood(house, g_config.good.oil, 2, 1, startY );
  ibox.drawGood(house, g_config.good.wine, 3, 1, startY );

  var btnHabitants = ibox.addButton(ibox.btnInfo.right(), ibox.btnInfo.top(), 24, 24);
  btnHabitants.text = "h";
  btnHabitants.style = "whiteBorderUp";
  /*btnHabitants.callback = function() {
    std::string workerState = utils::format( 0xff, "Live=%d\nUnemployed=%d\nHired=%d\nNewborn=%d\nChild=%d\nIn school=%d\nStudents=%d\nMature=%d\nAged(not work)=%d",
                                                  _house->habitants().count(),
                                                  (int)_house->unemployed(),
                                                  _house->hired(),
                                                  _house->habitants().count( CitizenGroup::newborn ),
                                                  _house->habitants().child_n(),
                                                  _house->habitants().scholar_n(),
                                                  _house->habitants().student_n(),
                                                  _house->habitants().mature_n(),
                                                  _house->habitants().aged_n() );

                                                  Dialogbox& dialog = ui()->add<Dialogbox>( Rect( 0, 0, 400, 400 ), "Habitants", workerState, Dialogbox::btnYes );
  }*/

  var btnServices = ibox.addButton(btnHabitants.right(), btnHabitants.top(), 24, 24);
  btnServices.text = "s";
  btnServices.style = "whiteBorderUp";
  /*btnServices.callback = function() {
    std::string srvcState = fmt::format( "Health={}",
                                        (int)_house->state( pr::health ));

                                        Dialogbox& dialog = ui()->add<Dialogbox>( Rect( 0, 0, 400, 400 ), "Services", srvcState, Dialogbox::btnYes );
  }*/

  ibox.show();
}

game.ui.infobox.aboutFreeHouse = function(house) {
  var ibox = this.aboutConstruction(0, 0, 480, 320);
  ibox.title = _u("freehouse_caption");
  ibox.overlay = house;

  var haveRoadAccess = house.getProperty("haveRoadAccess");
  ibox.initInfoLabel(20, 20, ibox.w-40, ibox.h-60);
  ibox.setInfoText( haveRoadAccess ? _u("freehouse_text_noroad")
                                   : _u("freehouse_text"));

  ibox.show();
}
