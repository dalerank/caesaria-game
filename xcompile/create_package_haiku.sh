#!/bin/bash
SF_PASSWORD=$1
BPATH=/Blank_BFS

cd $BPATH/projects/caesaria
git pull origin master
REVISION_NUMBER=`git rev-list HEAD --count`

#rm -rf build
#mkdir build 
#cd build
cmake -DCMAKE_BUILD_TYPE=Release .
#make -j5
cd $BPATH/projects/caesaria-test

echo "Update resources"
./updater.haiku --no-exec

echo "Compress package 4 haiku"
PACKAGE_NAME_HAIKU=caesaria_nightly_haiku_$REVISION_NUMBER.zip
zip -r $PACKAGE_NAME_HAIKU resources README.md caesaria.haiku

echo "Upload nigtly build haiku to sf.net"
scp $PACKAGE_NAME_HAIKU dalerank@frs.sourceforge.net:/home/frs/project/opencaesar3/bin/
scp caesaria.haiku updater.haiku dalerank@frs.sourceforge.net:/home/frs/project/opencaesar3/stuff/


