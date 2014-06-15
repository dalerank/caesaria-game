#!/bin/bash
SF_PASSWORD=$1

cd ~/projects/caesaria
git pull origin master
REVISION_NUMBER=`git rev-list HEAD --count`

rm -rf build
#mkdir build 
#cd build
cmake -DCMAKE_BUILD_TYPE=Release .
make -j5

cd ~/projects/caesaria-test

./updater.macos --no-exec

echo "Compress package 4 macos"
PACKAGE_NAME_MACOS=caesaria_nightly_macos_$REVISION_NUMBER.zip
zip -r $PACKAGE_NAME_MACOS resources README.md caesaria.macos

echo "Upload nigtly build macos to sf.net"
sshpass -p $SF_PASSWORD scp $PACKAGE_NAME_MACOS dalerank@frs.sourceforge.net:/home/frs/project/opencaesar3/bin/
sshpass -p $SF_PASSWORD scp caesaria.macos updater.macos dalerank@frs.sourceforge.net:/home/frs/project/opencaesar3/stuff/


