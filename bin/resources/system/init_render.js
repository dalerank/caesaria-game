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

var g_render = new Render()
