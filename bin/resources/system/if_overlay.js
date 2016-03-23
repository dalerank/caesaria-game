//*************************** Overlay class begin ***************************************//
Object.defineProperty( Overlay.prototype, "typename", { get : function() {
    var t = this.type();
    return g_session.getOverlayType(t);
}})

//*************************** Overlay class end ***************************************//
