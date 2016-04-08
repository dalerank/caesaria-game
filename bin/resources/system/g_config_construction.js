g_config.construction = {
  well : {
    cost : 5,
    desirability : { base : -1,    range : 1,    step : 1 },
    employers : 0,
    pretty : "Well",
    images : {
      type_1 : { size:1, rc : "well", start : 1, count : 1 }
    },
    class : "water",
    sound : [ "well", 1 ],
  },

  reservoir : {
    cost : 80,
    desirability : { base : -6,	range : 3,	step : -2 },
    employers : 0,
    mayBurn : false,
    class : "water",
    fullOffset : [ 0, 0 ],
    images : {
      t1 : { size:3, rc : "utilitya", start : 34, count : 1 }
    },
    animation : { rc : "resvanim", start : 1, count : 29, offset : [ 0, 211 ], delay : 4 },
    sound : [ "reservoir", 1 ],
  },

  fountain : {
    cost : 15,
    desirability : { base : 0,	  range : 0,	  step : 0 },
    employers : 4,
    class : "water",
    sound : [ "fountain", 1 ]
  },

  aqueduct : {
    cost : 8,
    desirability : { base: -2, range : 2, step : 1 },
    mayBurn : false,
    class : "water",
    sound : [ "aqueduct", 1 ]
  },
}
