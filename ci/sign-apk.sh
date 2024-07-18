#!/usr/bin/env bash

set -euo pipefail

OUT_APK=${1}
JKS_PASS=${2:-$JKS_PASS}
BUILD_TOOLS_PATH=${BUILD_TOOLS_PATH:-$ANDROID_HOME/build-tools/31.0.0}

"$BUILD_TOOLS_PATH"/zipalign -v -p 4 build/outputs/apk/release/EDOPro-release-unsigned.apk build/outputs/apk/release/EDOPro-release-unsigned-aligned.apk
mkdir -p release
"$BUILD_TOOLS_PATH"/apksigner sign --ks keys.jks --ks-pass pass:"$JKS_PASS" --out release/"$OUT_APK" build/outputs/apk/release/EDOPro-release-unsigned-aligned.apk
