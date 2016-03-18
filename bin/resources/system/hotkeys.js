var _hotkeyExecFunction;

function OnExecHotkey(name)
{
  _hotkeyExecFunction(name)
}

sim.hotkeys = {}
lobby.hotkeys = {}

sim.hotkeys.actions = [
  { name : "advisor.employers", sequence : "1", action : function() { g_session.setOption("advisor",g_config.advisor.employers) }},
  { name : "advisor.military", sequence : "2", action : function() { g_session.setOption("advisor",g_config.advisor.military) }},
  { name : "advisor.empire", sequence : "3", action : function() { g_session.setOption("advisor",g_config.advisor.empire)}},
  { name : "advisor.ratings", sequence : "4", action : function() { g_session.setOption("advisor",g_config.advisor.ratings) }},
  { name : "advisor.trading", sequence : "5", action : function() { g_session.setOption("advisor",g_config.advisor.trading)}},
  { name : "advisor.population", sequence : "6", action : function() { g_session.setOption("advisor",g_config.advisor.population) }},
  { name : "advisor.health", sequence : "7", action : function() {  g_session.setOption("advisor",g_config.advisor.health)}},
  { name : "advisor.education", sequence : "8", action : function() { g_session.setOption("advisor",g_config.advisor.education)}},
  { name : "advisor.entertainment", sequence : "9", action : function() { g_session.setOption("advisor",g_config.advisor.entertainment)}},
  { name : "advisor.religion", sequence : "0", action : function() { g_session.setOption("advisor",g_config.advisor.religion)}},
  //{ name : "advisor.finance", sequence : "tilda", action : function() { g_session.setOption("advisor",g_config.advisor.finance)}},
  { name : "advisor.main", sequence : "tilda", action : function() { g_session.setOption("advisor",g_config.advisor.main)}},

  { name : "layer.heath", sequence : "h", action : function() { g_session.setOption("layer",g_config.layer.health)}}, //health
  { name : "layer.fire", sequence : "f", action : function() { g_session.setOption("layer",g_config.layer.fire)}}, //fire
  { name : "layer.damage", sequence : "d", action : function() { g_session.setOption("layer",g_config.layer.damage)}}, //damage
  { name : "layer.crime", sequence : "c", action : function() { g_session.setOption("layer",g_config.layer.crime)}}, //crime
  { name : "layer.troubles", sequence : "t", action : function() { g_session.setOption("layer",g_config.layer.troubles)}}, //troubles
  { name : "layer.water", sequence : "w", action : function() { g_session.setOption("layer",g_config.layer.water)}}, //water
  { name : "layer.desirability", sequence : "g", action : function() { g_session.setOption("layer",g_config.layer.desirability)}}, //desirability
  { name : "fastsave.create", sequence : "F5", action : function() { sim.fastsave.create() }},
  { name : "fastsave.load", sequence : "F9", action : function() { sim.fastsave.load() }},

  { name : "gamespeed.increase", sequence : "plus", action : function() { sim.timescale.increase(10) }},
  { name : "gamespeed.increase2", sequence : "sum", action : function() { sim.timescale.increase(10) }},
  { name : "gamespeed.increase3", sequence : "=", action : function() { sim.timescale.increase(10) }},

  { name : "gamespeed.decrease", sequence : "minus", action : function() { sim.timescale.decrease(10) }},
  { name : "gamespeed.decrease2", sequence : "subtract", action : function() { sim.timescale.decrease(10) }},
  { name : "layer.toggle", sequence : "space", action : function() { sim.hotkeys.toggleLayer() }}
]

sim.hotkeys.toggleLayer = function() {
  var lastLayer = g_session.getOption("lastLayer");
  var layer = g_config.layer.simple;
  if (layer == g_config.layer.simple)
     layer = lastLayer
  g_session.setOption("layer",layer);
}

sim.hotkeys.init = function()
{
  engine.log("Initialize hotkeys")

  g_session.clearHotkeys()
  for (var i in sim.hotkeys.actions)
  {
    var item = sim.hotkeys.actions[i]
    g_session.setHotkey( item.name, item.sequence)
  }

  _hotkeyExecFunction = function(name) {
    for (var i in sim.hotkeys.actions)
    {
      var item = sim.hotkeys.actions[i]
      if (item.name == name)
        item.action();
    }
  }
}

lobby.hotkeys.init = function()
{
  g_session.clearHotkeys()
  _hotkeyExecFunction = function(name) {
    engine.log("no resolver for loggy hotkey " + name)
  }
}
