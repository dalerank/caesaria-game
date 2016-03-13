var g_wasChangesShow = false;
lobby.ui.buttons = [];

lobby.ui.showChanges = function(force)
{
  engine.log( "JS:lobby.ui.showChanges" );

  if( force )
  {
    engine.setOption( "showLastChanges", true);
    engine.setOption( "lastChangesNumber", 0 );
    g_wasChangesShow = false;
  }

  var lastChangesNumber = g_session.lastChangesNum();
  var currentChangesNumber = engine.getOption( "lastChangesNumber" );
  engine.setOption( "lastChangesNumber", lastChangesNumber );

  if( lastChangesNumber !== currentChangesNumber )
    engine.setOption( "showLastChanges", true);

  if( !g_wasChangesShow && engine.getOption( "showLastChanges" ) > 0 )
  {
    g_wasChangesShow = true;

    var wnd = g_ui.addWindow(0,0,400,500);
    wnd.model = ":/changes/" + lastChangesNumber + ".changes";

    var btn = wnd.addButton( 13, wnd.h-36, 300, 24 );
    btn.text = "##hide_this_msg##";
    btn.font = "FONT_2";
    btn.style = "whiteBorderUp";
    btn.callback = function() {
                                var showChanges = !engine.getOption("showLastChanges");
                                engine.setOption( "showLastChanges", showChanges );
                                btn.text = showChanges ?  "##show_this_msg##" : "##hide_this_msg##";
                              };

    wnd.addExitButton(wnd.w-36,wnd.h-36);

    wnd.moveToCenter();
    wnd.mayMove = false;
    wnd.setModal();
  }
}

lobby.ui.newgame.startCareer = function ()
{
  lobby.ui.changePlayerName(true,function() { g_session.setMode(0); } );
}

lobby.ui.newgame.showpage = function()
{
  engine.log("JS:lobby.ui.newgame.showpage");

  lobby.ui.clear();

  lobby.ui.addButton( "##mainmenu_startcareer##", function() { lobby.ui.newgame.startCareer()})
  lobby.ui.addButton( "##mainmenu_randommap##",   function() { lobby.ui.newgame.playRandom()})
  lobby.ui.addButton( "##mainmenu_constructor##", function() { lobby.ui.newgame.openConstructor()})
  lobby.ui.addButton( "##cancel##",               function() { lobby.ui.mainmenu.showpage() })
}

lobby.ui.options.showLanguageSettings = function()
{
  var wnd = g_ui.addWindow(0);
  wnd.title = "##mainmenu_language##"
  wnd.geometry = { x:0, y:0, w:512, h:384 };
  wnd.moveToCenter();
  wnd.closeAfterKey( {escape:true, rmb:true} );

  var listbox = wnd.addListbox(20, 50, wnd.w-40, wnd.h-90);
  listbox.setTextAlignment("center", "center");
  listbox.background = true;
  listbox.onSelectedCallback = function(index) {
                                                 game.setLanguage(g_config.languages[index]);
                                               }

  for (var i in g_config.languages)
  {
    var conf = g_config.languages;
    var index = listbox.addLine(conf[i].lang);
    listbox.setData(index, "lang", conf[i].ext);
  }

  var currentLang = engine.getOption("language");
  listbox.selectedWithData = { name:"lang", data:currentLang };

  var btn = wnd.addButton(20, wnd.h - 40, wnd.w-40, 24);
  btn.text = "##continue##";
  btn.callback = function () { g_session.setMode(6); }
}

lobby.ui.showLogs = function()
{
  var logfile = g_session.logfile;
  if (!logfile.exist())
    g_ui.addInformationDialog( "", "Can't find logfile" );
  else
    g_session.openUrl(logfile.str);
}

