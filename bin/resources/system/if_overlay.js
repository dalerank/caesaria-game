g_config.overlay = {};

g_config.overlay.params = {
    unknown: 0,
    fire: 1,
    damage: 2,
    inflammability: 3,
    collapsibility: 4,
    destroyable: 5,
    health: 6,
    happiness: 7,
    happinessBuff: 8,
    healthBuff: 9,
    settleLock: 10,
    lockTerrain: 11,
    food: 12,
    reserveExpires: 13
};

function UpdateOverlayPrototype(ObjectPrototype, name) {
    if (ObjectPrototype == undefined) {
      engine.log( "WARNING !!! Cant update prototype for " + name);
      return;
    }

    Object.defineProperty(ObjectPrototype, "typename", {
        get: function () {
            return g_session.getOverlayType(this.type())
        }
    })

    Object.defineProperty(ObjectPrototype, "name", {
        get: function () { return this.getProperty("name"); }
    })

    ObjectPrototype.as = function (type) {
        return new type(this);
    }
}

function UpdateWorkingBuildingPrototype(ObjectPrototype, name) {
    UpdateOverlayPrototype(ObjectPrototype, name);

    Object.defineProperty(ObjectPrototype, "active", {
        get: function () {
            return this.isActive()
        },
        set: function (en) {
            this.setActive(en)
        }
    })

    Object.defineProperty(ObjectPrototype, "numberWorkers", { get: function () { return this.getProperty("numberWorkers"); }})
    Object.defineProperty(ObjectPrototype, "maximumWorkers", { get: function () { return this.getProperty("maximumWorkers"); }})
    Object.defineProperty(ObjectPrototype, "needWorkers", { get: function () { return this.getProperty("needWorkers"); }})
}

function UpdateTemplePrototype(ObjectPrototype, name) {
    UpdateWorkingBuildingPrototype(ObjectPrototype, name);
    Object.defineProperty(ObjectPrototype, "big", {
        get: function () {
            return this.size().w > 2
        }
    })
}

function UpdateFactoryPrototype(ObjectPrototype, name) {
    UpdateWorkingBuildingPrototype(ObjectPrototype, name);
    Object.defineProperty(ObjectPrototype, "effiency", {
        get: function () {
            return this.getProperty("effiency");
        }
    })

    Object.defineProperty(ObjectPrototype, "productivity", {
        get: function () {
            return this.getProperty("productivity");
        }
    })

    Object.defineProperty(ObjectPrototype, "produce", {
        get: function () {
            var gtype = this.getProperty("produce");
            return g_config.good.getInfo(gtype);
        }
    })
    Object.defineProperty(ObjectPrototype, "consume", {
        get: function () {
            var gtype = this.getProperty("consume");
            return g_config.good.getInfo(gtype);
        }
    })
}

function UpdateServiceBuildingPrototype(ObjectPrototype, name) {
    UpdateWorkingBuildingPrototype(ObjectPrototype, name);
}

function UpdateEntertainmentBuildingPrototype(ObjectPrototype, name) {
    UpdateServiceBuildingPrototype(ObjectPrototype, name);

    Object.defineProperty(ObjectPrototype, "currentVisitors", {
        get: function () { return this.getProperty("currentVisitors"); }
    })
}

function UpdateEducationBuildingPrototype(ObjectPrototype, name) {
    UpdateServiceBuildingPrototype(ObjectPrototype, name);
}

function UpdateHEalthBuildingPrototype(ObjectPrototype, name) {
    UpdateServiceBuildingPrototype(ObjectPrototype, name);
}

function UpdateTrainingBuildingPrototype(ObjectPrototype, name) {
    UpdateWorkingBuildingPrototype(ObjectPrototype, name);
}

/***************** Overlays ******************/
UpdateOverlayPrototype(Overlay.prototype, "Overlay");
UpdateOverlayPrototype(Ruins.prototype, "Ruins");
UpdateOverlayPrototype(House.prototype, "House");
UpdateOverlayPrototype(Gatehouse.prototype, "Gatehouse");
UpdateOverlayPrototype(Reservoir.prototype, "Reservoir");
UpdateOverlayPrototype(FortArea.prototype, "FortArea");

/**************** Working buildings **********/
UpdateWorkingBuildingPrototype(Senate.prototype, "Senate");
UpdateWorkingBuildingPrototype(WorkingBuilding.prototype, "WorkingBuilding");
UpdateWorkingBuildingPrototype(Dock.prototype, "Dock");
UpdateWorkingBuildingPrototype(Warehouse.prototype, "Warehouse");
UpdateWorkingBuildingPrototype(Granary.prototype, "Granary");
UpdateWorkingBuildingPrototype(Fort.prototype, "Fort");

/*************** Training buildings ************/
UpdateTrainingBuildingPrototype(Barracks.prototype, "Barracks");
UpdateTrainingBuildingPrototype(WorkshopChariot.prototype, "WorkshopChariot");

/*************** Service buildings ************/
UpdateServiceBuildingPrototype(Fountain.prototype, "Fountain");
UpdateServiceBuildingPrototype(Market.prototype, "Market");

/*************** Health buildings ************/
UpdateHealthBuildingPrototype(HealthBuilding.prototype, "HealthBuilding");

/*************** Other buildings **************/
UpdateTemplePrototype(Temple.prototype, "Temple");
UpdateFactoryPrototype(Factory.prototype, "Factory");

/*************** Education buildings ******************/
UpdateEducationBuildingPrototype(EducationBuilding.prototype, "EducationBuilding");

/*************** Entertainment buildings ******************/
UpdateEntertainmentBuildingPrototype(EntertainmentBuilding.prototype, "EntertainmentBuilding");
UpdateEntertainmentBuildingPrototype(Theater.prototype, "Theater");
UpdateEntertainmentBuildingPrototype(Amphitheater.prototype, "Amphitheater");
