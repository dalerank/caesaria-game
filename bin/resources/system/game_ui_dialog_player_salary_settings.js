game.ui.dialogs.playerSalarySettings = {}

function ShowPlayerSalarySettings() {
  game.ui.dialogs.playerSalarySettings.show()
}

game.ui.dialogs.playerSalarySettings.show = function()
{
  var wint = g_session.winConditions();
  if (wint.reignYears > 0)
  {
    addInformationDialog( "", "##disabled_draw_salary_for_free_reign##" )
    return;
  }

  var w = g_ui.addWindow(0, 0, 510, 400)
  w.title = "##governor_salary_title##"
  w.font = "FONT_5"
  w.closeAfterKey( {escape:true, rmb:true} );

  var playerSalary = g_session.player.salary();
  var wantSalary = playerSalary

  var lbxTitles = w.addListbox(16, 50, w.w-32, w.h-100)
  lbxTitles.setTextAlignment( "center", "center" )
  lbxTitles.itemFont = "FONT_2_WHITE"
  lbxTitles.background = true;
  lbxTitles.itemHeight = 22

  for (var i in g_config.ranks)
  {
    var rank = g_config.ranks[i]
    var str = _t( "##" + rank.name + "_salary##" )
    var index = lbxTitles.addLine( str + "   " + rank.salary )
    if (rank.salary === playerSalary)
      lbxTitles.selectedIndex = i
  }

  lbxTitles.onSelectedCallback = function(index) { wantSalary = g_config.ranks[index].salary }

  var btnOk = w.addButton(176, w.w-55, 150, 20)
  btnOk.text = "##ok##"
  btnOk.callback = function() {
      g_session.player.setSalary(wantSalary);
      var current = g_session.city.rank();

      if (wantSalary > g_config.ranks[current].salary)
        g_ui.addInformationDialog( "##changesalary_warning##", "##changesalary_greater_salary##" )
  }

  var btnCancel = w.addButton(176, w.w-32, 150, 20)
  btnCancel.text = "##cancel##"
  btnCancel.tooltip = "##exit_salary_window##"
  btnCancel.callback = function() { w.deleteLater() }
}
