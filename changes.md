Releases:
##### 2014-05-07 (b1465)
  * Romanian and french localization start
  * Earthquake events add
  * Chief advisor show more city parameters
  * House may grow with vacant place too
  * Factory stop when outstock full
  * Add balista on tower and arrows for it
  * Localization improved, less textholders now
  * Now tower need soldier for produce wall guards
  * Rating advisor now correctly say what you need for raise culture rating
  * Entertainment buildings now have self troubles that show in information window
  * Improved city troubles information layer, now buildings without troubles not showing on map
  * Non-flat gardens now draw correctly
  * Add video event when start epidemy in city
  * Add empire tax
  * Add rome spearman and mounted and fort for those.
  * Imperial request now deisplay with movie
  * Improved attack alg from enemy soldiers, now more lot of walkers at one tile
  * Android add "Enter" button on action bar
  * Windows and layers now close by Escape press
  * Fixed calculating gods relation such original game
  * Add links to steam page and home page from start menu
  * Second tutorial mission fixed adviser enable event
  * Farm/Warehouse tiles drawing fixed, now show all tiles.
  * Calculating relation interval for gods less to one week
  * Add different color for selected menu item
  * Workers wage now may change from employers advisor window
  * Add play sound when tutorial window show
  * Add relation/absolute path for game resources, forexample "./resources/audio/1.wav" 
    equale "/audio/1.wav", that using in configs and scripts 
  * Add system warning about lost original resources
  * City information layer for damage now display animations
  * fix crash, when debug output cannot be created
  * Imperial request reminder window add
  * Show warning message when inland lake dock built

##### 2014-04-09 (b1362)
  * prefects attack enemy soldiers
  * atlas of animations redesign, fast search in the atlas, settings of animations in the configuration file
  * fish places cease to appear if in the city there is no river, or not access to the river
  * fix bug with a freeze citizens after loading game
  * road construction is corrected, now it no use existing roads
  * movement system for citizens use control points, not direction list
  * can use different speeds of movement for citizens
  * localization of the first training mission is corrected
  * city services reset when loading new game or saved
  * fix crash a sound theme at card change (pc)
  * add emperor and opportunity to send him gifts is added
  * add briefing for 4 missions and prepare of mission in Tarraco
  * In a repository stable branch is added, from where it is possible to download last stable sources
  * added garbage on water
  * way search now use deep water and shoal different
  * Employees try to find a way to the base if on a way was broken, instead of removal from map before
  * It is corrected shifts for low/ship bridges. Also bridges are drawn without auxiliary тайлов.
  * add ADB console ouptut
  * Now it is possible to hold a festival
  * fix the admission of processing of some events of a touchscreen on the android (the buttons are pressed by one tap)
  * loading games on android Is corrected 

##### 2014-03-21

  * Now may use native Caesar III(c) resources without repack to zip archives, use cmd option -c3gfx /path/to/caesar3_folder 
  * Add Tarrentum mission (3 military)
  * Add sound options window
  * Add background music player
  * Increased interval between month change
  * Add drawing sprites over walkers in constuctions
  * Capua mission improved.
  * Add briefing screen for mission select
  * Now may load custom archives list, see game/resources/archives.model
  * Now hippodrome and chariot workshop work correctly
  * Fixed chariot animation
  * Governor's houses now not need workers
  * Add project and compile instructions for Android build
  * Existence of necessary goods is displayed near factory
  * Loading window (map, mission or save) now show button for continue
  * Fixed farm broke carts when overload output store
  * Add animation for short information messages
  * Add confirmation dialog before dispatch emperor requesting goods
  * Factory production rates correct, now production rate equale carts/year
  * Granary now say about itself troubles
  * Now may open emperor adviser from senate infobox
  * MarketLady and MarketBuyer now load correctly, before those can lose here goods 
  * Add house morale calculating
  * Add Mercury and Nepture divinities
  * Fountain now evolve/devolbe such terrain around
  * Factory now not applying any goods without workers
  * Buildings animated on crime/damage layer now 
  * Now may switch netween same type buildings from infobox (keyboard button "," and "." )
  * Amphitheater now generate gladiators or actors when possible
  * Add citizens ideas about eduacation state in city
  * Engineer now have own ideas about city
  * BurningRuins have different animation interval by building
  * updater not download haiku-os specific file on windows

