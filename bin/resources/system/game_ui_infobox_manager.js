game.ui.infobox.show = function(typename,location)
{
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
  case "native_center": case "high_bridge": case "fortification":
  case "wall":
    game.ui.infobox.aboutObject(typename)
  break;

  case "low_bridge":
    game.ui.infobox.aboutObject(typename,_u("bridge_extends_city_area"))
  break;

  case "collapsed_ruins": case "burned_ruins": case "plague_ruins":
    game.ui.infobox.aboutRuins(location)
  break;

  case "burning_ruins":
    game.ui.infobox.aboutObject(typename,_u("this_fire_can_spread"))
  break;

  case "rift":
    game.ui.infobox.aboutObject(typename,_u("these_rift_info"))
  break;

  case "barracks":
    game.ui.infobox.aboutBarracks(location);
  break;
  }
}

function OnShowOverlayInfobox(location) {
  var overlay = g_session.city.getOverlay(location)
  game.ui.infobox.show(overlay.typename, location)
}
