/*function Window(x1,y1,x2,y2)
{
  this.cptr = engine.addWidget(0,"Window",x1,y1,x2,y2);
    engine.log( this.cptr );
}

Window.prototype.setText = function(text)
{
    engine.log( this.cptr );
    //engine.widgetSetText( this.cptr, tex );
};
//Window.prototype.setTitle = function(title) { engine.windowSetTitle( this.cptr, title ); };

var window = new Window( 0, 0, 540, 240 );
//window.setTitle( "##mission_win##" );
window.setText( "##test_text##" );

engine.log( "" );*/

var window = new Window( 0 );
window.setGeometry( 0, 0, 540, 240 );
window.setText( "##test_text##" );

engine.log( "asdasasdasd" );

