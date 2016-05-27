/**************************************** DateTime class begin **************************************************************/
DateTime.prototype.formateDate = function(drawDays, roman) {
  var month = _format("##month_{0}_short##", this.month());
  var age = _format( "##age_{0}##", this.age());
  var year = "";
  var text = "";
  var dayStr = "";
  if (drawDays) {
    if (!roman) dayStr = "" + time.day();
    else dayStr += Utils.toRoman(time.day());
  }

  var yearNum = Math.abs(time.year());
  if (!roman) year += yearNum;
  else year += Utils.toRoman(yearNum);

  text = fmt::format( "{} {} {} {}", dayStr, _( month ), year, _( age ) );
  return text;
}

DateTime.prototype.format = function (metric) {
    if (metric === g_config.metric.roman)
        return g_session.formatDate(this, true);

    return g_session.formatDate(this, false);
}

Object.defineProperty(DateTime.prototype, "romanStr", {
    get: function () {
        g_session.formatDate(this, true);
    }
});
/**************************************** DateTime class end **************************************************************/
