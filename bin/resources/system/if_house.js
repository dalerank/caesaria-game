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

Object.defineProperty( HouseSpec.prototype, "minReligionLevel", { get: function() { return this.house.getProperty("minReligionLevel"); }})
Object.defineProperty( HouseSpec.prototype, "minEducationLevel", { get: function() { return this.house.getProperty("minEducationLevel"); }})
Object.defineProperty( HouseSpec.prototype, "taxRate", { get: function() { return this.house.getProperty("taxRate"); }})

Object.defineProperty( House.prototype, "spec", { get: function() {
  if (this.hspec != undefined)
    return this.hspec;

  this.hspec = new HouseSpec(this);
  return this.hspec;
}})

Object.defineProperty( House.prototype, "habitable",   { get: function() { return this.getProperty("habitable");   }})
Object.defineProperty( House.prototype, "scholar_n",   { get: function() { return this.getProperty("scholar_n");   }})
Object.defineProperty( House.prototype, "student_n",   { get: function() { return this.getProperty("scholar_n");   }})
Object.defineProperty( House.prototype, "mature_n",    { get: function() { return this.getProperty("scholar_n");   }})
Object.defineProperty( House.prototype, "isPatrician", { get: function() { return this.getProperty("isPatrician"); }})
Object.defineProperty( House.prototype, "habitantsCount", { get: function() { return this.getProperty("habitantsCount"); }})
Object.defineProperty( House.prototype, "unwishedBuildingPos", { get: function() { return this.getProperty("unwishedBuildingPos"); }})

House.prototype.isEducationNeed = function (type) {
  var lvl = this.spec.minEducationLevel;
  switch( type )
  {
  case g_config.service.school: return (lvl>0);
  case g_config.service.academy: return (lvl>1);
  case g_config.service.library: return (lvl>2);
  default: break;
  }

  return false;
}
