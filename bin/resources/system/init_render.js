function Render() {
}

Render.prototype.picture = function() {
  var pic = new Picture()

  if (arguments.length == 1) {
    pic.load_str(arguments[0])
  }
  else if (arguments.length == 2) {
    pic.load_rcIndex(arguments[0], arguments[1])
  }

  return pic
}

Object.defineProperty(Picture.prototype, "w", { get: function () { return this.width(); } });
Object.defineProperty(Picture.prototype, "h", { get: function () { return this.height(); } });

var g_render = new Render();
