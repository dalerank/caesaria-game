function Label(parent) {
  this.widget = new _Label(parent);
}

Label.prototype = {
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set font (fname) { this.widget.setFont(fname); },
  set textAlign (align) { this.widget.setTextAlignment(align.h,align.v); },
  set tooltip (text) { this.widget.setTooltipText(text); },
  set textColor (color) { this.widget.setColor(color); },
  set subElement (value) { this.widget.setSubElement(value); },

  deleteLater : function() { this.widget.deleteLater(); }
};

function Spinbox(parent) {
  this.widget = new _SpinBox(parent);
}

Spinbox.prototype = {
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set font (fname) { this.widget.setFont(fname); },
  set tooltip (text) { this.widget.setTooltipText(engine.translate(text)); },
  set textColor (color) { this.widget.setColor(color); },
  set subElement (value) { this.widget.setSubElement(value); },
  set postfix (text) { this.widget.setPostfix(text); },
  set callback (func) { this.widget.onChangeA(func); },
  set textAlign (align) { this.widget.setTextAlignment(align.h,align.v); },

  deleteLater : function() { this.widget.deleteLater(); }
};

function DictionaryText(parent) {
  this.widget = new _DictionaryText(parent);
}

DictionaryText.prototype = {
  set text (str) { this.widget.setText(engine.translate(str)); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  get width () { return this.widget.width(); },
  get height () { return this.widget.height(); },

  deleteLater : function() { this.widget.deleteLater(); }
};

function Fade(parent) {
  this.widget = new _Fade(parent);
}

Fade.prototype = {
  set alpha (value) { this.widget.setAlpha(value); },
  get width () { return this.widget.width(); },
  get height () { return this.widget.height(); },

  addCloseCode : function(code) { this.widget.addCloseCode(code); },
  deleteLater : function() { this.widget.deleteLater(); }
};

function ExitButton(parent) {
  this.widget = new _ExitButton(parent);
}

ExitButton.prototype = {
  set position (point) { this.widget.setPosition(point.x,point.y); }
};

function Editbox(parent) {
  this.widget = new _EditBox(parent);
}

Editbox.prototype = {
  set position (point) { this.widget.setPosition(point.x,point.y); },
  get width () { return this.widget.width(); },
  get height () { return this.widget.height(); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set font (fname) { this.widget.setFont(fname); },
  set textOffset(p) { this.widget.setTextOffset(p.x,p.y); },
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set cursorPos (index) { this.widget.moveCursor(index); },
  set onTextChangeCallback (func) { this.widget.onTextChangedEx(func); },
  set onEnterPressedCallback (func) { this.widget.onEnterPressedEx(func); },

  moveToCenter : function() { this.widget.moveToCenter(); },
  deleteLater : function() { this.widget.deletLater(); },
    setFocus : function() { this.widget.setFocus(); }
}

function TexturedButton(parent) {
  this.widget = new _TexturedButton(parent);
}

TexturedButton.prototype = {
  set position (point) { this.widget.setPosition(point.x,point.y); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set states (st) { this.widget.changeImageSet(st.rc,st.normal,st.hover,st.pressed,st.disabled); },
  set tooltip (text) { this.widget.setTooltipText(text); },
  set callback (func) { this.widget.onClickedEx(func); },
};

function PositionAnimator(parent) {
  this.widget = new _PositionAnimator(parent);
}

PositionAnimator.prototype = {
  set destination (point) { this.widget.setDestination(point.x,point.y); },
  set speed (point) { this.widget.setSpeed(point.x,point.y); },
  set removeParent (value) { this.widget.setFlag("removeParent", value );}
};

function Image(parent) {
  this.widget = new _Image(parent);
}

Image.prototype = {
  set position (point) { this.widget.setPosition(point.x,point.y); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set tooltip (text) { this.widget.setTooltipText(text); },
  set picture (name) { this.widget.setPicture(name); }
};

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
    addCloseCode : function(code) { this.widget.addCloseCode(code); },

  closeAfterKey : function(obj) {
      if(obj.escape)
        this.addCloseCode(0x1B);
      if(obj.rmb)
        this.addCloseCode(0x4);
  },
  addLabel : function(rx,ry,rw,rh) {
    var label = new Label(this.widget);
    label.geometry = { x:rx, y:ry, w:rw, h:rh };
    return label;
  },
    addSpinbox : function(rx,ry,rw,rh) {
    var spinbox = new Spinbox(this.widget);
    spinbox.geometry = { x:rx, y:ry, w:rw, h:rh };
    return spinbox;
  },
  addFileSelector : function(rx,ry,rw,rh) {
    var selector = new FileSelector(this.widget);
    selector.geometry = { x:rx, y:ry, w:rw, h:rh };
    return selector;
  },
  addDictionaryText : function(rx,ry,rw,rh) {
    var dtext = new DictionaryText(this.widget);
    dtext.geometry = { x:rx, y:ry, w:rw, h:rh };
    return dtext;
  },
  addListbox : function(rx,ry,rw,rh) {
    var listbox = new Listbox(this.widget);
    listbox.geometry = { x:rx, y:ry, w:rw, h:rh };
    return listbox;
  },
  addEditbox : function(rx,ry,rw,rh) {
    var edit = new Editbox(this.widget);
    edit.geometry = { x:rx, y:ry, w:rw, h:rh };
    return edit;
  },
  addImage : function(rx,ry,picname) {
    var image = new Image(this.widget);
    image.picture = picname;
    image.position = {x:rx,y:ry};
    return image;
  },
  addTexturedButton : function(rx,ry,rw,rh) {
    var button = new TexturedButton(this.widget);
    button.geometry = { x:rx, y:ry, w:rw, h:rh };
    return button;
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
};

function Button(parent) {
  this.widget = new _PushButton(parent);
}

Button.prototype = {
  set text (str) { this.widget.setText( engine.translate(str) ); },
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set position (point) { this.widget.setPosition(point.x,point.y); },
  set font (fname) { this.widget.setFont(fname); },
  set callback (func) { this.widget.onClickedEx(func); },
  set style (sname) { this.widget.setBackgroundStyle(sname); },
  set tooltip (text) { this.widget.setTooltipText(text); },
  set textAlign (obj) { this.widget.setTextAlignment(obj.h,obj.v); },

  deleteLater : function() { this.widget.deleteLater(); },
  setFocus : function() { this.widget.setFocus(); }
};

function Listbox(parent) {
    this.widget = new _ListBox(parent);
}

Listbox.prototype = {
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set style (sname) { this.widget.setBackgroundStyle(sname); },
  set background (enabled) { this.widget.setDrawBackground(enabled); },
  set selectedIndex (index) { this.widget.setSelected(index); },
  set selectedWithData (obj) { this.widget.setSelectedWithData(obj.name,obj.data); },
  set font (fname) { this.widget.setFont(fname); },
  get itemsCount () { return this.widget.itemsCount(); },

  set itemColor (obj)
  {
    if(obj.simple) this.widget.setItemDefaultColor("simple", setItemDefaultColor);
    if(obj.hovered) this.widget.setItemDefaultColor("hovered", setItemDefaultColor);
  },

  set onSelectedCallback (func) { this.widget.onIndexSelectedEx(func); },

  addLine : function(text) { return this.widget.addLine(text); },
  findItem : function(text) { return this.widget.findItem(text); },
  setData : function(index,name,data) { this.widget.setItemData(index,name,data); },
  deleteLater : function() { this.widget.deleteLater(); },
  setTextAlignment : function(h,v) { this.widget.setTextAlignment(h,v); }
};

function FileSelector(parent) {
    this.widget = new _FileListBox(parent);
}

FileSelector.prototype = {
  set geometry (rect) { this.widget.setGeometry(rect.x,rect.y,rect.x+rect.w,rect.y+rect.h); },
  set style (sname) { this.widget.setBackgroundStyle(sname); },
  set background (enabled) { this.widget.setDrawBackground(enabled); },
  set selectedIndex (index) { this.widget.setSelected(index); },
  set itemHeight (h) { this.widget.setItemHeight(h); },
  set items (paths) { this.widget.addItems(paths); },
  set selectedWithData (obj) { this.widget.setSelectedWithData(obj.name,obj.data); },
  get itemsCount () { return this.widget.itemsCount(); },

  set onSelectedCallback (func) { this.widget.onIndexSelectedEx(func); },

  addLine : function(text) { return this.widget.addLine(text); },
  findItem : function(text) { return this.widget.findItem(text); },
  setData : function(index,name,data) { this.widget.setItemData(index,name,data); },
  deleteLater : function() { this.widget.deleteLater(); },
  setTextAlignment : function(h,v) { this.widget.setTextAlignment(h,v); }
};

function Dialogbox(parent) {
    this.widget = new _Dialogbox(parent);
}

Dialogbox.prototype = {
  set title (str) { this.widget.setTitle( engine.translate(str) ); },
  set text  (str) { this.widget.setText( engine.translate(str) ); },
  set buttons (flags) { this.widget.setButtons(flags); },
  set onYesCallback (func) { this.widget.onYesEx(func); },
  set onNoCallback (func) { this.widget.onNoEx(func); },
  set neverValue (enabled) { this.widget.setNeverValue(enabled); },
  set onNeverCallback (func) { this.widget.onNeverEx(func); }
};

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


  addFade : function(value) {
    var fade = new Fade(0);
    fade.alpha = value;
    return fade;
  },

  addConfirmationDialog : function(title, text) {
    var dialog = new Dialogbox(0);
    dialog.title = title;
    dialog.text = text;
    dialog.buttons = 3;
    return dialog;
  },

  elog : function(a) { engine.log(a); }
};

var g_ui = new Ui();
