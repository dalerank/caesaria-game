//*************************** Window class begin ***************************************//
Object.defineProperty( Window.prototype, "title", { set: function (str) { this.setText( _t(str) ); }} );
Object.defineProperty( Window.prototype, "geometry", { set: function (rect) { this.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); }} );
Object.defineProperty( Window.prototype, "mayMove", { set: function(may) { this.setWindowFlag("fdraggable",may); }} );
Object.defineProperty( Window.prototype, "onCloseCallback", { set: function (func) { this.onCloseEx(func); }} );
Object.defineProperty( Window.prototype, "model", { set: function (path) { this.setupUI(path); }} );
Object.defineProperty( Window.prototype, "internalName", { set: function (name) { this.setInternalName(name); }} );
Object.defineProperty( Window.prototype, "titleFont", { set: function (fname) { this.setFont(fname); }} );
Object.defineProperty( Window.prototype, "w", { get: function () { return this.width(); }} );
Object.defineProperty( Window.prototype, "h", { get: function() { return this.height(); }} );

Object.defineProperty( Window.prototype, "pauseGame", { set: function(en) {
  if (en)
  {
    var locker = new GameAutoPauseWidget(this);
    locker.activate();
  }
}})

Window.prototype.closeAfterKey = function(obj) {
  if(obj.escape)
    this.addCloseCode(0x1B);
  if(obj.rmb)
    this.addCloseCode(0x4);
}

Window.prototype.addLabel = function(rx,ry,rw,rh) {
  var label = new Label(this);
  label.geometry = { x:rx, y:ry, w:rw, h:rh };
  return label;
}

Window.prototype.addGroupbox = function(rx,ry,rw,rh) {
  var gbox = new Groupbox(this);
  gbox.geometry = { x:rx, y:ry, w:rw, h:rh };
  return gbox;
}

Window.prototype.addSpinbox = function(rx,ry,rw,rh) {
  var spinbox = new Spinbox(this);
  spinbox.geometry = { x:rx, y:ry, w:rw, h:rh };
  return spinbox;
}

Window.prototype.addFileSelector = function(rx,ry,rw,rh) {
  var selector = new FileSelector(this);
  selector.geometry = { x:rx, y:ry, w:rw, h:rh };
  return selector;
}

Window.prototype.addDictionaryText = function(rx,ry,rw,rh) {
  var dtext = new DictionaryText(this);
  dtext.geometry = { x:rx, y:ry, w:rw, h:rh };
  return dtext;
}

Window.prototype.addListbox = function(rx,ry,rw,rh) {
  var listbox = new Listbox(this);
  listbox.geometry = { x:rx, y:ry, w:rw, h:rh };
  return listbox;
}

Window.prototype.addKeyValueListbox = function(rx,ry,rw,rh) {
  var kvlistbox = new KeyValueListbox(this);
  kvlistbox.geometry = { x:rx, y:ry, w:rw, h:rh };
  return kvlistbox;
}

Window.prototype.addEditbox = function(rx,ry,rw,rh) {
  var edit = new Editbox(this);
  edit.geometry = { x:rx, y:ry, w:rw, h:rh };
  return edit;
}

Window.prototype.addImage = function() {
  var image = new Image(this);
  if (arguments.length==3)
  {
    image.position = {x:arguments[0],y:arguments[1]};
    image.picture = arguments[3];
  }
  else if(arguments.length==5)
  {
    var rx = arguments[0]
    var ry = arguments[1]
    var rw = arguments[2]
    var rh = arguments[3]
    image.geometry = {x:rx, y:ry, w:rw, h: rh}
    image.picture = arguments[4]
  }
  return image;
}

Window.prototype.addTexturedButton = function(rx,ry,rw,rh) {
  var button = new TexturedButton(this);
  button.geometry = { x:rx, y:ry, w:rw, h:rh };
  return button;
}

Window.prototype.addButton = function(rx,ry,rw,rh) {
  var button = new Button(this)
  button.geometry = { x:rx, y:ry, w:rw, h:rh }
  return button
}

Window.prototype.addHelpButton = function(rx,ry) {
  var btn = new HelpButton(this)
  btn.setPosition(rx, ry)
  return btn
}

Window.prototype.addExitButton = function(rx,ry) {
  var btn = new ExitButton(this)
  btn.position = { x:rx, y:ry }
  return btn;
}

Window.prototype.addSoundMuter = function(volume) {
  var muter = new SoundMuter(this);
  muter.setVolume(volume)
  return muter
}

Window.prototype.addSoundEmitter = function(sample, volume, type) {
  var emitter = new SoundEmitter(this)
  emitter.assign(sample, volume, type)
  return emitter
}

//*************************** Window class end ***************************************//
