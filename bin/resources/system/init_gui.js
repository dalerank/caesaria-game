function Label(parent) {
  this.widget = new _Label(parent);
}

Label.prototype = {
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set font (fname) { this.widget.setFont(fname); },
  set textAlign (align) { this.widget.setTextAlignment(align.h,align.h); },

  deleteLater : function() { this.widget.deleteLater(); }
}

function Window(parent) {
  this.widget = new _Window(parent);
}

Window.prototype = {
  set title (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  get width () { return this.widget.width(); },

  deleteLater : function() { this.widget.deleteLater(); },
  addLabel : function(rx,ry,rw,rh) {
    var label = new Label( this.widget );
    label.geometry = { x:rx, y:ry, w:rw, h:rh };
    return label;
  },
  addButton : function(rx,ry,rw,rh) {
    var button = new Button( this.widget );
    button.geometry = { x:rx, y:ry, w:rw, h:rh };
    return button;
  }
}

function Button(parent) {
  this.widget = new _PushButton(parent);
}

Button.prototype = {
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set font (fname) { this.widget.setFont(fname); },
  set callback (func) { this.widget.setCallback(func); },
  set style (sname) { this.widget.setBackgroundStyle(sname); },    

  deleteLater : function() { this.widget.deleteLater(); }
}

function ConfirmationDialog(parent) {
  this.widget = new _Dialogbox(parent);
}

function Ui() {
}

Ui.prototype = {
  addWindow : function(rx,ry,rw,rh) {
    var window = new Window(0);
    window.geometry = { x:rx, y:ry, w:rx+rw, h:ry+rh };
    return window;
  },

  addInformationDialog : function(title, text) {
    var dialog = new ConfirmationDialog(0);
    dialog.setTitle(title);
    dialog.setText(text);
    return dialog;
  },

  elog : function(a) { engine.log(a); }
}

