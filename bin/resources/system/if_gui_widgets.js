/* ***************************** widget class ********************************** */
function UpdateWidgetPrototype(objProto) {
  Object.defineProperty(objProto, 'text', { get : function () {}, set: function (str) { this.setText(_t(str)) }})
  Object.defineProperty(objProto, "geometry", { set: function (rect) { this.setGeometry(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h); } })
  Object.defineProperty(objProto, "font", { set: function (fname) { this.setFont(fname); } })
  Object.defineProperty(objProto, "enabled", { set: function (e) { this.setEnabled(e); } })
  Object.defineProperty(objProto, "textAlign", { set: function (align) { this.setTextAlignment(align.h, align.v); } })
  Object.defineProperty(objProto, "tooltip", { set: function (text) { this.setTooltipText(_t(text)); } })
  Object.defineProperty(objProto, "subElement", { set: function (value) { this.setSubElement(value); } })
  Object.defineProperty(objProto, "name", { set: function (str) { this.setInternalName(str); } });

  Object.defineProperty(objProto, "w", { get: function() { return this.width(); }, set: function(value) { this.setWidth(value)}} );
  Object.defineProperty(objProto, "h", { get: function() { return this.height(); }, set: function(value) { this.setHeight(value)}} );

  Object.defineProperty(objProto, "position", { set: function (point) {
    if (arguments.length==1) {
      this.setPosition(arguments[0].x, arguments[0].y)
    } else if(arguments.length==2) {
      this.setPosition(arguments[0],arguments[1])
    } }
  })
}

UpdateWidgetPrototype(Widget.prototype)
//************************** widget class end ************************************//

//****************************** Label class *************************************//
UpdateWidgetPrototype(Label.prototype)
Object.defineProperty( Label.prototype, "style", { set: function(s) { this.setBackgroundStyle(s) }} )
Object.defineProperty( Label.prototype, 'textOffset',{ get: function () {}, set: function (p) { this.setTextOffset(p.x, p.y) }})
Object.defineProperty( Label.prototype, 'iconOffset',{ get: function () {}, set: function (p) { this.setIconOffset(p.x, p.y) }})
Object.defineProperty( Label.prototype, "multiline", { set: function (en) { this.setWordwrap(en) }} )
Object.defineProperty( Label.prototype, "background", { set: function (picname) { this.setBackgroundPicture(picname) }} )
Object.defineProperty( Label.prototype, "textColor", {set: function(color) { this.setColor(color) }})
Object.defineProperty( Label.prototype, "padding", {set: function(rect) { this.setPadding(rect.left,rect.top,rect.right,rect.bottom) }})

Object.defineProperty( Label.prototype, "icon", { set: function(value) {
    if (!value)
      return;

    if ( typeof value == "string") {
      this.setIcon_str(value)
    }  else if (value instanceof Picture) {
      this.setIcon_pic(value)
    } else if (value.rc && value.index) {
      this.setIcon_rcIndex(value.rc,value.index)
    } else {
      engine.log("Label set picture no case found")
    }
  }
} )
/************************************* label class end ******************************/

//*************************** button class ***************************************/
function Button (parent) { return new PushButton(parent); }
function UpdateButtonPrototype(objProto) {
   UpdateWidgetPrototype(objProto)
   Object.defineProperty(objProto, "callback", { set: function(func) { this.onClickedEx(func) }})
   Object.defineProperty(objProto, "style", {  set: function(sname) { this.setBackgroundStyle(sname) }})
   Object.defineProperty(objProto, 'textOffset',{ get: function () {}, set: function (p) { this.setTextOffset(p) }})
   Object.defineProperty(objProto, "states", {  set: function(st) { this.changeImageSet(st.rc,st.normal,st.hover,st.pressed,st.disabled) }})
   Object.defineProperty(objProto, "iconMask", {  set: function(value) { this.setIconMask(value) }})

   Object.defineProperty(objProto, "icon", { set: function(value) {
        if (!value)
          return;

        if ( typeof value == "string") {
          this.setIcon_str(value)
        }  else if (value instanceof Picture) {
          this.setIcon_pic(value)
        } else if (value.rc && value.index) {
          this.setIcon_rcIndex(value.rc,value.index)
        } else {
          engine.log("Label set picture no case found")
        }
      }
    } )
 }

