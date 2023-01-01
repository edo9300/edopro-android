#!/usr/bin/env bash

ASSETS=src/main/assets/defaults
UPDATES=src/main/assets/update
mkdir -p $ASSETS
mkdir -p $UPDATES

curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/ProjectIgnis/Distribution/master/fonts/NotoSansJP-Regular.otf
lua ci/bin2c.lua "NotoSansJP-Regular.otf" "deps/gframe/CGUITTFont/bundled_font.cpp"

cd src/main/assets
touch index.txt
touch filelist.txt
touch indexu.txt
touch filelistu.txt