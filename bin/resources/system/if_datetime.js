/**************************************** DateTime class begin **************************************************************/
DateTime.prototype.formatDate = function(drawDays, roman) {
  var month = _format("##month_{0}_short##", this.month);
  var age = _format( "##age_{0}##", this.age);
  var year = "";
  var text = "";
  var dayStr = "";
  if (drawDays) {
    if (!roman) dayStr += time.day();
    else dayStr += Utils.toRoman(time.day());
  }

  var yearNum = Math.abs(this.year);
  if (!roman) year += yearNum;
  else year += Utils.toRoman(yearNum);

  text = _format( "{0} {1} {2} {3}", dayStr, _t(month), year, _t(age) );
  return text;
}

DateTime.prototype.format = function (metric) {
    if (metric === g_config.metric.roman)
        return this.formatDate(false, true);

    return this.formatDate(false, false);
}

Object.defineProperty(DateTime.prototype, "year", { get: function () { return this.get(0); }});
Object.defineProperty(DateTime.prototype, "month", { get: function () { return this.get(1); }});
Object.defineProperty(DateTime.prototype, "day", { get: function () { return this.get(2); }});
Object.defineProperty(DateTime.prototype, "age", { get: function () { return this.year > 0 ? "ad" : "bc"; }});

Object.defineProperty(DateTime.prototype, "romanStr", {
    get: function () {
        return this.formatDate(false, true);
    }
});
/**************************************** DateTime class end **************************************************************/
