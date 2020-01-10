#!/bin/bash
cd ./src/main/assets/defaults
rm -f ./../index.txt
rm -f ./../filelist.txt
ls -R | grep ":" | sed -e 's/://' -e 's/\.//' -e 's/^\///' > index.txt
find -L | sed -e 's/://' -e 's/\.//' -e 's/^\///' > filelist.txt
mv ./index.txt ./../index.txt
mv ./filelist.txt ./../filelist.txt