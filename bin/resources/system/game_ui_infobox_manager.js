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
  }
}

function OnShowOverlayInfobox(location) {
  var overlay = g_session.city.getOverlay(location)
  game.ui.infobox.show(overlay.typename, location)
}
