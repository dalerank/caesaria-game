function HouseSpec(house) {
  this.house = house;
}

Object.defineProperty( HouseSpec.prototype, "religionLevel", { get: function() {
  var res = 0;
  res += this.house.getServiceValue(g_config.service.religionMercury)>0 ? 1 : 0;
  res += this.house.getServiceValue(g_config.service.religionVenus)>0 ? 1 : 0;
  res += this.house.getServiceValue(g_config.service.religionMars)>0 ? 1 : 0;
  res += this.house.getServiceValue(g_config.service.religionNeptune)>0 ? 1 : 0;
  res += this.house.getServiceValue(g_config.service.religionCeres)>0 ? 1 : 0;
  return res;
}})

Object.defineProperty( HouseSpec.prototype, "minReligionLevel", { get: function() {
  return this.house.getProperty("minReligionLevel");
}})


Object.defineProperty( House.prototype, "spec", { get: function() {
  if (this.hspec != undefined)
    return this.hspec;

  this.hspec = new HouseSpec(this);
  return this.hspec;
}})
