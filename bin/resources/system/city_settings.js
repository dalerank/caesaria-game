var ctsettings = {
	getop : function(obj) {
	  engine.log(obj.group + " " + obj.flag);
		if (obj.group=="city")
			return g_session.getCityflag(obj.flag);
			
		if (obj.group=="game")
			return engine.getOption(obj.flag);
			
		if (obj.group=="draw")
			return g_session.getDrawsflag(obj.flag);
			
		return 0;	
	},
	
	setop : function(obj,value) {
		engine.log(obj.group + " " + obj.flag);
		if (obj.group=="city")
			return g_session.setCityflag(obj.flag);
			
		if (obj.group=="game")
			return engine.setOption(obj.flag);
			
		if (obj.group=="draw")
			return g_session.setDrawsflag(obj.flag);	
	}
}

function OptionButton(x,y,parent,obj)
{
	 this.obj = obj;
	 this.btn = parent.addButton(x,y,parent.w/2-30,24);
	 
	 this.btn.callback = function() {
			var value = ctsettings.getop(this.obj);
			var states = this.obj.states;
			for(var i in this.states)
			{
				if (states[i] == value)
				{
					i = (i+1)%states.length;					
					ctsettings.setop(this.obj,i);
					return;
				}				
			}
	 };
	  
	 this.update = function() {

			 var value = ctsettings.getop(this.obj);
		 	 this.btn.text = "##"+this.obj.base+ ("_"+this.obj.states[value]+"##");
			 this.btn.tooltip = "##"+this.obj.base+ ("_"+this.obj.states[value]+"_tlp##");
	 };
	 
	 this.update();
}

function OnShowCitySettings()
{
	var w = g_ui.addWindow(0, 0, 480, 540);
  w.closeAfterKey( {escape:true,rmb:true} )
	w.title = "##city_options##";

	var items = [ {base:"city_opts_god", 						states : [ "off", "on" ],   group : "city", flag:"godEnabled"},
											 {base:"city_warnings", 						states : [ "off", "on" ],   group : "city", flag:"warningsEnabled" },
											 {base:"city_zoom",     						states : [ "off", "on" ],   group : "city", flag:"zoomEnabled" },
											 {base:"city_zoominv",  						states : [ "off", "on" ],   group : "city", flag:"zoomInvert" },
											 {base:"city_lockinfo", 							states : [ "off", "on" ],   group : "game", flag:"lockInfobox" },
											 {base:"city_barbarian",						states : [ "off", "on" ],   group : "city", flag:"barbarianAttack" },      
											 {base:"city_tooltips", 							states : [ "off", "on" ],   group : "game", flag:"tooltipEnabled" },
											 {base:"city_buildoverdraw",			states : [ "off", "on" ],   group : "draw", flag:"overdrawOnBuild" },
											 {base:"city_buildoverdraw",			states : [ "off", "on" ],   group : "city", flag:"showGodsUnhappyWarn" },
											 {base:"city_warf_timber",				states : [ "off", "on" ],   group : "city", flag:"warfNeedTimber" },
											 {base:"river_side_well",					states : [ "off", "on" ],   group : "city", flag:"riversideAsWell" },
											 {base:"sldr_wages",								states : [ "off", "on" ],   group : "city", flag:"soldiersHaveSalary" },
											 {base:"personal_tax",							states : [ "off", "on" ],   group : "city", flag:"housePersonalTaxes" },
											 {base:"cut_forest",									states : [ "off", "on" ],   group : "city", flag:"cutForest2timber" },
											 {base:"rightMenu",									states : [ "off", "on" ],   group : "game", flag:"rightMenu" },
											 {base:"city_mapmoving", 				states : ["lmb","mmb"],  group : "draw", flag:"mmbMoving" },
											 {base:"city_debug",     						states : [ "off", "on" ],   group : "game", flag:"debugMenu" },
											 {base:"city_chastener", 				  states : [ "off", "on" ],   group : "city", flag:"legionAttack" },
											 {base:"city_androidbar",					states : [ "off", "on" ],   group : "game", flag:"showTabletMenu" },
											 {base:"city_ccuseai",    					states : [ "off", "on" ],   group : "game", flag:"ccUseAI" },
											 {base:"city_highlight_bld",				states : [ "off", "on" ],   group : "city", flag:"highlightBuilding" },
											 {base:"city_destroy_epdh",			states : [ "off", "on" ],   group : "city", flag:"destroyEpidemicHouses" },
											 {base:"city_border",    						states : [ "off", "on" ],   group : "draw", flag:"borderMoving" },
											 {base:"city_forest_fire",					states : [ "off", "on" ],   group : "city", flag:"forestFire" },
											 {base:"city_forest_grow",				states : [ "off", "on" ],   group : "city", flag:"forestGrow" },
											 {base:"city_claypit_collapse",  states : [ "off", "on" ],    group : "city", flag:"claypitMayCollapse" },
											 {base:"city_mines_collapse",		states : [ "off", "on" ],   group : "city", flag:"minesMayCollapse" },
											 {base:"draw_svk_border",				states : [ "off", "on" ],   group : "city", flag:"svkBorderEnabled" },
											 {base:"city_farm_use_meadow",states:[ "off", "on" ],   group : "city", flag:"farmUseMeadows" },
	]
	
	var sx=15;
	var sy=40;
	for(var i in items)
	{		 
		 if(sy+24>w.h-40)
		 {
				sx=w.w/2;
				sy=40;
		 }
		
		 var btn = new OptionButton(sx,sy,w,items[i]);	
		 sy = sy + 25;
	}
	
	/*btnDifficulty#PushButton : { maximumSize : [ 0, 24 ], bgtype : "smallGrayBorderLine", text : "##city_barbarian_mode##" }
      btnRoadBlocks#PushButton : { maximumSize : [ 0, 24 ], bgtype : "smallGrayBorderLine", text : "##city_roadblock_mode##" }
			btnToggleBatching#PushButton  : { maximumSize : [ 0, 24 ], bgtype : "smallGrayBorderLine", text : "##city_batching_mode##"    }
      btnMetrics#PushButton         : { maximumSize : [ 0, 24 ], bgtype : "smallGrayBorderLine", text : "##city_metrics_mode##"     }

      sbFireRisk#SpinBox :
      {
        maximumSize : [ 0, 24 ]
        font : "FONT_2"
        postfix : "%"
        text : "Fire risk"
        min : 10
        max : 150
        step : 10
      }

      sbCollapseRisk#SpinBox : {
        maximumSize : [ 0, 24 ]
        font : "FONT_2"
        postfix : "%"
        text : "Collapse risk"
        min : 10
        max : 150
        step : 10
      }
    }
  
	*/
	
	w.addExitButton(w.w-34,w.h-34)
	w.moveToCenter()
	w.setModal()
	w.mayMove = false
}