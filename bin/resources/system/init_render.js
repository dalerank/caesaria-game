function Render() {
}

Render.prototype.picture = function() {
  var pic = new Picture()
  if (typeof arguments[0] == "string")
    pic.load_str(arguments[0])
  else
    pic.load_rcIndex(arguments[0], arguments[1])
  return pic
}

var g_render = new Render()
