function Session() {
	this.session = new _Session();
}

Session.prototype = {
	continuePlay : function(years) { this.session.continuePlay(years); },
	loadNextMission : function() { this.session.loadNextMission(); } 
} 