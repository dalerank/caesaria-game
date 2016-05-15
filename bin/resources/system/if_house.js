g_config.houseNeeds = {
  theater:1,
  amphitheater:2,
  colosseum:3
}

Object.defineProperty( House.prototype, "habitable",   { get: function() { return this.getProperty("habitable");   }})
Object.defineProperty( House.prototype, "scholar_n",   { get: function() { return this.getProperty("scholar_n");   }})
Object.defineProperty( House.prototype, "student_n",   { get: function() { return this.getProperty("scholar_n");   }})
Object.defineProperty( House.prototype, "mature_n",    { get: function() { return this.getProperty("scholar_n");   }})
Object.defineProperty( House.prototype, "isPatrician", { get: function() { return this.getProperty("isPatrician"); }})
Object.defineProperty( House.prototype, "habitantsCount", { get: function() { return this.getProperty("habitantsCount"); }})
Object.defineProperty( House.prototype, "unwishedBuildingPos", { get: function() { return this.getProperty("unwishedBuildingPos"); }})
Object.defineProperty( House.prototype, "nextEntertainmentLevel", { get : function() { return this.getProperty("nextEntertainmentLevel"); }})
Object.defineProperty( House.prototype, "minReligionLevel", { get: function() { return this.getProperty("minReligionLevel"); }})
Object.defineProperty( House.prototype, "minEducationLevel", { get: function() { return this.getProperty("minEducationLevel"); }})
Object.defineProperty( House.prototype, "minEntertainmentLevel", { get: function() { return this.getProperty("minEntertainmentLevel"); }})
Object.defineProperty( House.prototype, "minHealthLevel", { get: function() { return this.getProperty("minHealthLevel"); }})
Object.defineProperty( House.prototype, "taxRate", { get: function() { return this.getProperty("taxRate"); }})

House.prototype.getHabitantsNumber = function (type) {
    return this.getProperty(type);
}

House.prototype.hasServiceAccess = function (type) {
    return this.getServiceValue(type) > 0;
}

House.prototype.isEntertainmentNeed = function (type) {
   var lvl = this.minEntertainmentLevel;
   switch (type) {
   case g_config.service.theater: return (lvl>=10);
   case g_config.service.amphitheater: return (lvl>=30);
   case g_config.service.colosseum: return (lvl>=60);
   case g_config.service.hippodrome: return (lvl>=80);
   default: break;
   }

  return false;
}

Object.defineProperty( House.prototype, "religionLevel", { get: function() {
  var res = 0;
  res += this.getServiceValue(g_config.service.religionMercury)>0 ? 1 : 0;
  res += this.getServiceValue(g_config.service.religionVenus)>0 ? 1 : 0;
  res += this.getServiceValue(g_config.service.religionMars)>0 ? 1 : 0;
  res += this.getServiceValue(g_config.service.religionNeptune)>0 ? 1 : 0;
  res += this.getServiceValue(g_config.service.religionCeres)>0 ? 1 : 0;
  return res;
}})

House.prototype.isHealthNeed = function(type) {
  var lvl = this.minHealthLevel;
  switch (type) {
  case g_config.service.baths: return (lvl>0);
  case g_config.service.doctor: return (lvl>=1);
  case g_config.service.hospital: return (lvl>=2);
  default: break;
  }

  return false;
}

House.prototype.isEducationNeed = function (type) {
  var lvl = this.minEducationLevel;
  switch (type) {
  case g_config.service.school: return (lvl>0);
  case g_config.service.academy: return (lvl>1);
  case g_config.service.library: return (lvl>2);
  default: break;
  }

  return false;
}
