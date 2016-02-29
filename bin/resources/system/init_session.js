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

function Session() {
    this.session = new _Session();
}

Session.prototype = {
    continuePlay : function(years) { this.session.continuePlay(years); },
    loadNextMission : function() { this.session.loadNextMission(); } ,
    getVideoMode : function(index) { return this.session.getVideoMode(index); },
    openUrl      : function(url) { return this.session.openUrl(url); },
    setMode      : function(mode) { return this.session.setMode(mode); },
    tradableGoods : function() { return this.session.tradableGoods(); },
    getGoodInfo : function(name) { return this.session.getGoodInfo(name); },
    playAudio   : function(name,volume,type) { return this.session.playAudio(name,volume,type); },
    startCareer : function() { this.session.startCareer(); },
    setFont     : function(fontname) { this.session.setFont(fontname); },
    reloadScene : function() { this.session.reloadScene(); },
    setLanguage : function(lang,sounds) { this.session.setLanguage(lang,sounds); },
    clearUi     : function() { this.session.clearUi(); },

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
}

var g_session = new Session();
