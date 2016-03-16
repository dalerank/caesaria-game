function SimulationTimescale() {

}

Object.defineProperty( SimulationTimescale.prototype, "value", {
    get: function () {
      return g_session.getAdvflag("gameSpeed")
    },

    set: function(value) {
      g_session.setAdvflag("gameSpeed",value)
      value = g_session.getAdvflag("gameSpeed")
      g_session.addWarningMessage( _t("##current_game_speed_is##") + value + "%" );
    }
} )

SimulationTimescale.prototype.increase = function(delta) {
  this.value = this.value + delta
}

SimulationTimescale.prototype.decrease = function(delta) {
  this.value = this.value - delta
}

sim.timescale = new SimulationTimescale()
