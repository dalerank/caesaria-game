g_config.overlay = {};

g_config.overlay.params = {
  unknown : 0,
  fire : 1,
  damage : 2,
  inflammability : 3,
  collapsibility : 4,
  destroyable : 5,
  health : 6,
  happiness : 7,
  happinessBuff : 8,
  healthBuff : 9,
  settleLock : 10,
  lockTerrain : 11,
  food : 12,
  reserveExpires : 13
};

function UpdateOverlayPrototype(ObjectPrototype) {
  Object.defineProperty(ObjectPrototype, "typename", { get : function() { return g_session.getOverlayType( this.type() )}})
  ObjectPrototype.as = function(type) { return new type(this); }
}

function UpdateWorkingBuildingPrototype(ObjectPrototype) {
  UpdateOverlayPrototype(ObjectPrototype)

  Object.defineProperty(ObjectPrototype, "active", { get : function() { return this.isActive()}, set: function(en) { this.setActive(en)}})
}

function UpdateTemplePrototype(ObjectPrototype) {
  UpdateWorkingBuildingPrototype(ObjectPrototype)
  Object.defineProperty(ObjectPrototype, "big", { get : function() { return this.size().w > 2 }})
}

function UpdateFactoryPrototype(ObjectPrototype) {
  UpdateWorkingBuildingPrototype(ObjectPrototype);
  Object.defineProperty(ObjectPrototype, "effiency", { get: function() { return this.getProperty("effiency");}})
  Object.defineProperty(ObjectPrototype, "produce", { get : function() {
      var gtype = this.getProperty("produce");
      return g_config.good.getInfo(gtype);
    }
  })
  Object.defineProperty(ObjectPrototype, "consume", { get : function() {
      var gtype = this.getProperty("consume");
      return g_config.good.getInfo(gtype);
    }
  })
}

function UpdateServiceBuildingPrototype(ObjectPrototype) {
  UpdateWorkingBuildingPrototype(ObjectPrototype);
}

UpdateOverlayPrototype(Overlay.prototype);
UpdateOverlayPrototype(Ruins.prototype);
UpdateTemplePrototype(Temple.prototype);
UpdateOverlayPrototype(Reservoir.prototype);
UpdateOverlayPrototype(House.prototype);
UpdateServiceBuildingPrototype(Fountain.prototype);
UpdateFactoryPrototype(Factory.prototype);
UpdateWorkingBuildingPrototype(Senate.prototype);
UpdateWorkingBuildingPrototype(WorkingBuilding.prototype);
UpdateWorkingBuildingPrototype(Dock.prototype);
