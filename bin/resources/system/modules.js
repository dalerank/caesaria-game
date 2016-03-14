engine.log("CaesarIA: load modules started")

var modules = [
                  "common",
                  "game_config",
                  "init_session",
                  "init_gui",
                  "init_city",
                  "init_game",
                  "vector",
                  "init_colors",
                  "video_settings",
                  "audio_settings",
                  "city_settings",
                  "gamespeed_settings",
                  "mission_topmenu",
                  "mission_events",
                  "mission_select",
                  "mission_targets",
                  "mission_debug",
                  "package_settings",
                  "climate_manager",
                  "emperor_gift",
                  "lobby"
              ]

for (var i in modules)
    engine.loadModule(":/system/"+modules[i]+".js")