lobby.ui.changePlayerName = function(force,continueCallback)
{
  var playerName = engine.getOption("playerName");
  if (playerName === null || playerName.length === 0)
    playerName = "##new_governor##";

  if (playerName.length === 0 || force)
  {
    var wnd = g_ui.addWindow(0);
    wnd.geometry = { x:0, y:0, w:380, h:128 }
    wnd.closeAfterKey( {escape:true} );
    wnd.mayMove = false;
    wnd.title = "##enter_your_name##";

    var exitFunc =  function() {
                                  wnd.deleteLater();
                                  if(continueCallback)
                                      continueCallback();
                               }

    var editbox = wnd.addEditbox(32, 50, 320, 32);
    editbox.textAlign = { h:"upperLeft", v:"center" };
    editbox.font = "FONT_2_WHITE";
    editbox.textOffset = { x:20, y:0 };
    editbox.tooltip = "##enter_dinasty_name_here##";
    editbox.text = playerName;
    editbox.cursorPos = playerName.length;
    editbox.onEnterCallback = exitFunc;
    editbox.onTextChangeCallback = function(text) { engine.setOption( "playerName", text ); }

    var lbNext = wnd.addLabel(180, 90, 100, 20);
    lbNext.text = "##plname_continue##";
    lbNext.font = "FONT_2";
    lbNext.textAlign = { h:"lowerRight", v:"center" };

    var btnContinue = wnd.addTexturedButton(300, 85, 27, 27);
    btnContinue.states = { rc:"paneling", normal:179, hover:180, pressed:181, disabled:179 };
    btnContinue.tooltip = "##plname_start_new_game##";
    btnContinue.callback = exitFunc;

    var lbExitHelp = wnd.addLabel(18, 90, 200, 20);
    lbExitHelp.text = "##press_escape_to_exit##";
    lbExitHelp.font = "FONT_1";

    wnd.moveToCenter();
    wnd.setModal();

    editbox.setFocus();
  }
}

lobby.ui.addButton = function(caption,callback)
{
  var buttonSize = { w:200, h:25 };
  var btnFont = "FONT_2";
  var offsetY = 40;

  var button = new Button(0);
  button.geometry = { x:0, y:0, w:buttonSize.w, h:buttonSize.h };
  //button.setBackgroundStyle( style );
  button.font = btnFont;
  button.text = caption;
  if (callback)
  {
      button.callback = callback;
  }
  else
  {
      engine.log("callback is null")
  }

  lobby.ui.buttons.push(button);

  var resolution = g_session.resolution;
  var offset = { x:(resolution.w - buttonSize.w) / 2, y:(resolution.h - offsetY * lobby.ui.buttons.length) / 2 };
  for (var i in lobby.ui.buttons)
  {
     var tmp = lobby.ui.buttons[i];
     tmp.position = offset;
     offset.y += offsetY;
  }

    return button;
}

lobby.ui.clear = function()
{
  for (var i in lobby.ui.buttons)
    lobby.ui.buttons[i].deleteLater();

  lobby.ui.buttons = [];
}

lobby.ui.continuePlay = function()
{
  engine.log("JS:lobby.ui.continuePlay");

  var lastGame = engine.getOption("lastGame");
  if(lastGame !== undefined && lastGame.length>0)
  {
      g_session.setOption("nextFile", lastGame);
      g_session.setMode(4);
  }
}

lobby.ui.newgame.playRandom = function()
{
  engine.log("JS:lobby.ui.newgame.playRandom");

  g_session.setOption("nextFile", ":/missions/random.mission" );
  g_session.setMode(3);
}

lobby.ui.newgame.openConstructor = function()
{
    engine.log("JS:lobby.ui.newgame.openConstructor");

    var fileDialog = g_ui.addFileDialog(":/maps/", ".map,.sav,.omap", false);
    fileDialog.mayDeleteFiles = false;
    fileDialog.title = "##mainmenu_loadmap##";
    fileDialog.text = "##start_this_map##";
    fileDialog.callback = function(path) {
                        g_session.setOption("nextFile",path);
                        g_session.setMode(2);
                     }
}

lobby.ui.loadgame.loadsave = function()
{
    engine.log("JS:lobby.ui.loadgame.loadsave");

    var fileDialog = g_ui.addFileDialog(g_session.savedir.str, "", true);
    fileDialog.mayDeleteFiles = true;
    fileDialog.title = "##mainmenu_loadgame##";
    fileDialog.text = "##load_this_game##";
    fileDialog.callback = function(path) {
                       g_session.setOption("nextFile",path);
                       g_session.setMode(4);
                     }
}

