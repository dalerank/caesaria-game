var _format = function() {
    var formatted = arguments[0];
    for (var arg in arguments) {
				if(arg==0)
					continue;
        formatted = formatted.replace("{" + (arg-1) + "}", arguments[arg]);
    }
    return formatted;
};

var ctsettings = {
    getop : function(obj) {
      engine.log("get" + obj.group + " " + obj.flag);
      switch(obj.group) {
			case "city": return g_session.getCityflag(obj.flag);
		    case "game": return engine.getOption(obj.flag);
			case "draw": return engine.getDrawsflag(obj.flag);
			case "build": return g_session.getBuildflag(obj.flag);
            case "gui": return this.getguiv(obj.flag);
            case "risks": return g_session.getCityflag(obj.flag);
			}
      return 0;
    },

    setop : function(obj,value) {
        engine.log("set " + obj.group + " " + obj.flag);
        switch(obj.group) {
				case "city":  g_session.setCityflag(obj.flag, value); break;
                case "game":  engine.setOption(obj.flag, value); break;
				case "draw":  engine.setDrawsflag(obj.flag, value); break;
				case "build": g_session.setBuildflag(obj.flag, value); break;
				case "gui":   this.setguiv(obj.flag); break;
				case "risks": g_session.setCityflag(obj.flag,value); break;
			}
    }, 

    getguiv : function(name)
    {
        engine.log("get guiopts " + name);
        if(name=="debug_menu")
        {
            return sim.ui.topmenu.debugmenu.visible();
        } 
    },

    setguiv : function(name)
    {
        engine.log("set guiopts " + name);
        if(name=="debug_menu")
        {
            var dm = sim.ui.topmenu.debugmenu;
            dm.setVisible(!dm.visible());
        }
    },

    next : function(obj) {
        var value = {}
        if(obj.group==="risks") 
        {
            value =  g_session.getCityflag(obj.flag)
            value += 10;
            if(value>100)
               value=0;
        }
        else if(obj.group==="gui")
		{
            return obj.value ? 0 : 1;
		}
		else
        {
            value = this.getop(obj)
            value += 1;
            if (value>=obj.states.length) 
                value = 0;            
        }
				
		return value;
    },

    text : function(obj) {
        var value = {};
        if(obj.group==="risks")
        {
            value = g_session.getCityflag(obj.flag);
			var lb = _t("##"+obj.base+"##");
            return _format( "{0} {1} %", lb, value);
        }
		else if(obj.group==="gui")
		{
			if(obj.flag === "debug_menu")
			{
				var value = this.getguiv(obj.flag);
				var tx =  _format("##{0}_{1}##",obj.base,obj.states[value]);
				engine.log(tx);
				return _t(tx);
			}
		}
        else
        {
		    value = this.getop(obj)
		    var tx = _format("##{0}_{1}##", obj.base, obj.states[value]);
            return _t(tx);
        } 
    },

    tooltip : function(obj) {
        if(obj.group==="risks")
        {
            return "";
        }
		else if(obj.group==="gui")
		{
			 if(obj.flag === "debug_menu")
				return "";
		} 
        else
        {
            var value = this.getop(obj)
            return _t("##"+obj.base+"_"+obj.states[value]+"_tlp##");
        }
    }
}

