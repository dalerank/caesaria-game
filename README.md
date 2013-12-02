CaesarIA
-----------
CaesarIA is a remake of the classic Caesar3 PC game, a city building game developed by Impression Games and published by Sierra Entertainment, in 1998.

The game is currently under heavy development and nearly playable state. Developers are invited to participate.

Building CaesarIA
-------------------
See INSTALL_LINUX.md or INSTALL_WINDOWS.md depending on which operating system you are running.

####External dependencies
CaesarIA is build using following excellent libraries:
  * SDL - http://www.libsdl.org/
    * SDL_image - http://www.libsdl.org/projects/SDL_image/
    * SDL_mixer - http://www.libsdl.org/projects/SDL_mixer/
    * SDL_ttf - http://www.libsdl.org/projects/SDL_ttf/
    
Releases:
---------
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

