//*************************** Overlay class begin ***************************************//
function UpdateOverlayPrototype(objProto) {
  Object.defineProperty(objProto, 'typename', { get : function() { return g_session.getOverlayType( obj.type() )}})

  objProto.as = function(type) { return new type(this); }
}

UpdateOverlayPrototype(Overlay)
//*************************** Overlay class end ***************************************//

function UpdateWorkingBuildingPrototype(objProto) {
  UpdateOverlayPrototype(objProto)

  Object.defineProperty(objProto.prototype, "active", { get : function() { return this.isActive()}, set: function(en) { this.setActive(en)}})
}
//*************************** Temple class begin ***************************************//
UpdateOverlayPrototype(Temple)

Object.defineProperty(Temple.prototype, "big", { get : function() { return this.size().w > 2 }})
//*************************** Temple class end ***************************************//

//*************************** Ruin class begin ***************************************//
UpdateOverlayPrototype(Ruins)
//*************************** Ruin class end ***************************************//
