var g_config = {
    level : {
        res_menu:0,
        res_restart:2,
    },

    audio : {
      theme : 2,
      speech : 4
    },

    climate : {
      central : 0,
      northen : 1,
      desert  : 2
    },

    gift : {
      modest : 1,
      generous : 2,
      lavish : 3
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

/********************** namespace lobby **********************************/
var lobby = {}
lobby.ui = {}
lobby.ui.newgame = {}
lobby.ui.loadgame = {}
lobby.ui.mainmenu = {}
lobby.ui.options = {}
