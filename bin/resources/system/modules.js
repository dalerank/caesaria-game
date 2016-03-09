engine.log("CaesarIA: load modules started")

var modules = [
												 "game_config", 
												 "init_session", 
												 "init_gui",
												 "init_city", 
												 "vector", 
												 "video_settings",
												 "audio_settings", 
												 "mission_topmenu",
												 "mission_events",
												 "mission_select", 
												 "mission_debug",
												 "package_settings", 												 
												 "city_settings",
												 "gamespeed_settings", 
												 "lobby"
											 ]

for (var i in modules)
	engine.loadModule(":/system/"+modules[i]+".js")
