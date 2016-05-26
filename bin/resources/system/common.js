var Utils = {};

function _t(text) { return engine.translate(text) }

function _u(text) {
  if (text[0] == "#")
    return text;

  return "##"+text+"##"
}

function _ut(text) { return _t(_u(text)) }

var _format = function() {
    var formatted = arguments[0]
    for (var arg in arguments) {
                if(arg==0)
                    continue
        formatted = formatted.replace("{" + (arg-1) + "}", arguments[arg])
    }
    return formatted
};

Utils.clone = function(obj) {
  if (obj == null)
    return null;

  var ret = new Object();
  for (var i in obj) {
    ret[i] = obj[i];
  }

  return ret;
}
