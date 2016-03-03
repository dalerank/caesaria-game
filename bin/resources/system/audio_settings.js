function OnShowAudioDialog()
{
  var wnd = g_ui.addWindow(0);
  wnd.geometry = { x:0, y:0, w:350, h:225 };
  wnd.moveToCenter();
  wnd.closeAfterKey( {escape:true, rmb:true} );
  wnd.title  = "##game_sound_options##";
  
  var saveGameValue = engine.getOption("soundVolume");
  var saveAmbValue = engine.getOption("ambientVolume");
  var saveMusicValue = engine.getOption("musicVolume");
  var wasChanged = false;
  
  var spnGameVolume = wnd.addSpinbox(28, 60, wnd.width-28*2, 24);
  spnGameVolume.text = "##gmsndwnd_game_volume##";
  spnGameVolume.postfix = " %";
	spnGameVolume.value = saveGameValue;
  spnGameVolume.font = "FONT_2";
  spnGameVolume.textAlign = { h:"center", v:"center" };
  spnGameVolume.callback = function(value) { 
																																			engine.setOption("soundVolume",value); 
																																			engine.setVolume(3,value);
																																			wasChanged = true;
																																	  }
     
  var spnAmbientVolume = wnd.addSpinbox(28, 96, wnd.width-28*2, 24);
  spnAmbientVolume.text = "##gmsndwnd_ambient_sound##";
  spnAmbientVolume.postfix = " %";
	spnAmbientVolume.value = saveAmbValue;
  spnAmbientVolume.font = "FONT_2";
  spnAmbientVolume.textAlign = { h:"center", v:"center" };
  spnAmbientVolume.callback = function(value) { 
		      																																engine.setOption("ambientVolume",value); 
					     																														engine.setVolume(1,value);
																																					wasChanged = true;
								      																								  }
     
  var spnThemeVolume = wnd.addSpinbox(28, 132, wnd.width-28*2, 24);
  spnThemeVolume.text = "##gmsndwnd_ambient_sound##";
  spnThemeVolume.postfix = " %";
	spnThemeVolume.value = saveMusicValue;
  spnThemeVolume.font = "FONT_2";
  spnThemeVolume.textAlign = { h:"center", v:"center" };
  spnThemeVolume.callback = function(value) { 
		      																														 engine.setOption("musicVolume",value); 
					     																												 engine.setVolume(2,value);
																																			 wasChanged = false;
								      																							 }	
																																		 
  var btnOk = wnd.addButton(wnd.width*0.25, wnd.height-60, wnd.width*0.5,  22);
  btnOk.style = "smallGrayBorderLine";
  btnOk.text = "##ok##";
  btnOk.callback = function() { wnd.deleteLater();	};   
	
	var btnCancel = wnd.addButton(wnd.width*0.25, wnd.height-35, wnd.width*0.5,  22);
  btnCancel.style = "smallGrayBorderLine";
  btnCancel.text = "##cancel##";
  btnCancel.callback = function() {
																										if (wasChanged)
																										{
																											engine.setOption("soundVolume",saveGameValue); 
																											engine.setVolume(3,saveGameValue);
																											engine.setOption("ambientVolume",saveAmbValue); 
																											engine.setVolume(1,saveAmbValue);
																											engine.setOption("musicVolume",saveMusicValue); 
																											engine.setVolume(2,saveMusicValue);
																										}
																										wnd.deleteLater();	
																								 };   
													
   wnd.moveToCenter();													
   wnd.setModal();																									
}