lobby.ui.loadgame.loadmap = function()
{
    engine.log("JS:lobby.ui.loadgame.loadmap");

    var fileDialog = g_ui.addFileDialog(":/maps/", ".map,.sav,.omap");
    fileDialog.mayDeleteFiles = false;
    fileDialog.title = "##mainmenu_loadmap##";
    fileDialog.text = "##start_this_map##";
    fileDialog.callback = function(path) {
                           g_session.setOption("nextFile",path);
                           g_session.setMode(1);
                     }
}

lobby.ui.loadgame.showpage = function()
{
    engine.log("JS:lobby.ui.loadgame.showpage");

    lobby.ui.clear();

    lobby.ui.addButton("##mainmenu_playmission##", OnShowMissionSelectDialog );
    lobby.ui.addButton("##mainmenu_loadgame##",    function() { lobby.ui.loadgame.loadsave() })
    lobby.ui.addButton("##mainmenu_loadmap##",     function() { lobby.ui.loadgame.loadmap() })
    lobby.ui.addButton("##cancel##",               function() { lobby.ui.mainmenu.showpage()})
}

lobby.ui.options.showpage = function()
{
    engine.log("JS:lobby.ui.options.showpage");

    lobby.ui.clear();

    lobby.ui.addButton( "##mainmenu_language##", function() { lobby.ui.options.showLanguageSettings()} )
    lobby.ui.addButton( "##mainmenu_video##",    function() { sim.ui.topmenu.options.showVideoOptions()} )
    lobby.ui.addButton( "##mainmenu_sound##",    function() { game.ui.dialogs.showAudioOptions() })
    lobby.ui.addButton( "##mainmenu_package##",  OnShowPackageOptions );
    lobby.ui.addButton( "##mainmenu_plname##",   function() { lobby.ui.changePlayerName(true)} )
    lobby.ui.addButton( "##mainmenu_showlog##",  function() { lobby.ui.showLogs() } )
    lobby.ui.addButton( "##mainmenu_changes##",  function() { lobby.ui.showChanges(true)} )
    lobby.ui.addButton( "##cancel##",            function() { lobby.ui.mainmenu.showpage()} )
}

lobby.ui.options.showdlc = function()
{
    engine.log("JS:lobby.ui.options.showdlc");
    lobby.ui.clear();

    var path = g_session.getPath(":/dlc");
    if (!path.exist())
    {
       g_ui.addInformationDialog( "##no_dlc_found_title##", "##no_dlc_found_text##" );
       lobby.ui.mainmenu.showpage();
       return;
    }

    var folders = g_session.getFolders(path.str,true);
    for (var i in folders)
    {
      var fullpath = g_session.getPath(folders[i]);
      var folderName = fullpath.baseName;
      var locText = "##mainmenu_dlc_" + folderName + "##";

      lobby.ui.addButton(locText, function() { g_session.showDlcViewer(fullpath.str);  }	);
    }
}

lobby.ui.mainmenu.showpage = function()
{
  lobby.ui.clear();

  var lastGame = engine.getOption("lastGame");
  if(lastGame && lastGame.length > 0)
      lobby.ui.addButton("##mainmenu_continueplay##", function() {lobby.ui.continuePlay()})

  lobby.ui.addButton("##mainmenu_newgame##", function() { lobby.ui.newgame.showpage() } )
  lobby.ui.addButton("##mainmenu_load##",    function() { lobby.ui.loadgame.showpage() } )
  lobby.ui.addButton("##mainmenu_options##", function() { lobby.ui.options.showpage() } )
  lobby.ui.addButton("##mainmenu_credits##", function() { lobby.ui.showGameCredits() } )

  var dlc = g_session.getPath(":/dlc");
  if (dlc.exist())
     lobby.ui.addButton("##mainmenu_mcmxcviii##", function() { lobby.ui.options.showdlc() } )

  lobby.ui.addButton("##mainmenu_quit##",         function() { g_session.setMode(5); } );
}

