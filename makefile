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

OBJECTS=caesar.o pic_loader.o tilemap.o tilemap_area.o city.o path_finding.o scenario.o serializer.o scenario_saver.o scenario_loader.o model_loader.o \
	picture.o sdl_facade.o gui_paneling.o sound_engine.o gfx_engine.o gfx_sdl_engine.o gfx_gl_engine.o screen.o screen_wait.o screen_menu.o screen_game.o \
	gui_tilemap.o house_level.o house.o building_data.o factory_building.o service_building.o training_building.o good.o building.o warehouse.o \
	walker.o walker_market_buyer.o walker_cart_pusher.o gui_info_box.o oc3_buildmenu.o oc3_emigrant.o oc3_eventconverter.o oc3_widgetanimator.o oc3_guienv.o \
	oc3_label.o oc3_menu.o oc3_menurgihtpanel.o oc3_pictureconverter.o oc3_pushbutton.o oc3_resourcegroup.o oc3_startmenu.o oc3_time.o oc3_topmenu.o \
	oc3_widget.o oc3_widgetpositionanimator.o oc3_infoboxmanager.o oc3_roadpropagator.o oc3_burningruins.o oc3_water_buildings.o oc3_collapseruins.o


all: $(CAESAR_EXE) tags chmod

# phony commands are executed without condition
.PHONY: chmod tags gettext

$(CAESAR_EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(CAESAR_EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o

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
	# Extract strings from source code, merge into translation file
	xgettext -c++ --output=fr/LC_MESSAGES/caesar.po --join-existing -k"_" *.cpp
	# Remove comments
	sed -i "/^#/d" fr/LC_MESSAGES/caesar.po
	# Compile
	msgfmt fr/LC_MESSAGES/caesar.po -o fr/LC_MESSAGES/caesar.mo


