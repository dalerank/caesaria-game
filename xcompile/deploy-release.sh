#!/bin/bash
# inside this git repo we'll pretend to be a new user
git config user.name "Travis CI build bot"
git config user.email "caesaria@travis.ci"

# The first and only commit to this new Git repo contains all the
# files present with the commit message "Deploy to GitHub Pages".
REL_NAME=caesaria-release-$ARCH
REL_NAME_STEAM=caesaria-release-steam-$ARCH

mv bin/caesaria.macos bin/${REL_NAME}.macos
mv bin/caesaria.linux bin/${REL_NAME}.linux
mv bin/caesaria.exe   bin/${REL_NAME}.exe
mv bin/caesaria-steam.macos   bin/${REL_NAME_STEAM}.macos
mv bin/caesaria-steam.linux   bin/${REL_NAME_STEAM}.linux
mv bin/caesaria-steam.exe   bin/${REL_NAME_STEAM}.exe
git add bin/${REL_NAME}.macos
git add bin/${REL_NAME}.linux
git add bin/${REL_NAME}.exe
git add bin/${REL_NAME_STEAM}.macos
git add bin/${REL_NAME_STEAM}.linux
git add bin/${REL_NAME_STEAM}.exe
git commit -m "deploy release binary to repository"

# Force push from the current repo's master branch to the remote
# repo's gh-pages branch. (All previous history on the gh-pages branch
# will be lost, since we are overwriting it.) We redirect any output to
# /dev/null to hide any sensitive credential data that might otherwise be exposed.
git push --force --quiet "https://${GH_TOKEN}@${GH_REF}" master
