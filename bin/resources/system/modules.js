engine.log("CaesarIA: load modules started")

var modules = [
                  "namespaces",
                  "common",
                  "vector",
                  "game_config",
                  "init_session",
                  "init_gui",
                  "init_city",
                  "init_game",
                  "init_sound",
                  "init_colors",
                  "init_ranks",
                  "init_movie",
                  "init_render",
                  "init_picture",
                  "init_languages",
                  "video_settings",
                  "audio_settings",
                  "city_settings",
                  "gamespeed_settings",
                  "mission_topmenu",
                  "mission_events",
                  "mission_targets",
                  "mission_debug",
                  "mission_timescale",
                  "mission_dialogs",
                  "splash_common",
                  "package_settings",
                  "salary_settings",
                  "climate_manager",
                  "emperor_gift",
                  "mission_common",
                  "hotkeys",
                  "lobby",
                  "lobby_select_mission"
              ]

for (var i in modules)
    engine.loadModule(":/system/"+modules[i]+".js")
