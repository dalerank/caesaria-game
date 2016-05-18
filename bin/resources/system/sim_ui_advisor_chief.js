sim.ui.advisors.chief = {}

sim.ui.advisors.chief.consts = {
  cityHavenotFood : -4,
  cityHavenotFoodNextMonth : 0,
  cityHaveSmallFoodNextMonth : 1,
  cityPriduceMoreFood : 2,
  bigWorklessPercent : 10,
  enemyNearCityGatesDistance : 40,
  enemyNearCityDistance : 100,
  bigThreatValue : 100,
  serviceAwesomeCoverage : 100,
  haveThreatDistance : 200,
  infinteDistance : 999,
}

sim.ui.advisors.chief.rows = {
  employers :      {index:0 , title:"advchief_employment"       },
  profit    :      {index:1 , title:"advchief_finance"          },
  migration :      {index:2 , title:"advchief_migration"        },
  foodStock :      {index:3 , title:"advchief_food_stocks"      },
  foodConsumption: {index:4 , title:"advchief_food_consumption" },
  military:        {index:5 , title:"advchief_military"         },
  crime:           {index:6 , title:"advchief_crime"            },
  health:          {index:7 , title:"advchief_health"           },
  education:       {index:8 , title:"advchief_education"        },
  religion:        {index:9 , title:"advchief_religion"         },
  entertainment:   {index:10, title:"advchief_entertainment"    },
  sentiment:       {index:11, title:"advchief_sentiment"        }
}

sim.ui.advisors.chief.addEmploymentState = function(w) {
  var wInfo = g_city.laborDetails;
  var workless = g_city.worklessPercent;
  var reasons = [];

  var consts = sim.ui.advisors.chief.consts;

  if (g_city.population == 0) {
    reasons.push( {text:"##no_people_in_city##", color:"brown"} );
  } else {
    var needWorkersNumber = wInfo.need - wInfo.current;
    if (needWorkersNumber > 10) {
      reasons.push( {text:_format( "{0} {1}", _("##advchief_needworkers##"), needWorkersNumber ),
                     color:"brown"} );
    } else if(workless > consts.bigWorklessPercent) {
      reasons.push( {text:_format( "{0} {1}%", _("##advchief_workless##"), workless ),
                     color:"brown" } );
    } else {
      reasons.push({text:"##advchief_employers_ok##", color:"black"});
    }
  }

  w.addReportRow("employers", reasons);
}

sim.ui.advisors.chief.drawFoodConsumption = function(w) {
  var text = "";
  var fk = g_city.getParam(g_config.history.thisMonth,g_config.cityparam.foodKoeff);

  if (fk < this.consts.cityHavenotFood) {
    text = "##we_eat_much_then_produce##";
  } else if( fk > this.consts.cityPriduceMoreFood ) {
    text = "##we_produce_much_than_eat##";
  } else {
    switch(fk) {
    case -3: text = "##we_eat_more_thie_produce##"; break;
    case -2: text = "##we_eat_some_then_produce##"; break;
    case -1: text= "##we_produce_less_than_eat##"; break;
    case 0: text = "##we_noproduce_food##"; break;
    case 1: text = "##we_produce_some_than_eat##"; break;
    case 2: text = "##we_produce_more_than_eat##"; break;
    }
  }

  var reasons = [];
  reasons.push( { text:text, color:"black" } );
  w.addReportRow("foodConsumption", reasons);
}

