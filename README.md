![CaesarIA](/logo.png)
![BuildStatus](https://drone.io/bitbucket.org/dalerank/caesaria/status.png)

CaesarIA is remake of Caesar III in the big and seamless world of Ancient Rome. You can build the small village with some of gardens and a workshop where you want.
You also can perform tasks of an emperor or build the whole city and broke attacks of barbarians, Egyptians or carfagens which want to steal your goods and to kill your inhabitants!
You also can pass the company from original game (when it will be restored)
The good lock with high walls will help to cope with any invasions. 

The fighting system of game continues and develops the principles from Caesar III. Legions of soldiers can be sent to any point of the card, but don't submit to direct instructions of the player, and work independently.
Depending on a situation soldiers can recede or be reformed for the best interaction.

The agriculture, extraction and production of goods will demand adjustment of production chains and if the necessary goods can't be got in the city, your trade partner can always sell it to you using
land or maritime trade routes.

Building CaesarIA
-------------------
See INSTALL_LINUX.md or INSTALL_WINDOWS.md depending on which operating system you are running.

#### External dependencies
CaesarIA is build using following excellent libraries:
  * SDL - http://www.libsdl.org/
  * SDL_mixer - http://www.libsdl.org/projects/SDL_mixer/
  * SDL_ttf - http://www.libsdl.org/projects/SDL_ttf/
    
Releases:
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

