function UpdateWalkerPrototype(ObjectPrototype) {
  Object.defineProperty(ObjectPrototype, "typename", { get : function() { return g_session.getWalkerType(this.type())}})
  ObjectPrototype.as = function(type) { return new type(this); }
}

/***************** Overlays ******************/
UpdateWalkerPrototype(Walker.prototype);
UpdateOverlayPrototype(Ruins.prototype);
UpdateOverlayPrototype(House.prototype);
UpdateOverlayPrototype(Gatehouse.prototype);
UpdateOverlayPrototype(Reservoir.prototype);

/**************** Working buildings **********/
UpdateWorkingBuildingPrototype(Senate.prototype);
UpdateWorkingBuildingPrototype(WorkingBuilding.prototype);
UpdateWorkingBuildingPrototype(Dock.prototype);
UpdateWorkingBuildingPrototype(Warehouse.prototype);
UpdateWorkingBuildingPrototype(Granary.prototype);

/*************** Service buildings ************/
UpdateServiceBuildingPrototype(Fountain.prototype);
UpdateServiceBuildingPrototype(Market.prototype);

/*************** Other buildings **************/
UpdateTemplePrototype(Temple.prototype);
UpdateFactoryPrototype(Factory.prototype);
