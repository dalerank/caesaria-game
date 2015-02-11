{
  terrain : {
    class : "terrain"
    image.ext : {
      type_1 : { size:1, rc : "land1a", start : 62, count : 1 }
    }
  }

  water : {
    class : "water"
    image.ext : {
      type_1 : { size:1, rc : "land1a", start : 120, count : 8 }
    }
  }

  meadow : {
    class : "terrain"
    image.ext : {
      type_1 : { size:1, rc : "land1a", start : 18, count : 11 }
    }
  }

  roadBlock : {
    cost : 10
    class : "road"
    image : [ "roadBlock", 1 ]
  }

  tree : {
    class : "tree"
    image.ext : {
      type_1 : { size:1, rc : "land1a", start : 10, count : 1 }
    }
  }

  road : {
    cost : 4,
    desirability : { base : 1, range : 0, step : 0 }
    class : "road"
    image : [ "land2a", 44 ]
  },

  elevation : {
    cost : 0
    class : "road"
  }
  
  wall : {
    desirability : 12,
    range : 0,
    class : "wall"
    cost : 10
  },

  fortification : {
    desirability : 16
    range : 0
    class : "wall"
    cost : 20
  }
  
  aqueduct : {
    cost : 8,
    desirability : { base: -2, range : 2, step : 1 }
    class : "water"
    sound : [ "aqueduct", 1 ]
  },

  amphitheater : {
    cost : 100,
    desirability : { base : 4, range : 4, step : -1 }
    employers : 12
    class : "entertainment"    
    animation : { rc : "entertainment", start : 2, count : 10, offset : [ 100, 49 ], delay : 3 }
    image : [ "entertainment", 1 ]
    sound : [ "ampitheatre", 1 ]
  },

  theater : {
    cost : 50,
    desirability : { base : 2, range : 2, step : -1 }
    employers : 8
    class : "entertainment"
    image : [ "entertainment", 13 ]
    animation : { rc : "entertainment", start : 14, count : 21, offset : [ 60, 36 ], delay : 3 }
    sound : [ "theatre", 1 ]
  },

  hippodrome : {
    cost : 2500,
    desirability : { base : -3, range : 3, step : 1 }
    employers : 150
    class : "entertainment"
  },
  
  colloseum : {
    cost : 500,
    desirability : { base : -3, range : 3, step : 1 }
    employers : 25
    class : "entertainment"
    sound : [ "colloseum", 1 ]
  },
  
  gladiatorSchool : {
    cost : 75,
    desirability : { base : -3, range : 3, step : 1 }
    employers : 8
    class : "entertainment"
    image : [ "entertainment", 51 ]
    animation : { rc : "entertainment", start : 52, count : 10, offset : [ 62, 24 ], delay : 2 }
    sound : [ "gladiatorpit", 1 ]
  },
  
  house : {
    cost : 10,
    desirability : { base : -3, range : 3, step : 1 }
    employers : 0
    class : "house"
  }

  lionsNursery : {
    cost : 75,
    desirability : { base : -3, range : 3, step : 1 }
    employers : 8
    class : "entertainment"
    image : [ "entertainment", 62 ]
    animation : { rc : "entertainment", start : 63, count : 18, offset : [ 78, 21 ], delay : 2 }
  },
  
  actorColony : {
    cost : 50,
    desirability : { base : -2, range : 3, step : 1 }
    employers : 5
    image : [ "entertainment", 81 ]
    class : "entertainment"
    animation : { rc : "entertainment", start : 82, count : 9, offset : [ 68, -6 ], delay : 2 }
    sound : [ "artistpit", 1  ]
  },

  chariotSchool : {
    cost : 75,
    desirability : { base : -3, range : 3, step : 1 }
    employers : 10
    class : "entertainment"
    image : [ "entertainment", 91 ]
    animation : { rc : "entertainment", start : 92, count : 10, offset : [ 54, 23 ], delay : 2 }
    sound : [ "charioterspit", 1 ]
  },
  
  plaza : {
    cost : 15,
    desirability : { base : 4, range : 2,	step : -2 }
    employers : 0
    class : "road"
    image : [ "entertainment", 102 ]
    image.ext : {
      type_1 : { size:1, rc : "entertainment", start : 102, count : 6 }
      type_1a: { size:1, rc : "plaza", start : 3139, count : 6 }
      type_2 : { size:2, rc : "entertainment", start : 108, count : 2 }
    }
  },  

  garden : {
    cost : 12,
    desirability : { base :3,	range : 3,	step : -1 }
    employers : 0
    class : "garden"
    image : [ "entertainment", 110 ]
    image.ext : {
      type_1 : { size:1, rc : "entertainment", start : 110, count : 4 }
      type_2 : { size:2, rc : "entertainment", start : 114, count : 3 }
    }
    sound : [ "garden", 1 ]
  },
  
  fort_legionaries : {
    cost : 1000,
    desirability : { base :-20,	range : 10,	step : 2 }
    employers : 16
    class : "military"
    sound : [ "fort", 1 ]
    requestDestroy : true
  },

  missionaryPost : {
    cost : 100
    employers : 20
    class : "education"
  },

  statue_small : {
    cost : 12,
    desirability : { base :3,	range : 3,	step : -1}
    employers : 0
    class : "sight"
    sound : [ "statue", 1 ]
  },
  
  statue_middle : {
    cost : 60,
    desirability : { base :10,	range : 5,	step : -2 }
    employers : 0
    class : "sight"
  },  

  statue_big : {
    cost : 150,
    desirability : { base :15,	range : 5,	step : -3 }
    employers : 0
    class : "sight"
  },  

  fort_javelin : {
    cost : 1000,
    desirability : { base :-20,	range : 10,	step : 2 }
    employers : 16
    class : "military"
    requestDestroy : true
  }, 

  fort_horse : {
    cost : 1000,
    desirability : { base :-20,	range : 10,	step : 2 }
    employers : 16
    class : "military"
    requestDestroy : true
  }, 

  clinic : {
    cost : 30,
    desirability : { base :0,	range : 0,	step : 0 }
    employers : 5
    class : "health"
    image : [ "security", 20 ]
    service : "srvc_doctor"
    sound : [ "clinic", 1 ]
  },  
  
  hospital : {
    cost : 300,
    desirability : { base :2,	range : 2,	step : -1 }
    employers : 30
    class : "health"
    image : [ "security", 44 ]
    service : "srvc_hospital"
  },
  
  baths : {
    cost : 50,
    desirability : { base :4,	range : 4,	step : -1 }
    employers : 10
    class : "health"
    image : [ "security", 32 ]
    animation : { rc : "security", start : 22, count : 10, offset : [ 23, 25 ], delay : 2 }
    service : "srvc_baths"
    sound : [ "baths", 1 ]
  }, 

  barber : {
    cost : 25,
    desirability : { base :2,	range : 2,	step : -1 }
    employers : 2
    class : "health"
    image : [ "security", 19 ]
    service : "srvc_barber"
    sound : [ "barber", 1 ]
  }, 
  
  school : {
    cost : 50,
    desirability : { base :-2,	range : 2,	step : 1 }
    employers : 10
    class : "education"
    image : [ "commerce", 83 ]
    sound : [ "school", 1 ]
  },    
  
  academy : {
    cost : 100,
    desirability : { base :4,	range : 4,	step : -1 }
    employers : 30
    class : "education"
    image : [ "commerce", 85 ]
    sound : [ "academy", 1 ]
  },  
  
  library : {
    cost : 75,
    desirability : { base : 4,	range : 4,	step : -1 }
    employers : 20
    class : "education"
    image : [ "commerce", 84 ]
    sound : [ "library", 1 ]
  },

  prefecture : {
    cost : 30,
    desirability : { base :-2,	range : 2,	step : 1 }
    employers : 6
    image : [ "security", 1 ]
    animation : { rc : "security", start : 2, count : 10, offset : [ 20, 36 ], delay : 4 }
    class : "security"
  },  
 
  triumphal_arch : {
    cost : 0,
    desirability : { base :18,	range : 6,	step : -3 }
    employers : 0
    class : "sight"
  }, 
  
  gatehouse : {
    cost : 100,
    desirability : -4,
    range : 4,
    step : -1,
    employers : 3
    class : "gate"
  },  

  tower : {
    cost : 150,
    desirability : { base :-8,	range : 4,	step : -2 }
    employers : 6
    class : "tower"
  },  
  
  small_ceres_temple : {
    cost : 50,
    desirability : { base :4,	range : 4,	step : -1 }
    image : [ "security", 45 ]
    employers : 2
    class : "religion"
    sound : [ "templeceres", 1 ]
  },  
  
  small_neptune_temple : {
    cost : 50,
    desirability : { base :4,	range : 4,	step : -1 }
    employers : 2
    class : "religion"
    sound : [ "security", 47 ]
    image : [ "smlneptune", 1 ]
    image.offset : [-1, -1]
  }, 
  
  small_mercury_temple : {
    cost : 50,
    desirability : { base : 4, 	range : 4,	step : -1 }
    employers : 2
    class : "religion"
    image : [ "security", 49 ]
    image.offset : [-1, -1]
    sound : [ "templemercury", 1 ]
  },
 
  small_mars_temple : {
    cost : 50,
    desirability : { base : 4,	range : 4,	step : -1 }
    employers : 2
    class : "religion"
    image : [ "security", 51 ]
    image.offset : [-1, -1]
    sound : [ "templemars", 1 ]
  },

  small_venus_temple : {
    cost : 50,
    desirability : { base : 4,	range : 4,	step : -1 }
    employers : 2
    class : "religion"
    image : [ "security", 53 ]
    image.offset : [-1, -1]
    sound : [ "templevenus", 1 ]
  },
 
  big_ceres_temple : {
    cost : 150,
    desirability : { base : 8,	range : 8,	step : -1 }
    employers : 5
    class : "religion"
    image : [ "security", 46 ]
    sound : [ "templeceres", 2 ]
  }, 
  
  big_neptune_temple : {
    cost : 150,
    desirability : { base : 8,	range : 8,	step : -1 }
    employers : 5
    class : "religion"
    image : [ "security", 48 ]
    sound : [ "templeneptune", 2 ]
  }, 
  
  big_mercury_temple : {
    cost : 150,
    desirability : { base : 8, 	range : 8,	step : -1 }
    employers : 5
    class : "religion"
    image : [ "security", 50 ]
    sound : [ "templemercury", 2 ]
  },
  
  big_mars_temple : {
    cost : 150,
    desirability : { base : 8, 	range : 8,	step : -1 }
    employers : 5
    class : "religion"
    image : [ "security", 52 ]
    sound : [ "templemars", 2 ]
  },
  
  big_venus_temple : {
    cost : 150,
    desirability : { base : 8, 	range : 8,	step : -1 }
    employers : 5
    image : [ "security", 54 ]
    class : "religion"
    sound : [ "templevenus", 2 ]
  }, 

  market : {
    cost : 40,
    desirability : { base : -2, range : 6, 	step : 1 }
    employers : 5
    class : "food"
    image : [ "commerce", 1 ]
    sound : [ "market", 1 ]
  }, 

  granery : {
    cost : 100,
    desirability : { base : -4,	range : 4,	step : 1 }
    employers : 6
    class : "food"
    sound : [ "granary", 1 ]
  }, 

  warehouse : {
    cost : 70,
    desirability : { base : -6,	range : 3,	step : 2 }
    employers : 6
    class : "trade"
    sound : [ "warehouse", 1 ]
  }, 

  shipyard : {
    cost : 100,
    desirability : { base : 8,	range : 4,	step : -2 }
    employers : 10
    class : "trade"
    sounde : [ "shipyard", 1 ]
  }, 
  
  dock : {
    cost : 100,
    desirability : { base : 8, 	range : 4,	step : -2 }
    employers : 12
    class : "trade"
    sound : [ "dock", 1 ]
  }, 

  wharf : {
    cost : 60,
    desirability : { base : 8,	range : 4, 	step : -2 }
    employers : 6
    class : "food"
    sound : [ "wharf", 1 ]
  }, 
  
  governorHouse : {
    cost : 150,
    desirability : { base : 12, 	range : 3,	step : -2 }
    employers : 0
    class : "administration"
  }, 
  
  governorVilla : {
    cost : 400,
    desirability : { base : 20, 	range : 4,	step : -4 }
    employers : 0
    class : "administration"
  }, 

  governorPalace : {
    cost : 700,
    desirability : { base : 28, 	range : 7,	step : -4 }
    employers : 0
    class : "administration"
  },
  
  engineering_post : {
    cost : 30,
    desirability : { base : 0, 	range : 0,	step : 0 }
    employers : 5
    image : [ "transport", 56 ]
    class : "engineer"
  },

  low_bridge : {
    cost : 40,
    desirability : { base : 0,	range : 0,	step : 0 }
    employers : 0
    class : "bridge"
    requestDestroy : true
  }, 
  
  high_bridge : {
    cost : 100,
    desirability : { base : 0,	range : 0,	step : 0 }
    employers : 0
    class : "bridge"
    requestDestroy : true
  },

  senate : {
    cost : 250,
    desirability : { base : 8,	range : 4,	step : -2 }
    employers : 20
    class : "administration"
    sound : [ "senate", 1 ]
  }, 
   
  forum : {
    cost : 75,
    desirability : { base : 3,	range : 3,	step : -1 }
    employers : 6
    class : "administration"
    sound : [ "forum", 1 ]
  }, 
    
  native_hut : {
    cost : 50,
    desirability : { base : 0, 	range : 0,	step : 0 }
    employers : 0
    class : "native"
  }, 
  
  native_center : {
    cost : 50,
    desirability : { base : 0,	range : 0,	step : 0 }
    employers : 0
    class : "native"
  }, 
  
  reservoir : {
    cost : 80,
    desirability : { base : -6,	range : 3,	step : -2 }
    employers : 0
    class : "water"
    sound : [ "reservoir", 1 ]
  }, 

  fountain : {
    cost : 15,
	  desirability : { base : 0,	  range : 0,	  step : 0 }
    employers : 4
	  class : "water"
    sound : [ "fountain", 1 ]
  }, 
  
  well : {
    cost : 5,
    desirability : { base : -1,    range : 1,    step : 1 }
    employers : 0,
    pretty : "Well",
    image : [ "utilitya", 1 ]
    image.ext : {
      type_1 : { size:1, rc : "well", start : 1, count : 4 }
    }
    class : "water"
    sound : [ "well", 1 ]
  }, 
 
  native_field : {
    cost : 0,
    desirability : { base : 0,	range : 0,	step : 0 }
    employers : 0
    class : "native"
  },
  
  military_academy : {
    cost : 1000,
    desirability : { base : -3,	range : 3,	step : 1 }
    employers : 20
    class : "military"
  },
  
  barracks : {
    cost : 150,
    desirability : { base : -6,	range : 3,	step : 2 }
    employers : 10
    class : "military"
    sound : [ "barracks", 1 ]
  }, 

  oracle : {
    cost : 20,
    desirability : { base : 8,	range : 7,	step : -1 }
    employers : 6
    class : "religion"
  }, 

  burning_ruins : {
    cost : 0,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 0
    class : "disaster"
    sound : [ "burningruins", 1 ]
  }, 
  
  burned_ruins : {
    cost : 0,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 0
    class : "disaster"
  }, 
  
  collapsed_ruins : {
    cost : 0,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 0
    class : "disaster"
  },
  
  plague_ruins :
  {
    cost : 0,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 0
    class : "disaster"
  },
  
  wheat_farm : 
  {
    cost : 40,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 10
    output : "wheat"
    class : "food"
    productRate : 9.6
    sound : [ "wheatfarm", 1 ]
  }, 

  vegetable_farm :
  {
    cost : 40,
    desirability : { base : -2, 	range : 2,	step : 1 }
    employers : 10
    output : "vegetable"
    productRate : 9.6
    class : "food"
    sound : [ "vegetablefarm", 1 ]
  },
 
  fig_farm : 
  {
    cost : 40,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 10
    output : "fruit"
    productRate : 9.6
    class : "food"
  },
  
  olive_farm : 
  {
    cost : 40,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 10
    output : "olive"
    class : "food"
    productRate : 9.6
    sound : [ "olivesfarm", 1 ]
  },

  vinard :
  {
    cost : 40,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 10
    output : "grape"
    class : "food"
    productRate : 4.8
    sound : [ "grapefarm", 1 ]
  },

  meat_farm : 
  {
    cost : 40,
    desirability : { base : -2,	range : 2,	step : 1 }
    employers : 10
    class : "food"
    output : "meat"
    productRate : 4.8
    sound : [ "meatfarm", 1 ]
  },

  quarry :
  {
    cost : 50,
    desirability : { base : -6,	range : 6,	step : 1 }
    employers : 10
    output : "marble"
    class : "rawmaterial"
    productRate : 2.4
    sound : [ "marblequarry", 1 ]
    image : [ "commerce", 43 ]
  },

  iron_mine :
  {
    cost : 50,
    desirability : { base : -6,	range : 6,	step : 1 }
    employers : 10
    output : "iron"
    class : "rawmaterial"
    productRate : 2.4
    sound : [ "ironmine", 1 ]
  },
  
  lumber_mill :
  {
    cost : 40,
    desirability : { base : -4,	range : 4,	step : 1 }
    employers : 10
    output : "timber"
    class : "rawmaterial"
    productRate : 4.8
    sound : [ "lumbermill", 1 ]
  },

  clay_pit :
  {
    cost : 40,
    desirability : { base : -3,	range : 3,	step : 1 }
    employers : 10
    output : "clay"
    image : [ "commerce", 61 ]
    productRate : 4.8
    class : "rawmaterial"
    animation : { rc : "commerce", start : 62, count : 10, offset : [ 14, 32 ], delay : 3 }
    sound : [ "claypit", 1 ]
  },
  
  wine_workshop :
  {
    cost : 45,
    desirability : { base : -1,	range : 1,	step : 1 }
    employers : 10
    output : "wine"
    productRate : 4.8
    class : "industry"
    sound : [ "winery", 1 ]
  },

  oil_workshop :
  {
    cost : 50,
    desirability : { base : -4,	range : 2,	step : 2 }
    employers : 10
    output : "oil"
    class : "industry"
    productRate : 4.8
    sound : [ "creamery", 1 ]
  },
  
  weapons_workshop :
  {
    cost : 50,
    desirability : { base : -4,	range : 2,	step : 2 }
    employers : 10
    output : "weapon"
    productRate : 2.4
    class : "industry"
    sound : [ "weapons_workshop", 1 ]
  },
  
  furniture_workshop :
  {
    cost : 40,
    desirability : { base : -4,	range : 2,	step : 2 }
    employers : 10
    output : "furniture"
    class : "industry"
    productRate : 2.4
    sound : [ "furniture", 1 ]
    image : [ "commerce", 117 ]
    animation : { rc : "commerce", start : 118, count : 14, delay : 4 }
  },
  
  pottery_workshop :
  {
    cost : 40,
    desirability : { base : -4,	range : 2,	step : 2 }
    employers : 10
    output : "pottery"
    class : "industry"
    productRate : 4.8
    image : [ "commerce", 132 ]
    animation : { rc : "commerce", start : 133, count : 7, delay : 3 }
  }
}