//*************************** button class end***************************************//

//*************************** Buttons classes begin***************************************//
UpdateButtonPrototype(PushButton.prototype)
UpdateButtonPrototype(TexturedButton.prototype)
UpdateButtonPrototype(ExitButton.prototype)

UpdateButtonPrototype(HelpButton.prototype)
Object.defineProperty(HelpButton.prototype, "uri", { set: function(uri) { this.setProperty("uri", uri) }} )
//*************************** Buttons classes end***************************************//

//*************************** Spinbox class ***************************************//
function Spinbox(parent) { return new SpinBox(parent); }
UpdateWidgetPrototype(SpinBox.prototype)
Object.defineProperty( SpinBox.prototype, "postfix", { set: function(text) { this.setPostfix(_t(text)); }} );
Object.defineProperty( SpinBox.prototype, "value", { set: function(text) { this.setValue(text); }} );
Object.defineProperty( SpinBox.prototype, "callback", { set: function(func) { this.onChangeA(func); }} );
//*************************** Spinbox class end***************************************//

//*************************** KeyValueListbox class ***************************************//
function KeyValueListbox(parent) { return new KeyValueListBox(parent); }
UpdateWidgetPrototype(KeyValueListBox.prototype)
Object.defineProperty( KeyValueListBox.prototype, "itemHeight", {set: function (h) { this.setItemsHeight(h) }} )
//*************************** KeyValueListbox class end***************************************//

//*************************** Listbox class ***************************************//
function Listbox(parent) { return new ListBox(parent); }
UpdateWidgetPrototype(ListBox.prototype)
Object.defineProperty( ListBox.prototype, "style", { set: function(sname) { this.setBackgroundStyle(sname) }} )
Object.defineProperty( ListBox.prototype, "background", { set: function(enabled) { this.setBackgroundVisible(enabled) }} )
Object.defineProperty( ListBox.prototype, "selectedIndex", { set: function(index) { this.setSelected(index) }} )
Object.defineProperty( ListBox.prototype, "selectedWithData", { set: function(obj) { this.setSelectedWithData(obj.name,obj.data) }} )
Object.defineProperty( ListBox.prototype, "count", { get: function () { return this.itemsCount() }} )
Object.defineProperty( ListBox.prototype, "onSelectedCallback", { set: function(func) { this.onIndexSelectedEx(func) }} )
Object.defineProperty( ListBox.prototype, "onDblclickCallback", { set: function(func) { this.onIndexSelectedAgainEx(func) }} )
Object.defineProperty( ListBox.prototype, "scrollbarVisible", { set: function(v) {this.setScrollbarVisible(v)}} )
Object.defineProperty( ListBox.prototype, "itemHeight", {set: function (h) { this.setItemsHeight(h) }} )
Object.defineProperty( ListBox.prototype, "itemFont", {set: function (fname) { this.setFont(h) }} )
Object.defineProperty( ListBox.prototype, "itemTextAlignment", {set: function (obj) { this.setTextAlignment(obj.h,obj.v) }} )

Object.defineProperty( ListBox.prototype, "itemColor", { set: function (obj) {
    if(obj.simple) this.setItemDefaultColor("simple", obj.simple);
    if(obj.hovered) this.setItemDefaultColor("hovered", obj.hovered);
  }} );

ListBox.prototype.setData = function(index,name,data) { this.setItemData(index,name,data); };
ListBox.prototype.margin = function(m) {
  if(m.left) this.setMargin(0,m.left);
  if(m.top) this.setMargin(1,m.top);
  if(m.right) this.setMargin(2,m.right);
  if(m.bottom) this.setMargin(3,m.bottom);
}
//*************************** Listbox class end ***************************************//

