function UpdateOverlayPrototype(objProto) {
  Object.defineProperty(objProto, "typename", { get : function() { return g_session.getOverlayType( this.type() )}})
  objProto.as = function(type) { return new type(this); }
}

//*************************** Overlay class begin ***************************************//
UpdateOverlayPrototype(Overlay.prototype)
//*************************** Overlay class end ***************************************//

//*************************** Temple class begin ***************************************//
UpdateOverlayPrototype(Temple.prototype)
Object.defineProperty( Temple.prototype, "big", { get : function() { return this.size().w > 2 }})
Object.defineProperty( Temple.prototype, "active", { get : function() { return this.isActive()}, set: function(en) { this.setActive(en)}})

//*************************** Temple class end ***************************************//

UpdateOverlayPrototype(Ruins.prototype)
UpdateOverlayPrototype(Reservoir.prototype)
