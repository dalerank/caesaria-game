/**************************************** Path class begin **************************************************************/
Object.defineProperty( Path.prototype, "str", { get: function () { return this.toString(); }} )
Path.prototype.slice = function(str) {
  var p = new Path()
  p.set(this.str)
  p.add(str)
  return p
}
/*************************************** Path class end ******************************************************************/

/**************************************** Emperor class begin **************************************************************/

/**************************************** Emperor class end **************************************************************/

/**************************************** Player class begin **************************************************************/
Player.prototype.removeMoney = function(value) {
  this.appendMoney(-value)
}
/**************************************** Player class end **************************************************************/

/**************************************** MissionInfo class begin **************************************************************/
Object.defineProperty( MissionInfo.prototype, "title", { get: function() { return this.get("preview.title"); }} )
Object.defineProperty( MissionInfo.prototype, "localization", { get: function () { return this.get("localization.name"); }} )
Object.defineProperty( MissionInfo.prototype, "image", { get: function() { return this.get("preview.image"); }} )
Object.defineProperty( MissionInfo.prototype, "desc", { get: function  () { return this.get("preview.text"); }} )
Object.defineProperty( MissionInfo.prototype, "map", { get: function() { return this.get("map"); }} )
/*************************************** MissionInfo class end ******************************************************************/

Session.prototype.getMissionInfo = function(path) {
    var info = new MissionInfo();
    info.load(path);
    return info;
}

Session.prototype.getPath = function(p) {
    var path = new Path();
    path.set(p);
    return path;
}

Session.prototype.getOptPath = function(p) {
    var path = new Path();
    path.set(engine.getOption(p));
    return path;
}

Object.defineProperty( Session.prototype, "resolution", { set: function(mode) { this.setResolution(mode.w,mode.h) },
                                                          get: function() { return this.getResolution() }} )
Object.defineProperty( Session.prototype, "date", { get: function()  { return this.getGameDate() }} )
Object.defineProperty( Session.prototype, "credits", { get: function()  { return this.getCredits() }} )
Object.defineProperty( Session.prototype, "player", { get: function()  { return this.getPlayer() }} )
Object.defineProperty( Session.prototype, "city", { get: function()  { return this.getCity() }} )
Object.defineProperty( Session.prototype, "emperor", { get: function()  { return this.getEmperor() }} )
Object.defineProperty( Session.prototype, "c3mode", { get: function()  { return this.isC3mode() }} )
Object.defineProperty( Session.prototype, "workdir", { get: function () { return getPath(engine.getOption("workDir")) }})

Object.defineProperty( Session.prototype, "logfile", { get: function () {
      var path = this.workdir;
      path.add(engine.getOption("logfile"));
      return path;
}});

Object.defineProperty( Session.prototype, "missionsdir", { get: function () { return getPath(":/missions/"); }});
Object.defineProperty( Session.prototype, "savedir", { get: function () { return getPath(engine.getOption("savedir")); }});
Object.defineProperty( Session.prototype, "screenshotdir", { get: function () { return getPath(engine.getOption("screenshotDir")); }});

var g_session = new Session();
