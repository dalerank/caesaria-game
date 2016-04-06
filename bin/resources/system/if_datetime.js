/**************************************** DateTime class begin **************************************************************/
DateTime.prototype.format = function(metric) {
  if (metric === g_config.metric.roman)
    return g_session.formatDate(this, true);

  return g_session.formatDate(this, false);
}

Object.defineProperty( DateTime.prototype, "romanStr", { get: function () { g_session.formatDate(this, true); }} );
/**************************************** DateTime class end **************************************************************/
