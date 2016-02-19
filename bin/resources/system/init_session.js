function Session() {
	this.session = new _Session();
} 

Session.prototype = {
    continuePlay : function(years) { this.session.continuePlay(years); },
    loadNextMission : function() { this.session.loadNextMission(); } ,
    setResolution : function(size) { return this.session.setResolution(size.w,size.h); },
    getVideoMode : function(index) { return this.session.getVideoMode(index); },
    openUrl      : function(url) { return this.session.openUrl(url); },
    quitGame     : function() { return this.session.quitGame(); },
    tradableGoods : function() { return this.session.tradableGoods(); },
    getGoodInfo : function(name) { return this.session.getGoodInfo(name); },
    playAudio : function(name,volume,type) { return this.session.playAudio(name,volume,type); },
    startCareer : function() { this.session.startCareer(); },
		setFont: function(fontname) { this.session.setFont(fontname); },
		reloadScene : function() { this.session.reloadScene(); },
		setLanguage : function(lang,sounds) { this.session.setLanguage(lang,sounds); },

    get credits ()  { return this.session.getCredits(); },
    get videoModesCount () { return this.session.videoModesCount(); },
    get lastChangesNum () { return this.session.lastChangesNum(); },		
    get resolution () { return this.session.getResolution(); }
}  
