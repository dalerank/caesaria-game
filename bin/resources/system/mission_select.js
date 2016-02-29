function OnShowMissionSelectDialog()
{
    var wnd = g_ui.addWindow(0, 0, 1024, 768);
    wnd.title = "##mainmenu_playmission##";
    wnd.closeAfterKey( {escape:true,rmb:true} );

    wnd.addImage(0,0,"ckit_00001");

    var selector = wnd.addFileSelector(210, 350, 300, 260);
    selector.background = false;
    selector.itemHeight = 16;
    selector.font = "FONT_1_WHITE";
    selector.itemColor = { simple:"0xffffffff", hovered:"0xff000000" };

    var files = g_session.getFiles(g_session.missionsdir.str, ".mission");
    selector.items = files;

    var imgPreview = wnd.addImage(280, 175, 170, 150);
    //mode : "center"

    var lbDescription = wnd.addDictionaryText(525, 165, 280, 400);
    lbDescription.font = "FONT_1";

    var btnLoad = wnd.addButton(520, 573, 290, 25);
    btnLoad.font = "FONT_2";
    btnLoad.enabled = false;
    btnLoad.style = "whiteBorderUp";
    btnLoad.textAlign = { v:"center", h:"center" };
    btnLoad.text = "##start_this_map##";

    var lbExitHelp = wnd.addLabel(520, 598, 290, 20);
    lbExitHelp.text = "##press_escape_to_exit##";
    lbExitHelp.font = "FONT_1";
    lbExitHelp.textAlign = { v:"center", h:"center" };

    selector.onSelectedCallback = function(index) {
        var path = files[index];
        var mission = new MissionInfo(path);

        g_session.loadLocalization(mission.localization);

        lbDescription.text = mission.desc;
        wnd.title = mission.title;
        imgPreview.picture = mission.image;
        btnLoad.enabled = mission.map.lenght > 0;
    }
}
