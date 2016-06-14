Object.defineProperty(Request.prototype, "isDeleted", { get: function () { return this.getProperty("isDeleted"); }});
Object.defineProperty(Request.prototype, "qty", { get: function () { return this.getProperty("qty"); }});
Object.defineProperty(Request.prototype, "description", { get: function () { return this.getProperty("description"); }});
Object.defineProperty(Request.prototype, "isReady", { get: function () { return this.action(g_city, "isReady"); }});
Object.defineProperty(Request.prototype, "gtype", { get: function () { return this.getProperty("gtype"); }});

Request.prototype.exec = function() {
  this.action(g_city, "exec");
}

Object.defineProperty(Request.prototype, "finishDate", {
  get: function () {
    var dt = this.getProperty("finishDate");
    var ret = new DateTime();
    ret.setDate(dt.year, dt.month, dt.day);
    return ret;
  }
})
