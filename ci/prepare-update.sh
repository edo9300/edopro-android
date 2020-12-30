#!/usr/bin/env bash

ASSETS=src/main/assets/defaults
UPDATES=src/main/assets/update
set -euo pipefail

JKS_PASS=${1:-$JKS_PASS}
BUILD_TOOLS_PATH=${BUILD_TOOLS_PATH:-$ANDROID_HOME/build-tools/29.0.3}

"$BUILD_TOOLS_PATH"/zipalign -v -p 4 build/outputs/apk/release/EDOPro-release-unsigned.apk build/outputs/apk/release/EDOPro-release-unsigned-aligned.apk
mkdir -p release
"$BUILD_TOOLS_PATH"/apksigner sign --ks keys.jks --ks-pass pass:"$JKS_PASS" --out release/EDOPro-release-full.apk build/outputs/apk/release/EDOPro-release-unsigned-aligned.apk

rm -rf build/outputs

rm -f index.txt
rm -f filelist.txt
cd $ASSETS
rm -rf *
# Finds all directories, excluding the current directory, and removes the ./ prefix
find -L . ! -path . -type d | sed -E "s|^\./||" > ../index.txt
# Finds all files and directories, excluding the current directory, and removes the ./ prefix
find -L . ! -path . | sed -E "s|^\./||" > ../filelist.txt
