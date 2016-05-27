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

Utils.romanDigits = [ {value:1000, numeral:"M"},
                      {value:900,  numeral:"CM"},
                      {value:500,  numeral:"D"},
                      {value:400,  numeral:"CD"},
                      {value:100,  numeral:"C"},
                      {value:90,   numeral:"XC"},
                      {value:50,   numeral:"L"},
                      {value:40,   numeral:"XL"},
                      {value:10,   numeral:"X"},
                      {value:9,    numeral:"IX"},
                      {value:5,    numeral:"V"},
                      {value:4,    numeral:"IV"},
                      {value:1,    numeral:"I"},
                      {value:0,    numeral:NULL} ]; // end marker


Utils.toRoman = function(value) {
  var result;
  for (var current in this.romanDigits) {
    while (value >= current.value) {
      result += current.numeral;
      value  -= current.value;
    }
  }
  
  return result;
}

Utils.clone = function(obj) {
  if (obj == null)
    return null;

  var ret = new Object();
  for (var i in obj) {
    ret[i] = obj[i];
  }

  return ret;
}
