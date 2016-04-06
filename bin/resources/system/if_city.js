/**************************************** City class begin **************************************************************/
PlayerCity.prototype.walkers = function(location) {
  if (typeof location == "undefined")
   return this.walkers_void();

  return this.walkers_tilepos(location); 
}
/**************************************** City class end **************************************************************/
