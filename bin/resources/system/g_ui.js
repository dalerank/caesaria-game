function Ui() {
}

Object.defineProperty( Ui.prototype, "cursor", { get: function () { return g_session.getCursorPos() }} );

Ui.prototype.addWindow = function(rx,ry,rw,rh) {
    var window = new Window(0);
    window.geometry = { x:rx, y:ry, w:rx+rw, h:ry+rh };
    return window;
  }

Ui.prototype.addSimpleWindow = function(rx,ry,rw,rh) {
    var window = new Window(0);
    window.geometry = { x:rx, y:ry, w:rx+rw, h:ry+rh };
    window.addExitButton(window.w-36, window.h-36);
    window.moveToCenter();
    window.closeAfterKey({escape:true,rmb:true});
    return window;
  }

Ui.prototype.addInformationDialog = function(title, text) {
    var dialog = new Dialogbox(0);
    dialog.title = _t(title);
    dialog.text = _t(text);
    dialog.buttons = 1;
    return dialog;
  }

Ui.prototype.addFileDialog = function(dir,ext,adv) {
    var dialog = new FileDialog(0,adv);
    dialog.directory = dir;
    if (ext.length > 0)
      dialog.filter = ext;
    return dialog;
  }

Ui.prototype.addFade = function(value) {
    var fade = new Fade(0);
    fade.alpha = value;
    return fade;
  }

Ui.prototype.addSaveGameDialog = function(dir, ext) {
    var dialog = new SaveGame(0);
    dialog.directory = dir;
    dialog.filter = ext;
    return dialog;
  }

Ui.prototype.addConfirmationDialog = function(title, text) {
    var dialog = new Dialogbox(0);
    dialog.title = _t(title);
    dialog.text = _t(text);
    dialog.buttons = 3;
    return dialog;
  }

Ui.prototype.addImage = function() {
    var image = new Image(0);
    if (arguments.length==3)
    {
      image.mode = "image"
      image.position = {x:arguments[0],y:arguments[1]};
      image.picture = arguments[2];
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

Ui.prototype.addLabel = function(rx,ry,rw,rh) {
    var label = new Label(0);
    label.geometry = { x:rx, y:ry, w:rw, h:rh };
    return label;
  }

Ui.prototype.elog = function(a) { engine.log(a); }

var g_ui = new Ui()
