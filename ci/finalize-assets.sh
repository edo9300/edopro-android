#!/usr/bin/env bash

ASSETS=src/main/assets/defaults
UPDATES=src/main/assets/update
ARCH=("armeabi-v7a" "arm64-v8a" "x86" "x86_64" )
OUTPUT=("libocgcorev7.so" "libocgcorev8.so" "libocgcorex86.so" "libocgcorex64.so")
mkdir -p $ASSETS
mkdir -p $UPDATES
for i in {0..3}; do
	CORE="deps/ocgcore/libs/${ARCH[i]}/libocgcore.so"
	if [[ -f "$CORE" ]]; then
		cp $CORE "$ASSETS/${OUTPUT[i]}"
		cp $CORE "$UPDATES/${OUTPUT[i]}"
	fi
done
cd $ASSETS
touch .nomedia
# Finds all directories, excluding the current directory, and removes the ./ prefix
find -L . ! -path . -type d | sed -E "s|^\./||" > ../index.txt
# Finds all files and directories, excluding the current directory, and removes the ./ prefix
find -L . ! -path . | sed -E "s|^\./||" > ../filelist.txt
cd ../update
touch .nomedia
# Finds all directories, excluding the current directory, and removes the ./ prefix
find -L . ! -path . -type d | sed -E "s|^\./||" > ../indexu.txt
# Finds all files and directories, excluding the current directory, and removes the ./ prefix
find -L . ! -path . | sed -E "s|^\./||" > ../filelistu.txt
