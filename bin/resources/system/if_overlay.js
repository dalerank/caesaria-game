function UpdateOverlayPrototype(objProto) {
  Object.defineProperty(objProto, "typename", { get : function() { return g_session.getOverlayType( this.type() )}})
  objProto.as = function(type) { return new type(this); }
}

//*************************** Overlay class begin ***************************************//
UpdateOverlayPrototype(Overlay.prototype)
//*************************** Overlay class end ***************************************//

function UpdateWorkingBuildingPrototype(objProto) {
  UpdateOverlayPrototype(objProto)

  Object.defineProperty(objProto.prototype, "active", { get : function() { return this.isActive()}, set: function(en) { this.setActive(en)}})
}
//*************************** Temple class begin ***************************************//
UpdateWorkingBuildingPrototype(Temple)
Object.defineProperty(Temple.prototype, "big", { get : function() { return this.size().w > 2 }})
//*************************** Temple class end ***************************************//

UpdateOverlayPrototype(Reservoir.prototype)
//*************************** Ruin class begin ***************************************//
UpdateOverlayPrototype(Ruins)
//*************************** Ruin class end ***************************************//