//*************************** Dialogbox class ***************************************//
UpdateWidgetPrototype(Dialogbox.prototype)
Object.defineProperty( Dialogbox.prototype, "title", { set: function(str) { this.setTitle( _t(str) ); }} )
Object.defineProperty( Dialogbox.prototype, "buttons", { set: function(flags) { this.setButtons(flags); }} )
Object.defineProperty( Dialogbox.prototype, "onYesCallback", { set:function (func) { this.onYesEx(func); }} )
Object.defineProperty( Dialogbox.prototype, "onNoCallback", { set:function (func) { this.onNoEx(func); }} )
Object.defineProperty( Dialogbox.prototype, "neverValue", { set: function (enabled) { this.setNeverValue(enabled); }} )
Object.defineProperty( Dialogbox.prototype, "onNeverCallback", { set:function (func) { this.onNeverEx(func); }} )
//*************************** Dialogbox class end ***************************************//

//*************************** Editbox class ***************************************//
function Editbox(parent) { return new EditBox(parent); }
UpdateWidgetPrototype(EditBox.prototype)
Object.defineProperty( EditBox.prototype, "textOffset", { set: function(p) { this.setTextOffset(p.x,p.y); }} );
Object.defineProperty( EditBox.prototype, "cursorPos", { set: function(index) { this.moveCursor(index); }} );
Object.defineProperty( EditBox.prototype, "onTextChangeCallback", { set: function(func) { this.onTextChangedEx(func); }} );
Object.defineProperty( EditBox.prototype, "onEnterPressedCallback", { set: function(func) { this.onEnterPressedEx(func); }} );
//*************************** Editbox class end***************************************//

//*************************** Fade class ***************************************//
Object.defineProperty( Fade.prototype, "alpha", { set:function (value) { this.setAlpha(value); }} )
Object.defineProperty( Fade.prototype, "w", { get:function () { return this.width(); }} )
Object.defineProperty( Fade.prototype, "h", { get:function () { return this.height(); }} )
//*************************** Fade class end***************************************//

//*************************** DictionaryText class begin ***************************************//
UpdateWidgetPrototype(DictionaryText.prototype)
//*************************** DictionaryText class end ***************************************//

//*************************** PosisitionAnimator class begin ***************************************//
Object.defineProperty( PositionAnimator.prototype, "destination", { set: function (point) { this.setDestination(point.x,point.y); }} )
Object.defineProperty( PositionAnimator.prototype, "speed", { set:function (point) { this.setSpeed(point.x,point.y); }} )
Object.defineProperty( PositionAnimator.prototype, "removeParent", { set:function (value) { this.setFlag("removeParent", value ); }} )
//*************************** Posisit class end ***************************************//

//*************************** Image class begin ***************************************//
UpdateWidgetPrototype(Image.prototype)
Object.defineProperty( Image.prototype, "mode", {set: function (value) { this.setMode(value); }} )
Object.defineProperty( Image.prototype, "picture", {
  set: function (value) {
    if (!value)
      return;

    if ( typeof value == "string") {
      this.setPicture_str(value)
    }  else if (value instanceof Picture) {
      this.setPicture_pic(value)
    } else if (value.rc && value.index) {
      this.setPicture_rcIndex(value.rc,value.index)
    } else {
      engine.log("Image set picture no case found")
    }
  }
} )
//*************************** Image class end ***************************************//

//*************************** FileSelector class begin ***************************************//
function FileSelector(parent) { return new FileListBox(parent); }
UpdateWidgetPrototype(FileListBox.prototype)
Object.defineProperty( FileListBox.prototype, "background", {set: function (enabled) { this.setBackgroundVisible(enabled); }} )
Object.defineProperty( FileListBox.prototype, "selectedIndex", {set:function (index) { this.setSelected(index); }} )
Object.defineProperty( FileListBox.prototype, "itemHeight", {set: function (h) { this.setItemsHeight(h); }} )
Object.defineProperty( FileListBox.prototype, "itemsFont", {set: function (fname) { this.setItemsFont(fname); }} )
Object.defineProperty( FileListBox.prototype, "selectedWithData", {set: function (obj) { this.setSelectedWithData(obj.name,obj.data); }} )
Object.defineProperty( FileListBox.prototype, "count", {set: function () { return this.itemsCount(); }} )
Object.defineProperty( FileListBox.prototype, "onSelectedCallback", { set: function(func) { this.onIndexSelectedEx(func); }} )
Object.defineProperty( FileListBox.prototype, "items", {set: function (paths) { this.addLines(paths); }} )

