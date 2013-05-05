# This file is part of openCaesar3.
#
# openCaesar3 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# openCaesar3 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
#
# Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#
# Usage:
#   make OS=linux    ==> compiles for linux
#   make OS=win32    ==> compiles for windows


OS?=linux
ifeq ("$(OS)", "linux")
   # LINUX
   CAESAR_EXE=caesar
   CC=g++
   SDL_CFLAGS=$(shell sdl-config --cflags)
   SDL_LFLAGS=$(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_mixer
   ARCHIVE_LFLAGS=-L/usr/lib -larchive
   GL_LFLAGS=-lGL -lGLU
   CFLAGS=-I. $(SDL_CFLAGS) -c -Wall -g
   LFLAGS=$(SDL_LFLAGS) $(ARCHIVE_LFLAGS) $(GL_LFLAGS) -lboost_system -lboost_filesystem
endif
ifeq ("$(OS)", "win32")
   # WINDOWS
   CAESAR_EXE=caesar.exe
   CC=i686-w64-mingw32-g++
   WIN_PREFIX=/home/greg/dev/caesar/win32
   CFLAGS=-I. -I$(WIN_PREFIX)/include -c -g -Wall
   LFLAGS=-mwindows -lmingw32 -static-libgcc -static-libstdc++ -L$(WIN_PREFIX)/lib -lintl -lopengl32 -lSDLmain -lSDL -lSDL_mixer -lSDL_ttf -lSDL_image -larchive
endif

OBJECTS=oc3_caesar.o oc3_pic_loader.o oc3_tilemap.o oc3_tilemap_area.o oc3_city.o oc3_path_finding.o oc3_scenario.o oc3_serializer.o oc3_scenario_saver.o oc3_scenario_loader.o oc3_model_loader.o \
	oc3_picture.o oc3_sdl_facade.o oc3_gui_paneling.o oc3_sound_engine.o oc3_gfx_engine.o oc3_gfx_sdl_engine.o oc3_gfx_gl_engine.o oc3_screen.o oc3_screen_wait.o oc3_screen_menu.o oc3_screen_game.o \
	oc3_gui_tilemap.o oc3_house_level.o oc3_house.o oc3_building_data.o oc3_factory_building.o oc3_service_building.o oc3_training_building.o oc3_good.o oc3_building.o oc3_warehouse.o \
	oc3_walker.o oc3_walker_market_buyer.o oc3_walker_cart_pusher.o oc3_gui_info_box.o oc3_buildmenu.o oc3_emigrant.o oc3_eventconverter.o oc3_widgetanimator.o oc3_guienv.o \
	oc3_label.o oc3_menu.o oc3_menurgihtpanel.o oc3_pictureconverter.o oc3_pushbutton.o oc3_resourcegroup.o oc3_startmenu.o oc3_time.o oc3_topmenu.o \
	oc3_widget.o oc3_widgetpositionanimator.o oc3_infoboxmanager.o oc3_roadpropagator.o oc3_burningruins.o oc3_water_buildings.o oc3_animation.o \
	oc3_buildingengineer.o oc3_constructionmanager.o oc3_collapsedruins.o oc3_tilemapchangecommand.o oc3_workerhunter.o oc3_senate.o oc3_immigrant.o \
	oc3_walkermanager.o oc3_astarpathfinding.o oc3_buildingprefect.o oc3_cityservice_emigrant.o oc3_cityservice_workershire.o oc3_walker_prefect.o \
	oc3_tile.o oc3_terraininfo.o oc3_factoryclay.o oc3_cityservice_timers.o oc3_timer.o


all: $(CAESAR_EXE) tags chmod gettext

# phony commands are executed without condition
.PHONY: chmod tags gettext

$(CAESAR_EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(CAESAR_EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o $(CAESAR_EXE)

release_src: format
	./release_src.sh

release_win32: caesar.exe
	./release_win32.sh

tags:
	ctags -R --extra=+f --exclude=BAK* --exclude=resources

chmod:
	chmod -x *.?pp

format:
	astyle --formatted --indent=spaces=3 -A1 --suffix=none *.cpp *.hpp

gettext:

	mkdir -p ./en/LC_MESSAGES
	mkdir -p ./fr_FR/LC_MESSAGES
	
	# Extract strings from source code, merge into translation file
	xgettext -c++ --output=locales/fr_FR.po --join-existing -k"_" *.cpp
	# Remove comments
	#sed -i "/^#/d" fr/LC_MESSAGES/caesar.po
	# Compile
	msgfmt locales/fr_FR.po -o fr_FR/LC_MESSAGES/caesar.mo
	
	# Extract strings from source code, merge into translation file
	xgettext -c++ --output=locales/en.po --join-existing -k"_" *.cpp
	# Remove comments
	#sed -i "/^#/d" fr/LC_MESSAGES/caesar.po
	# Compile
	msgfmt locales/en.po -o en/LC_MESSAGES/caesar.mo


