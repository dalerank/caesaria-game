/**************************************** City class begin **************************************************************/
PlayerCity.prototype.walkers = function (location) {
    if (typeof location == "undefined")
        return this.walkers_void();

    return this.walkers_tilepos(location);
}

Object.defineProperty(PlayerCity.prototype, "map", {
    get: function () {
        return this.tilemap()
    }
});
/**************************************** City class end **************************************************************/