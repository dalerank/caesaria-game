function Path (path) {
    this.path = new _Path();
    if(path)
      this.assign(path);
}

Path.prototype = {
    get exist () { return this.path.exist(); },
    get str () { return this.path.toString(); },

    add : function(str) { return this.path.add(str); },
    assign : function(str) { this.path.set(str); },
}

function MissionInfo (path) {
    this.info = _MissionInfo();
    if (path)
        this.load(path);
}

MissionInfo.prototype = {
    load: function(path) { this.info.load(path); },

    get title () { return this.info.get("preview.title"); },
    get localization () { return this.info.get("localization.name"); },
    get image () { return this.info.get("preview.image"); },
    get desc  () { return this.info.get("preview.text"); },
    get map   () { return this.info.get("map"); }
}

function Session() {
    this.session = new _Session();
}

Session.prototype = {
    continuePlay : function(years) { this.session.continuePlay(years); },
    loadNextMission : function() { this.session.loadNextMission(); } ,
    getVideoMode : function(index) { return this.session.getVideoMode(index); },
    openUrl      : function(url) { return this.session.openUrl(url); },
    setOption    : function(name,value) { this.session.setOption(name,value); },
    setMode      : function(mode) { return this.session.setMode(mode); },
    tradableGoods : function() { return this.session.tradableGoods(); },
    getFiles    : function(dir,ext) { return this.session.getFiles(dir,ext); },
    getGoodInfo : function(name) { return this.session.getGoodInfo(name); },
    playAudio   : function(name,volume,type) { return this.session.playAudio(name,volume,type); },
    setFont     : function(fontname) { this.session.setFont(fontname); },
    loadLocalization : function(path) { this.session.loadLocalization(path); },
    setLanguage : function(lang,sounds) { this.session.setLanguage(lang,sounds); },
    clearUi     : function() { this.session.clearUi(); },
    findWidget  : function(name) { return this.session.findWidget(name); },

    get credits ()  { return this.session.getCredits(); },
    get videoModesCount () { return this.session.videoModesCount(); },
    get lastChangesNum () { return this.session.lastChangesNum(); },
    get resolution () { return this.session.getResolution(); },
    set resolution (mode) { this.session.setResolution(mode.w,mode.h); },

    get workdir () {
      var path = new Path();
      path.assign(engine.getOption("workDir"));
      return path;
    },

    get logfile () {
      var path = this.workdir;
      path.add(engine.getOption("logfile"));
      return path;
    },

    get missionsdir () {
        var path = new Path();
        path.assign(":/missions/");
        return path;
    },

    get savedir () {
        var path = new Path();
        path.assign(engine.getOption("savedir"));
        return path;
    },

    get screenshotdir () {
        var path = new Path();
        path.assign(engine.getOption("screenshotDir"));
        return path;
    }
}

var g_session = new Session();
