//*************************** Picture class begin ***************************************//
Object.defineProperty( Picture.prototype, "valid", { get: function()  { return this.isValid() }} )

Picture.prototype.fallback = function() {
  if (this.valid)
    return this;

  if (arguments.length == 2)
    this.load_rcIndex(arguments[0], arguments[1])
  else if(arguments.length == 1)
    this.load_str(arguments[0])

  return this
}
//*************************** Picture class end ***************************************//
