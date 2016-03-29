sim.fastsave.filename = function()
{
  var filename = g_session.savedir;
  filename.add( g_session.city.name()
                +g_config.saves.fast
                +g_config.saves.ext );
  return filename.str;
}

sim.fastsave.create = function() {
  g_session.save(sim.fastsave.filename())
}

sim.fastsave.load = function() {
  g_session.setOption("nextFile", sim.fastsave.filename())
}

sim.autosave.rotate = 0
sim.autosave.filename = function(index)
{
  sim.autosave.rotate = (sim.autosave.rotate + 1) % 3;
  var filename = g_session.savedir;
  filename.add( g_session.city.name()
                +g_config.saves.auto
                +sim.autosave.rotate
                +g_config.saves.ext );
  return filename.str;
}

sim.autosave.create = function() {
  g_session.save(sim.autosave.filename());
}

function OnSimulationCreateAutosave() {
  sim.autosave.create()
}
