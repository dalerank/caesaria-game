/**************************************** DateTime class begin **************************************************************/
DateTime.prototype.format = function(metric) {
  if (metric === g_config.metric.roman)
    return g_session.formatDate(this, true);

  return g_session.formatDate(this, false);
}
/**************************************** DateTime class end **************************************************************/
