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

function ExitButton(parent) {
  this.widget = new _ExitButton(parent);
}

ExitButton.prototype = {
  set position (point) { this.widget.setPosition(point.x,point.y); }
}

function Window(parent) {
  this.widget = new _Window(parent);
} 

Window.prototype = {
  set title (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set mayMove (may) { this.widget.setWindowFlag("fdraggable",may); },
  set onClose (func) { this.widget.onCloseEx(func); },
  set model (path) { this.widget.setupUI(path); },
  get width () { return this.widget.width(); },
  get height () { return this.widget.height(); },

  moveToCenter : function() { this.widget.moveToCenter(); },
  setModal : function() { this.widget.setModal(); },
  deleteLater : function() { this.widget.deleteLater(); },
  addLabel : function(rx,ry,rw,rh) {
    var label = new Label( this.widget ); 
    label.geometry = { x:rx, y:ry, w:rw, h:rh };
    return label; 
  },
	addListbox : function(rx,ry,rw,rh) {
    var listbox = new Listbox(this.widget);
    listbox.geometry = { x:rx, y:ry, w:rw, h:rh };
    return listbox;
  },
  addButton : function(rx,ry,rw,rh) {
    var button = new Button(this.widget);
    button.geometry = { x:rx, y:ry, w:rw, h:rh };
    return button;
  },
  addExitButton : function(rx,ry) {
    var btn = new ExitButton(this.widget);
    btn.position = { x:rx, y:ry }
    return btn;
  }
}

function Button(parent) {
  this.widget = new _PushButton(parent);
}

Button.prototype = {
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set font (fname) { this.widget.setFont(fname); },
  set callback (func) { this.widget.onClickedEx(func); },
  set style (sname) { this.widget.setBackgroundStyle(sname); },    

  deleteLater : function() { this.widget.deleteLater(); }
	setFocus : function() { this.widget.setFocus(); }
}

function Listbox(parent) {
	this.widget = new _ListBox(parent);	
}

Listbox.prototype = {
	set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
	set style (sname) { this.widget.setBackgroundStyle(sname); },    
	
	setItemData : function(index,data) { this.widget.setItemData(index,data); }
	deleteLater : function() { this.widget.deleteLater(); }
}

function Dialogbox(parent) {
	this.widget = new _Dialogbox(parent);
}

Dialogbox.prototype = {
	set title (str) { this.widget.setTitle( engine.translate(str) ); },
	set text  (str) { this.widget.setText( engine.translate(str) ); },
  set buttons (flags) { this.widget.setButtons(flags); },
  set onYesCallback (func) { this.widget.setYesCallback(func); },
  set neverValue (enabled) { this.widget.setNeverValue(enabled); },
  set onNeverCallback (func) { this.widget.onNeverEx(func); }
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
    var dialog = new Dialogbox(0);
    dialog.title = title;
    dialog.text = text;
    dialog.buttons = 1;
    return dialog; 
  },

  elog : function(a) { engine.log(a); }
}
  