sim.ui.advisors.chief.drawFoodStockState = function(w) {
  var romeSendWheat = g_city.isRomeSendWheat;

  var reasons = [];
  if( romeSendWheat ) {
    reasons.push( { text:"##rome_send_wheat_to_city##", color:"green" } );
  } else {
    var text;
    var lastMonthFoodStock = g_city.getParam(g_config.history.thisMonth,g_config.cityparam.foodStock);
    var prevMonthFoodStock = g_city.getParam(g_config.history.lastMonth,g_config.cityparam.foodStock);

    if (lastMonthFoodStock < prevMonthFoodStock) {
      text = "##no_food_stored_last_month##";
    } else {
      var monthWithFood = g_city.getParam(g_config.history.lastMonth,g_config.cityparam.monthWithFood);
      switch (monthWithFood) {
        case this.consts.cityHavenotFoodNextMonth: text = "##have_no_food_on_next_month##"; break;
        case this.consts.cityHaveSmallFoodNextMonth: text = "##small_food_on_next_month##"; break;
        case 2: text = "##some_food_on_next_month##"; break;
        case 3: text = "##our_foods_level_are_low##"; break;

        default:
          text = _format( "{0} {1} {2}", _ut("have_food_for"), monthWithFood, _ut("months") );
        }
    }
    reasons.push( { text: text, color:"black" } );
  }

  if (reasons.length==0) {
    reasons.push( { text:_ut("food_stock_unknown_reason"), color:"red" } );
  }

  w.addReportRow("foodStock", reasons);
}

sim.ui.advisors.chief.drawMigrationState = function(w) {
  var reasons = [];
  var migrationReason = g_city.migrationReason;
  var textColor = "red";

  if (migrationReason.length==0) {
    migrationReason = _u("migration_unknown_reason");
    textColor = "black"
  }

  reasons.push( { text: migrationReason, color:textColor } );

  w.addReportRow("migration", reasons);
}

sim.ui.advisors.chief.drawProfitState = function(w) {
  var reasons = [];
  var profit = g_city.profit;
  var prefix = (profit >= 0 ? "advchief_haveprofit" : "advchief_havedeficit");
  var message = _ut(prefix) + " " + profit;
  var messageColor = profit > 0 ? "black" : "brown";
  reasons.push( { text: message, color: messageColor } );

  w.addReportRow("profit", reasons);
}

sim.ui.advisors.chief.drawCrime = function(w) {
  var reasons = [];

  var disorderReason = g_city.disorderReason;
  if(disorderReason.length>0) {
    reasons.push( { text:disorderReason, color:"red" } );
  } else {
    reasons.push( { text:"##advchief_no_crime##", color:"green" } );
  }

  w.addReportRow( "crime", reasons );
}

sim.ui.advisors.chief.drawHealth = function(w) {
  var reasons = [];

  var cityHealthReason = g_city.cityHealthReason;
  if (cityHealthReason.length>0) {
    reasons.push( { text:cityHealthReason, color:"red" } );
  } else {
    reasons.push( { text:"##advchief_health_good##", color:"green" } );
  }

  w.addReportRow("health", reasons);
}

sim.ui.advisors.chief.drawEducation = function(w) {
  var reasons = [];

  var avTypes = [ {object:"school", text:"##advchief_some_need_education##"},
                  {object:"library", text:"##advchief_some_need_library##"},
                  {object:"academy", text:"##advchief_some_need_academy##"} ];

  var houses = g_city.houses;
  for (var i=0; i < avTypes.length; i++) {
    var h = houses[i].as(House);
    if (houses.length > 0)
      reasons.push( { text:avTypes[i].text, color:"yellow" } );
  }

  if (reasons.length==0) {
    reasons.push( { text:"##advchief_education_ok##", color:"black" } );
  }

  w.addReportRow( "education", reasons );
}

sim.ui.advisors.chief.drawReligion = function(w) {
  var reasons = [];
  var religionReason = g_city.religionReason;
  if (religionReason.length>0) {
    reasons.push( { text:religionReason, color:"black" } );
  } else {
    reasons.push( { text:"##advchief_religion_unknown##", color:"red" } );
  }

  w.addReportRow("religion", reasons);
}

sim.ui.advisors.chief.drawEntertainment = function(w) {
  var reasons = [];

  var lastFestivalDate = g_city.lastFestivalDate;
  var monthFromLastFestival = lastFestivalDate.monthsTo(g_session.date);
  if (monthFromLastFestival > 6) {
    reasons.push( { text:"##citizens_grumble_lack_festivals_held##", color:"yellow" } );
  }

  var coverage = g_city.getProperty("theatreCoverage");
  if( coverage >= this.consts.serviceAwesomeCoverage ) {
    reasons.push( { text:"##current_play_runs_for_another##", color:"yellow" } );
  }

  /*int hippodromeCoverage = city->statistic().entertainment.coverage( Service::hippodrome );
  if( hippodromeCoverage >= serviceAwesomeCoverage )
  {
    reasons.addIfValid( { "##current_races_runs_for_another##", ColorList::yellow } );
  }*/

  w.addReportRow( "entertainment", reasons );
}

