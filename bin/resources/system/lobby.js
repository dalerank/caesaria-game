var g_wasChangesShow = false;

function OnShowChanges(force)
{
  engine.log( "JS:OnShowChanges" );

  var g_session = new Session();
  if( force )
  {
    engine.setOption( "showLastChanges", true);
    engine.setOption( "lastChangesNumber", 0 );
    g_wasChangesShow = false;
  }

  var lastChangesNumber = g_session.lastChangesNum;
  var currentChangesNumber = engine.getOption( "lastChangesNumber" );
  engine.setOption( "lastChangesNumber", lastChangesNumber );

  if( lastChangesNumber !== currentChangesNumber )
    engine.setOption( "showLastChanges", true);

  if( !g_wasChangesShow && engine.getOption( "showLastChanges" ) > 0 )
  {
    g_wasChangesShow = true;

    var g_ui = new Ui();

    var wnd = g_ui.addWindow(0,0,400,500);
    wnd.model = ":/changes/" + lastChangesNumber + ".changes";

    var btn = wnd.addButton( 13, wnd.height-36, 300, 24 );
    btn.text = "##hide_this_msg##";
    btn.font = "FONT_2";
    btn.style = "whiteBorderUp";
    btn.callback = function() {
                                var showChanges = !engine.getOption("showLastChanges");
                                engine.setOption( "showLastChanges", showChanges );
                                btn.text = showChanges ? "##hide_this_msg##" : "##show_this_msg##";
                              }

    wnd.addExitButton(wnd.width-36,wnd.height-36);

    wnd.moveToCenter();
    wnd.mayMove = false;
    wnd.setModal();
  }
}

function OnLobbyStart()
{
  engine.log( "JS:OnLobbyStart" );

  var g_session = new Session();
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
  btnSteamPage.tooltip = "Open steamstore page in browser";

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
}
