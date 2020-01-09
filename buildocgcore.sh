#!/bin/bash
cd deps/ocgcore
ndk-build -j
archs=("armeabi-v7a" "arm64-v8a" "x86" "x86_64" )
names=("libocgcorev7.so" "libocgcorev8.so" "libocgcorex86.so" "libocgcorex64.so")
mkdir -p ../../src/main/assets/defaults
for i in {0..3}; do
	FILE="libs/${archs[i]}/libocgcore.so"
	DEST="../../src/main/assets/defaults/${names[i]}"
	if test -f "$FILE"; then
		yes | cp -rf $FILE $DEST
	fi
done
