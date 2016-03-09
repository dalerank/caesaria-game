/**************************************** Path class begin **************************************************************/
Object.defineProperty( Path.prototype, "str", { get: function () { return this.toString(); }} )
/*************************************** Path class end ******************************************************************/

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

Object.defineProperty( Session.prototype, "resolution", { set: function(mode) { this.setResolution(mode.w,mode.h); },
                                                          get: function() { return this.getResolution(); }} );

Object.defineProperty( Session.prototype, "credits", { get: function()  { return this.getCredits(); }} );
Object.defineProperty( Session.prototype, "workdir", { get: function () { return getPath(engine.getOption("workDir")); }});

Object.defineProperty( Session.prototype, "logfile", { get: function () {
      var path = this.workdir;
      path.add(engine.getOption("logfile"));
      return path;
}});

Object.defineProperty( Session.prototype, "missionsdir", { get: function () { return getPath(":/missions/"); }});
Object.defineProperty( Session.prototype, "savedir", { get: function () { return getPath(engine.getOption("savedir")); }});
Object.defineProperty( Session.prototype, "screenshotdir", { get: function () { return getPath(engine.getOption("screenshotDir")); }});
Object.defineProperty( Session.prototype, "city", { get: function() { return this.getCity(); }} )

var g_session = new Session();