sim.ui.advisors.chief.drawSentiment = function(w) {
  var reasons = [];
  var sentimentReason = g_city.sentimentReason;

  if (sentimentReason.length>0) {
    reasons.push( { text:sentimentReason, color:"black" } );
  } else {
    reasons.push( { text:"##unknown_sentiment_reason##", color:"red" } );
  }

  w.addReportRow( "sentiment", reasons );
}

sim.ui.advisors.chief.drawMilitary = function(w) {
  var reasons = [];
  var threatValue = "";
  var isBesieged = g_city.threatValue > this.consts.bigThreatValue;

  if (!isBesieged) {
    //Notification n = military->priorityNotification();
    reasons.push( { text:"##enemy_closed_to_town##", color:"red" } );
  }

  if (reasons.length==0) {
    var threat = g_city.getProperty("cityThreat");

    if (threat.type) {
      if( threat.distance <= this.consts.enemyNearCityGatesDistance ) {
          var threatText = _format( "##{0}_troops_at_our_gates##", threa.type() );
          reasons.push( { text:threatText, color:"red" } );
      } else if( threat.distance <= this.consts.enemyNearCityDistance ) {
          reasons.push( { text:"##our_enemies_near_city##", color:"red" } );
      } else {
          reasons.push( { text:"##getting_reports_about_enemies##", color:"yellow" } );
      }
    }
  }

  if (reasons.length==0) {
    var barracks = g_city.findOverlays("barracks");

    for(var i=0; i < barracks.length; i++) {
      if (barrack.getProperty("isNeedWeapons")) {
        reasons.push( { text:"##some_soldiers_need_weapon##", color:"yellow" } );
        break;
      }
    }
  }

  if (reasons.length==0) {
    reasons.push( { text:"##no_warning_for_us##", color:"green" } );
  }

  w.addReportRow( "military", reasons );
}

sim.ui.advisors.chief.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";

  w.geometry = {x:0, y:0, w:640, h:420};
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.title = _u("chief_advisor");

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = { rc:"paneling", index:266 };

  var blackframe = w.addLabel(20, 55, w.w-40, w.h-95);
  blackframe.style = "blackFrame"

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "advisor_chief";

  w.addReportRow = function(index, reasons) {
    var conf = sim.ui.advisors.chief.rows[index];

    var btn = new Button(blackframe);
    btn.geometry = {x:5, y:2+27*conf.index, w:blackframe.w-10, h:26}
    btn.style = "brownBorderOnly";
    btn.icon = g_render.picture("paneling", 48);
    btn.iconOffset = {x:6, y:6};
    btn.font = "FONT_4";
    btn.textOffset = {x:255, y:0};

    var reason = reasons[Math.randomIndex(0, reasons.length-1)];
    btn.addLabel(20, 0, 200, 20, _u(conf.title), {size:18, color:reason.color});
    btn.addLabel(220, 0, 300, 20, _t(reason.text), {size:18, color:reason.color} );
  }

  sim.ui.advisors.chief.addEmploymentState(w);
  sim.ui.advisors.chief.drawMigrationState(w);
  sim.ui.advisors.chief.drawProfitState(w);
  sim.ui.advisors.chief.drawFoodStockState(w);
  sim.ui.advisors.chief.drawFoodConsumption(w);
  sim.ui.advisors.chief.drawCrime(w);
  sim.ui.advisors.chief.drawHealth(w);
  sim.ui.advisors.chief.drawEducation(w);
  sim.ui.advisors.chief.drawReligion(w);
  sim.ui.advisors.chief.drawEntertainment(w);
  sim.ui.advisors.chief.drawSentiment(w);
  sim.ui.advisors.chief.drawMilitary(w);
}
