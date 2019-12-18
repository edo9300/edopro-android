#!/bin/bash
cd ./src/main/assets/defaults
ls -R | grep ":" | sed -e 's/://' -e 's/\.//' -e 's/^\///' > index.txt
find -L | sed -e 's/://' -e 's/\.//' -e 's/^\///' > filelist.txt
mv ./index.txt ./../index.txt
mv ./filelist.txt ./../filelist.txt