sim.ui.topmenu.options.showCitySettings = function() 
{
    var items = [ 	 {base:"city_opts_god", 		states : [ "off", "on" ],   group : "city", flag:"godEnabled"},
             {base:"city_warnings", 		states : [ "off", "on" ],   group : "city", flag:"warningsEnabled" },
             {base:"city_zoom",     		states : [ "off", "on" ],   group : "city", flag:"zoomEnabled" },
             {base:"city_zoominv",  		states : [ "off", "on" ],   group : "city", flag:"zoomInvert" },
             {base:"city_lockinfo", 		states : [ "off", "on" ],   group : "game", flag:"lockInfobox" },
             {base:"city_barbarian",		states : [ "off", "on" ],   group : "city", flag:"barbarianAttack" },
             {base:"city_tooltips", 		states : [ "off", "on" ],   group : "game", flag:"tooltipEnabled" },
             {base:"city_buildoverdraw",	states : [ "off", "on" ],   group : "draw", flag:"overdrawOnBuild" },
             {base:"city_buildoverdraw",	states : [ "off", "on" ],   group : "city", flag:"showGodsUnhappyWarn" },
             {base:"city_warf_timber",		states : [ "off", "on" ],   group : "city", flag:"warfNeedTimber" },
             {base:"river_side_well",		states : [ "off", "on" ],   group : "city", flag:"riversideAsWell" },
             {base:"sldr_wages",		states : [ "off", "on" ],   group : "city", flag:"soldiersHaveSalary" },
             {base:"personal_tax",		states : [ "off", "on" ],   group : "city", flag:"housePersonalTaxes" },
             {base:"cut_forest",		states : [ "off", "on" ],   group : "city", flag:"cutForest2timber" },
             {base:"rightMenu",			states : [ "off", "on" ],   group : "game", flag:"rightMenu" },
             {base:"city_mapmoving", 		states : [ "lmb", "mmb"],   group : "draw", flag:"mmbMoving" },
             {base:"city_debug",     		states : [ "off", "on" ],   group : "game", flag:"debugMenu" },
             {base:"city_chastener", 		states : [ "off", "on" ],   group : "city", flag:"legionAttack" },
             {base:"city_androidbar",		states : [ "off", "on" ],   group : "game", flag:"showTabletMenu" },
             {base:"city_ccuseai",    		states : [ "off", "on" ],   group : "game", flag:"ccUseAI" },
             {base:"city_highlight_bld",	states : [ "off", "on" ],   group : "city", flag:"highlightBuilding" },
             {base:"city_destroy_epdh",		states : [ "off", "on" ],   group : "city", flag:"destroyEpidemicHouses" },
             {base:"city_border",    		states : [ "off", "on" ],   group : "draw", flag:"borderMoving" },
             {base:"city_forest_fire",		states : [ "off", "on" ],   group : "city", flag:"forestFire" },
             {base:"city_forest_grow",		states : [ "off", "on" ],   group : "city", flag:"forestGrow" },
             {base:"city_claypit_collapse", states : [ "off", "on" ],   group : "city", flag:"claypitMayCollapse" },
             {base:"city_mines_collapse",	states : [ "off", "on" ],   group : "city", flag:"minesMayCollapse" },
             {base:"draw_svk_border",		states : [ "off", "on" ],   group : "city", flag:"svkBorderEnabled" },
             {base:"city_farm_use_meadow",	states : [ "off", "on" ],   group : "city", flag:"farmUseMeadows" },
             {base:"city_fire_risk",            group : "risks", flag : "fireCoeff" },
             {base:"city_collapse_risk",        group : "risks", flag : "collapseKoeff" },
             {base:"city_df",               states : [ "fun", "easy", "simple", "usual", "nicety", "hard", "impossible" ], group : "city", flag : "difficulty" },
             {base:"city_batching",         states : [ "off", "on" ],   group : "draw", flag:"batching" },
             //{base:"city_c3rules",          states : [ "off", "on" ],   group : "city", flag:"c3gameplay" },
             //{base:"city_roadblock",        states : [ "off", "on" ],   group : "build", flag: "roadBlock" },
			 {base:"city_debug", states : [ "off", "on" ], group : "gui", flag : "debug_menu" },
    ]

    var w = g_ui.addWindow(0, 0, 480, 540);
    w.closeAfterKey( {escape:true,rmb:true} ) 
    w.title = "##city_options##";

    var lbHelp = w.addLabel(15,w.h-40,w.w-80,20);
    lbHelp.text = "##dblclick_to_change_option##";
    lbHelp.font = "FONT_1";

    var lbxModes = w.addListbox(25, 40, w.w-50, w.h-90);
    lbxModes.setTextAlignment( "center", "center" );
    lbxModes.background = true;
    lbxModes.onDblclickCallback = function(index) {
            var obj = items[index];
            var value = ctsettings.next(obj)
            var states = obj.states;

            ctsettings.setop(obj,value);

            lbxModes.setItemText(index,ctsettings.text(obj));
            lbxModes.setItemTooltip(index,ctsettings.tooltip(obj));
        }

    for(var i in items)
    {
        var obj = items[i];
        engine.log(obj.base);
        var index = lbxModes.addLine(ctsettings.text(obj));
        lbxModes.setItemTooltip(index,ctsettings.tooltip(obj));
    }

    /*btnDifficulty#PushButton : { maximumSize : [ 0, 24 ], bgtype : "smallGrayBorderLine", text : "##city_barbarian_mode##" }
      btnMetrics#PushButton         : { maximumSize : [ 0, 24 ], bgtype : "smallGrayBorderLine", text : "##city_metrics_mode##"     }
    }

    */

    w.addExitButton(w.w-34,w.h-34)
    w.moveToCenter()
    w.setModal()
    w.mayMove = false
}
