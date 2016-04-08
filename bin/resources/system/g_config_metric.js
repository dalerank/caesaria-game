g_config.metric = {
    none: 0,
    modern: 1,
    roman: 2
}

Object.defineProperty( g_config.metric, "mode", {
  get: function() { return g_session.getAdvflag("metric") },
  set: function(value) { g_seesion.setAdvflag("metric",value) },
  configurable: true
})

Object.defineProperty( g_config.metric, "modeName", {
  get: function() {
    switch( this.mode )
    {
      case g_config.metric.native: return "##quantity##";
      case g_config.metric.metric: return "##kilogram##";
      case g_config.metric.roman:  return "##modius##";
      default: return "unknown";
    }
  },
  configurable: true
})

Object.defineProperty( g_config.metric, "modeShort", {
  get: function() {
    switch( this.mode )
    {
      case g_config.metric.native: return "##qty##";
      case g_config.metric.metric: return "##kg##";
      case g_config.metric.roman:  return "##md##";
      default: return "unknown";
    }
  },
  configurable: true
})

g_config.metric.convQty = function(qty)
{
  switch( this.mode )
  {
  case g_config.metric.native: return qty;
  case g_config.metric.metric: return qty / 2;
  case g_config.metric.roman: return qty / 7;
  default: return qty;
  }
}