Object.defineProperty( FileListBox.prototype, "itemColor", { set: function (obj) {
    if(obj.simple) this.setItemDefaultColor("simple", obj.simple);
    if(obj.hovered) this.setItemDefaultColor("hovered", obj.hovered);
  }} );
//*************************** FileSelector class end ***************************************//

//*************************** SaveGameDialog class begin ***************************************//
Object.defineProperty( SaveGame.prototype, "directory", {set: function (path) { this.setDirectory(path); }} )
Object.defineProperty( SaveGame.prototype, "filter", {set: function (ext) { this.setFilter(ext); }} )
Object.defineProperty( SaveGame.prototype, "callback", {set: function (func) { this.onFileSelectedEx(func); }} )
//*************************** SaveGameDialog class end ***************************************//

//*************************** FileDialog class begin ***************************************//
function FileDialog(parent,advanced) {
    if (advanced)
      this.widget = new LoadGame(parent);
    else
      this.widget = new LoadFile(parent);
}

Object.defineProperty( FileDialog.prototype, "title", { set: function (str) { this.widget.setTitle( _t(str) ); }} )
Object.defineProperty( FileDialog.prototype, "showExtensions", { set: function (en) { this.widget.setShowExtensions(en); }} )
Object.defineProperty( FileDialog.prototype, "text", { set: function (str) { this.widget.setText( _t(str) ); }} )
Object.defineProperty( FileDialog.prototype, "directory", { set: function (path) { this.widget.setDirectory(path); }} )
Object.defineProperty( FileDialog.prototype, "filter", {set: function (str) { this.widget.setFilter(str); }} )
Object.defineProperty( FileDialog.prototype, "mayDeleteFiles", { set: function (en) { this.widget.setMayDelete(en); }} )
Object.defineProperty( FileDialog.prototype, "callback", {set: function (func) { this.widget.onSelectFileEx(func); }} )
//*************************** FileDialog class end ***************************************//

//*************************** ContextMenu class begin ***************************************//
ContextMenu.prototype.addItemWithCallback = function(path,caption,func) {
    var item = this.addItem(path,_t(caption));
    item.callback = func;
    return item;
}
UpdateWidgetPrototype(ContextMenu.prototype)
//*************************** ContextMenu class end ***************************************//

//*************************** ContextMenuItem class begin ***************************************//
UpdateWidgetPrototype(ContextMenuItem.prototype)
Object.defineProperty( ContextMenuItem.prototype, "callback", {set: function (func) { this.onClickedA(func); }} )
Object.defineProperty( ContextMenuItem.prototype, "toggleCallback", {set: function (func) { this.onCheckedEx(func); }} )
Object.defineProperty( ContextMenuItem.prototype, "autoChecking", {set: function(en) { this.setAutoChecking(en)}} )

ContextMenuItem.prototype.addItemWithCheckingCallback = function(caption,func) {
    var item = this.addSubmenuItem(_t(caption));
    item.toggleCallback = func;
    return item;
}

ContextMenuItem.prototype.addItemWithCallback = function(caption,func) {
    var item = this.addSubmenuItem(_t(caption));
    item.callback = func;
    return item;
}
//*************************** ContextMenuItem class end ***************************************//

//*************************** Groupbox class begin ***************************************//
function Groupbox(parent) { return new GroupBox(parent) }
UpdateWidgetPrototype(GroupBox.prototype)

GroupBox.prototype.addLabel = function(rx,ry,rw,rh) {
  var label = new Label(this);
  label.geometry = { x:rx, y:ry, w:rw, h:rh };
  return label;
}

GroupBox.prototype.addButton = function(rx,ry,rw,rh) {
  var btn = new Button(this);
  btn.geometry = { x:rx, y:ry, w:rw, h:rh };
  return btn;
}
//*************************** Groupbox class end ***************************************//
