var haveChanges = false;
var fullscreen = false;

function OnShowVideoSettings()
{
  engine.log( "JS:OnShowVideoSettings" );

  var g_ui = new Ui();
  var g_session = new Session();

  haveChanges = false;
  fullscreen = engine.getOption( "fullscreen" );

  var wnd = g_ui.addWindow(0,0,320,290);
  wnd.title = "##screen_settings##";

  var btnSwitchMode = wnd.addButton( 25, 45, wnd.width-50, 20 );
  btnSwitchMode.text =  fullscreen ? "##fullscreen_on##" : "##fullscreen_off##";
  btnSwitchMode.style = "smallGrayBorderLine";
  btnSwitchMode.callback = function() {
                                        fullscreen = !fullscreen;
                                        engine.setOption( "fullscreen", fullscreen );
                                        haveChanges = true;
                                        btnSwitchMode.text =  fullscreen ? "##fullscreen_on##" : "##fullscreen_off##";
                                        g_session.saveSettings();
				      }
     
  var lbxModes = wnd.addListbox( 25, 68, wnd.width-50, 160 );
  lbxModes.setTextAlignment( "center", "center" );
  lbxModes.onSelectedCallback = function(index) {
						var size = lbxModes.getItemData(index, "mode");
						g_session.setResolution( size.w, size.h );
						g_session.saveSettings();
					}

  for( var i=0; i < g_session.videoModesCount; i++ )
  {
    var mode = g_session.getVideoMode(i);
    var index = lbxModes.addLine( mode.w+"x"+mode.h );
    lbxModes.setData( index, "mode", (mode.w << 16) + mode.h );
  }

  var resolution = g_session.resolution;

  var btnOk = wnd.addButton( 25, 237, wnd.width-50, 20 );
  btnOk.style = "smallGrayBorderLine";
  btnOk.text = "##ok##";
  btnOk.callback = function() {
				if( haveChanges )
  				  g_ui.addInformationDialog( "##pls_note##", "##need_restart_for_apply_changes##" );
	        		wnd.deleteLater();		
			}
  btnOk.setFocus();

  wnd.moveToCenter();
  wnd.mayMove = false;
  wnd.setModal();  
}
