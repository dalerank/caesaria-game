g_config.getObjectTypename = function(obj) {
  return g_session.getOverlayType( obj.type() )
}

//*************************** Overlay class begin ***************************************//
Object.defineProperty( Overlay.prototype, "typename", { get : function() { return g_config.getObjectTypename(this)}})

Overlay.prototype.as = function(type) { return new type(this); }
//*************************** Overlay class end ***************************************//

//*************************** Temple class begin ***************************************//
Object.defineProperty( Temple.prototype, "big", { get : function() { return this.size().w > 2 }})
Object.defineProperty( Temple.prototype, "typename", { get : function() { return g_config.getObjectTypename(this)}})
Object.defineProperty( Temple.prototype, "active", { get : function() { return this.isActive()}, set: function(en) { this.setActive(en)}})

//*************************** Temple class end ***************************************//