function OnLobbyStart()
{
  engine.log( "JS:OnLobbyStart" );
  lobby.ui.clear();

  var screen = g_session.resolution;

  var btnHomePage = new TexturedButton(0);
  var startx = screen.w/2 - 128 - 32;
  btnHomePage.geometry = { x:startx, y:screen.h - 70, w:64, h:64 };
  btnHomePage.states = { rc:"logo_rdt", normal:1, hover:2, pressed:2, disabled:2 };
  btnHomePage.callback = function() { g_session.openUrl( "http://www.caesaria.net" ); }
  btnHomePage.tooltip = "Open website in browser";

  var btnSteamPage = new TexturedButton(0);
  btnSteamPage.geometry = { x:startx + 64, y:screen.h - 70, w:64, h:64 };
  btnSteamPage.states = { rc:"steam_icon", normal:1, hover:2, pressed:2, disabled:2 };
  btnSteamPage.callback = function() { g_session.openUrl("http://store.steampowered.com/app/327640"); }
  btnSteamPage.tooltip = "Open steam store page in browser";

  var btnIrcPage = new TexturedButton(0);
  btnIrcPage.geometry = { x:startx + 64*2, y:screen.h - 70, w:64, h:64 };
  btnIrcPage.states = { rc:"irc_rdt", normal:1, hover:2, pressed:2, disabled:2 };
  btnIrcPage.callback = function() { g_session.openUrl( "http://webchat.quakenet.org/?channels=caesaria" ); }
  btnIrcPage.tooltip = "Open developer's chat in browser";

  var btnReportBugPage = new TexturedButton(0);
  btnReportBugPage.geometry = { x:startx + 64*3, y:screen.h - 70, w:64, h:64 };
  btnReportBugPage.states = { rc:"breport", normal:1, hover:2, pressed:2, disabled:2 };
  btnReportBugPage.callback = function() { g_session.openUrl( "https://bitbucket.org/dalerank/caesaria/issues/new" ); }
  btnReportBugPage.tooltip = "Report about bug";

  var btnTranslationPage = new TexturedButton(0);
  btnTranslationPage.geometry = { x:startx + 64*4, y:screen.h - 70 , w:64, h:64 };
  btnTranslationPage.states = { rc:"translation", normal:1, hover:2, pressed:2, disabled:2 };
  btnTranslationPage.callback = function() { g_session.openUrl( "https://docs.google.com/spreadsheets/d/1vpV9B6GLUX5G5z3ftucBFl7pXr-I0QvHPI9vW6K4xlY" ); }
  btnTranslationPage.tooltip = "Help with translation!";

  g_session.playAudio( "main_menu", 50, g_config.audio.theme );

  lobby.ui.mainmenu.showpage();
  lobby.ui.showChanges(false);
}

lobby.ui.showGameCredits = function()
{
  g_session.playAudio( "combat_long", 50, g_config.audio.theme );

  var fade = g_ui.addFade(0xA0);
  fade.addCloseCode(0x1B); //escape
  fade.addCloseCode(0x4);//right mouse button

  var credits = g_session.credits;
  for( var i=0; i < credits.length; i++ )
  {
    var lb = new Label(fade);
    lb.geometry = { x:0, y:fade.h + i * 15, w:fade.w, h:15};
    lb.text = credits[i];
    lb.textAlign = { h:"center", v:"center" };
    lb.font = "FONT_2_WHITE";
    lb.subElement = true;

    var animator = new PositionAnimator(lb);
    animator.removeParent = true;
    animator.destination = {x:0, y:-20};
    animator.speed = {x:0, y:-0.5};
  }

  var btnExit = new Button(fade);
  btnExit.geometry = {x:fade.w - 150, y:fade.h-34, w:140, h:24};
  btnExit.text = "##close##";
  btnExit.callback = function() {
                                  fade.deleteLater();
                                  g_session.playAudio( "main_menu", 50, g_config.audio.theme );
                                }
}
