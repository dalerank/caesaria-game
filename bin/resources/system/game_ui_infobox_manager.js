game.ui.infobox.show = function(typename,location)
{
  engine.log(typename);
  switch(typename) {
  case "small_ceres_temple": case "small_mars_temple":
  case "small_neptune_temple": case "small_venus_temple":
  case "small_mercury_temple": case "big_ceres_temple":
  case "big_mars_temple": case "big_neptune_temple":
  case "big_venus_temple": case "big_mercury_temple":
  case "oracle":
    game.ui.infobox.aboutTemple(location)
  break;

  case "roadBlock": case "elevation": case "aqueduct":
  case "garden": case "statue_small": case "statue_middle":
  case "statue_big": case "native_hut": case "native_field":
  case "native_center": case "fortification":
  case "wall":
    game.ui.infobox.aboutObject(typename);
  break;

  case "iron_mine": case "quarry": case "lumber_mill": case "clay_pit":
  case "meat_farm": case "fig_farm": case "olive_farm": case "vegetable_farm":
  case "wheat_farm": case "vinard":
    game.ui.infobox.aboutRaw(location);
  break;

  case "prefecture": case "engineering_post": case "clinic":
  case "baths": case "barber": case "hospital": case "missionaryPost":
  case "school": case "academy": case "library": case "actorColony":
  case "gladiatorSchool": case "lionsNursery": case "hippodrome":
  case "chariotSchool": case "forum":
    game.ui.infobox.aboutServiceBuilding(location);
  break;

  case "dock":
    game.ui.infobox.aboutDock(location);
  break;

  case "amphitheater":
    game.ui.infobox.aboutAmphitheater(location);
  break;

  case "governorHouse": case "governorVilla": case "governorPalace":
    var text = _u(typename + "_info")
    game.ui.infobox.aboutServiceBuilding(location, text);
  break;

  case "well":
    game.ui.infobox.aboutWell(location);
  break;

  case "house":
    game.ui.infobox.aboutHouse(location);
  break;

  case "colosseum":
    game.ui.infobox.aboutColosseum(location);
  break;

  case "shipyard":
    game.ui.infobox.aboutShipyard(location);
  break;

  case "theater":
    game.ui.infobox.aboutTheater(location);
  break;

  case "wharf":
    game.ui.infobox.aboutWharf(location);
  break;

  case "market":
    game.ui.infobox.aboutMarket(location);
  break;

  case "pottery_workshop": case "weapons_workshop": case "furniture_workshop":
  case "wine_workshop": case "oil_workshop":
    game.ui.infobox.aboutFactory(location);
  break;

  case "senate":
    game.ui.infobox.aboutSenate(location);
  break;

  case "fountain":
    game.ui.infobox.aboutFountain(location);
  break;

  case "reservoir":
    game.ui.infobox.aboutReservoir(location);
  break;

  case "low_bridge": case "high_bridge":
    game.ui.infobox.aboutObject(typename,_u("bridge_extends_city_area"))
  break;

  case "collapsed_ruins": case "burned_ruins":
  case "plague_ruins", "burning_ruins":
    game.ui.infobox.aboutRuins(location)
  break;

  case "warehouse":
    game.ui.infobox.aboutWarehouse(location);
  break;

  case "military_academy":
    game.ui.infobox.aboutMillitaryAcademy(location);
  break;

  case "gatehouse":
    game.ui.infobox.aboutGatehouse(location);
  break;

  case "fort_legionaries": case "fort_javelin": case "fort_horse":
  case "fortArea":
    game.ui.infobox.aboutFort(location);
  break;

  case "granary":
    game.ui.infobox.aboutGranary(location);
  break;

  case "rift":
    game.ui.infobox.aboutObject(typename,_u("these_rift_info"))
  break;

  case "barracks":
    game.ui.infobox.aboutBarracks(location);
  break;

  default:
    game.ui.infobox.aboutLand(location);
  break;
  }
}

game.ui.infobox.tryShow = function (location) {
  var walkers = g_session.city.walkers(location);
  if (walkers.length > 0) {
    game.ui.infobox.wshow(walkers, location);
  } else {
    var overlay = g_session.city.getOverlay(location)
    game.ui.infobox.show(overlay.typename, location)
  }
}

function OnShowOverlayInfobox(location) {
  game.ui.infobox.tryShow(location);
}