##### 2014-02-25

  * Localization improved
  * MarketLady append market access to house, when it buy something 
  * MarketLady now buy foods from warehouse also
  * House now comsume all type of foods, which available on market
  * MarketKid save/load fixed
  * Game not crashed if priest cannot load state from save
  * Fixed loading serviceman from save
  * Fixed display city wokers number on Employer's adviser screen
  * Granary/Warehouse now may use "deliver good" order
  * Dock correct trade operation, only 2 workers active work from it
  * SeaMerchant now go away from city when buy all needed goods
  * Add stacktrace for informatively logging error
  * Fixed trade route save\load land type
  * Fixed merchant leave city if no trade operations did
  * Save game state improved, now save events from missions
  * TaxCollector fixed bug with big money collecting
  * capua.mission improved empire start state
  * empire map, now may contain drawble objects
  * entry point now correct loading from (caesarIII).sav files
  * EmperrorWindow display request improved
  * Fixed emperror requests complete
  * Tooptip  widget displaing fixed
  * Tutrorial mission now use map from original story
  * City funds history fixed step add

##### 2014-02-12

  * MacOS X an Haiku support
  * Mission engine and event dispatcher improved
  * more citizen thinks 
  * education, entertainment layer add
  * culture rating calculate improved
  * listbox now may drawing image
  * first tutorial mission add
  * fast save/load
  * tutorial window add
  * senate show rating
  * ruins infobox add
  * fort and legions add
  * low bridge behaviour improved
  * goods consumption fixed

##### 2013-17-11

  * Garden desirability improved
  * House upgrade logic improved
  * Migrants pathfinder logic improved
  * Warehouse/Granary now save/load orders
  * City walkers grid improved
  * BurnedRuins now flat. Prefect logic improved
  * theater/lionpit/gl.school animations improved
  * Walker now update pathway if next time impossible
  * empire trading system now save/load
  * fishplace crash on destroy fixed
  * layer crime drawing improved
  * prefect now catch protestors
  * protestors add 

##### 2013-10-31
  * warf and fishing boat add
  * translation system improved
  * education adviser, employers adviser improved
  * source structure refactoring
  * add tile animation
  * city population model changed
  * speed options window and video options added add
  * big screnn resolutions add
  * utf8 text rendering
  * paved road add
  * health layer add
  * desirability calculating improved
  * city workers model changed
  * house now using furniture and pottery
  * actor and bathlady available onloading from oc3save
  * actor colony\theater logic improved, add warning when theater build if no actor colony in city
  * widget now support proportional geometry
  * house infobox now show what need for next level.
  * house upgrade improved
  * plague ruins add
  * fountain animation fixed
  * other small fixes


##### 2013-05-11
  * Fixed a lot of bugs.
  * Minimap is introduced.
  * Migration to cmake building system.
  * Houses now can evolve/devolve (a little bit buggy).

##### 2013-04-03
  * Added many buildings.

##### 2013-03-02
  * First binary release for windows.
  * Start implement the menubar with money/population/date.
  * Use the slim interface for build menu (regression: no more access to save/load game).
  * Start implement funds and taxes.

##### 2013-02-09
  * Implement clear land.
  * Add copyright notice.

##### 2012-12-01
  * Save/load game complete.
  * Implement nearly all entertainment buildings.

##### 2012-11-10
  * Save/load game starts to work.

##### 2012-10-12
  * Start implement load game.

##### 2012-09-17
  * Start implement save game.

##### 2012-06-14
  * Big code refactoring of the GUI. prepare for game music.

##### 2012-05-21
  * Add an installation procedure.

##### 2012-05-15
  * Implement some buildings (water, entertainment, education). 
  * Start implementation of house evolution/regression.

##### 2012-04-20
  * First public release.
