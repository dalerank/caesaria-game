/**************************************** City class begin **************************************************************/
PlayerCity.prototype.walkers = function (location) {
    if (typeof location == "undefined")
        return this.walkers_void();

    return this.walkers_tilepos(location);
}

Object.defineProperty(PlayerCity.prototype, "map", {
    get: function () {
        return this.tilemap()
    },
    configurable : true
});

PlayerCity.prototype.findNextSameBuilding = function(location, type) {
  var locations = g_session.getBuildingLocations(type);

  if (locations.length > 1) {
    locations.push(locations[0]);
  }

  for (var i=0; i < locations.length; i++) {
    var tmp = locations[i];
    if (location.i == tmp.i && location.j == tmp.j) {
        return locations[i+1];
    }
  }

  return {i:-1, j:-1};
}

PlayerCity.prototype.findPrevSameBuilding = function(location, type) {
  var locations = g_session.getBuildingLocations(type);

  if (locations.length > 1)
    locations.splice(0, 0, locations[locations.length-1]);

  for (var i=0; i < locations.length; i++) {
    var tmp = locations[i];
    if (location.i == tmp.i && location.j == tmp.j) {
        return locations[i-1];
    }
  }

  return {i:-1, j:-1};
}
/**************************************** City class end **************************************************************/
