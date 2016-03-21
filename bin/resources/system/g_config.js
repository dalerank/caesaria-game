var g_config = {
  level: {
    res_menu: 0,
    res_restart: 2
  },

  audio: {
    theme: 2,
    speech: 4
  },

  climate: {
    central: 0,
    northen: 1,
    desert: 2
  },

  gift: {
    modest: 1,
    generous: 2,
    lavish: 3
  },

  metric: {
    none: 0,
    modern: 1,
    roman: 2
  },

  advisor: {
    none: 0,
    employers: 1,
    military: 2,
    empire: 3,
    ratings: 4,
    trading: 5,
    population: 6,
    health: 7,
    education: 8,
    entertainment: 9,
    religion: 10,
    finance: 11,
    main: 12,
    unknown: 13
  },

  layer : {
    simple : 0,
    water : 1,
    fire : 2,
    damage : 3,
    desirability : 4,
    entertainments : 5,
    entertainment : 6,
    theater : 7,
    amphitheater : 8,
    colloseum : 9,
    hippodrome : 10,
    health : 11,
    healthAll : 12,
    doctor : 13,
    hospital : 14,
    barber : 15,
    baths : 16,
    food : 17,
    religion : 18,
    risks : 19, crime : 20, aborigen : 21, troubles : 22,
    educations : 23, education : 24, school : 25, library : 26, academy : 28,
    commerce : 29, tax : 30, market : 31, sentiment : 32, unemployed : 33, comturnover : 34,
    build : 35, destroyd : 36, constructor : 37,
    products : 38
  },

  saves : {
    ext : ".oc3save",
    fast : "_fastsave",
    auto : "_autosave",
  }
}

var game = {}
game.ui = {}
game.ui.dialogs = {}
game.sound = {}

/********************** namespace level **********************************/
var sim = {}

sim.climate = {}

sim.ui = {}
sim.ui.topmenu = {}
sim.ui.topmenu.help = {}
sim.ui.topmenu.file = {}
sim.ui.topmenu.debug = {}
sim.ui.topmenu.advisors = {}
sim.ui.topmenu.options = {}
sim.ui.advisors = {}
sim.ui.dialogs = {}
sim.hotkeys = {}
sim.fastsave = {}
sim.autosave = {}
sim.timescale = {}

/********************** namespace lobby **********************************/
var lobby = {}
lobby.ui = {}
lobby.ui.newgame = {}
lobby.ui.loadgame = {}
lobby.ui.mainmenu = {}
lobby.ui.options = {}
lobby.hotkeys = {}
