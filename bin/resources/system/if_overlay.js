//*************************** Overlay class begin ***************************************//
Object.defineProperty( Overlay.prototype, "typename", { get : function() {Overlay.getTypename(this)}})

Overlay.prototype.as = function(type) { return new type(this); }

Overlay.getTypename = function(obj) {
  return g_session.getOverlayType( obj.type() )
}

//*************************** Overlay class end ***************************************//

//*************************** Temple class begin ***************************************//
Object.defineProperty( Temple.prototype, "big", { get : function() { return this.size().w > 2 }})
Object.defineProperty( Temple.prototype, "typename", { get : function() {Overlay.getTypename(this)}})

//*************************** Temple class end ***************************************//
