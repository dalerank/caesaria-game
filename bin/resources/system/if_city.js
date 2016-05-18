/**************************************** City class begin **************************************************************/
PlayerCity.prototype.walkers = function (location) {
    if (typeof location == "undefined")
        return this.walkers_void();

    return this.walkers_tilepos(location);
}

Object.defineProperty(PlayerCity.prototype, "map", { get: function () { return this.tilemap() }, configurable : true });
Object.defineProperty(PlayerCity.prototype, "population", { get: function () { return this.getProperty("population"); }, configurable : true });
Object.defineProperty(PlayerCity.prototype, "health", { get: function () { return this.getProperty("health"); }, configurable : true });
Object.defineProperty(PlayerCity.prototype, "workingBuildings", { get: function () { return g_session.getWorkingBuildings(); }, configurable : true });
Object.defineProperty(PlayerCity.prototype, "worklessPercent", { get: function () { return this.getProperty("worklessPercent"); }, configurable : true });
Object.defineProperty(PlayerCity.prototype, "profit", { get: function () { return this.getProperty("profit"); }, configurable : true });
Object.defineProperty(PlayerCity.prototype, "migrationReason", { get: function() { return this.getProperty("migrationReason"); }})
Object.defineProperty(PlayerCity.prototype, "isRomeSendWheat", { get: function() { return this.getProperty("isRomeSendWheat"); }})
Object.defineProperty(PlayerCity.prototype, "disorderReason", { get: function() { return this.getProperty("disorderReason"); }})
Object.defineProperty(PlayerCity.prototype, "cityHealthReason", { get: function() { return this.getProperty("cityHealthReason"); }})
Object.defineProperty(PlayerCity.prototype, "theatreCoverage", { get: function() { return this.getProperty("theatreCoverage"); }})
Object.defineProperty(PlayerCity.prototype, "threatValue", { get: function() { return this.getProperty("threatValue"); }})
Object.defineProperty(PlayerCity.prototype, "religionReason", { get: function() { return this.getProperty("religionReason"); }})
Object.defineProperty(PlayerCity.prototype, "sentimentReason", { get: function() { return this.getProperty("sentimentReason"); }})

Object.defineProperty(PlayerCity.prototype, "lastFestivalDate", {
  get: function() {
      var dt = this.getProperty("lastFestivalDate");
      var ret = new DateTime();
      ret.setDate(dt.year, dt.month, dt.day);
      return ret;
  }
})

PlayerCity.prototype.findOverlays = function(type) {
  return g_session.getOverlays(type);
}

PlayerCity.prototype.getOverlaysNumber = function(type) {
  return g_session.getOverlaysNumber(type);
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

Object.defineProperty(PlayerCity.prototype, "houses", {
  get: function() { return g_session.getOverlays("house") }
})

Object.defineProperty(PlayerCity.prototype, "laborDetails", {
  get: function () {
    var ret = {map : {}, current: 0, need : 0};

    var buildings = this.workingBuildings;
    for (var i=0; i < buildings.length; i++) {
      var bld = buildings[i].as(WorkingBuilding);

      if (bld == null)
        continue;

      ret.current += bld.numberWorkers;
      ret.need += bld.maximumWorkers;

      ret.map[bld.workerType] += bld.numberWorkers;
    }

    return ret;
  }, configurable : true
})
/**************************************** City class end **************************************************************/

var g_city = null;
