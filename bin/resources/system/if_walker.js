function UpdateWalkerPrototype(ObjectPrototype) {
    Object.defineProperty(ObjectPrototype, "typename", {
        get: function () {
            return g_session.getWalkerType(this.type())
        }
    })
    ObjectPrototype.as = function (type) {
        return new type(this);
    }
}

/***************** Overlays ******************/
UpdateWalkerPrototype(Walker.prototype);
