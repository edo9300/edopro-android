#!/usr/bin/env bash

ASSETS=src/main/assets/defaults
UPDATES=src/main/assets/update
mkdir -p $ASSETS
mkdir -p $UPDATES
cd src/main/assets
touch index.txt
touch filelist.txt
touch indexu.txt
touch filelistu.txt