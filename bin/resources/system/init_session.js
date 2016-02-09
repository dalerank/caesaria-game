function Session() {
	this.session = new _Session();
}

Label.prototype = {
	continuePlay : function(years) { this.session.continuePlay(years); },
	loadNextMission : function() { this.session.loadNextMission(); }
}