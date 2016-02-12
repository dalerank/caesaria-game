function Session() {
	this.session = new _Session();
} 

Session.prototype = {
    continuePlay : function(years) { this.session.continuePlay(years); },
    loadNextMission : function() { this.session.loadNextMission(); } ,
    setResolution : function(size) { return this.session.setResolution(size.w,size.h); },
    getVideoMode : function(index) { return this.session.getVideoMode(index); },
		
    get videoModesCount () { return this.session.videoModesCount(); },
    get lastChangesNum () { return this.session.lastChangesNum(); }
    get resolution () { return this.session.getResolution(); }
}  
