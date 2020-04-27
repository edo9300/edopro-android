#!/usr/bin/env bash

set -euo pipefail

JKS_PASS=${1:-JKS_PASS}

zipalign -v -p 4 build/outputs/apk/release/EDOPro-release-unsigned.apk build/outputs/apk/release/EDOPro-release-unsigned-aligned.apk
mkdir -p release
apksigner sign --ks keys.jks --ks-pass pass:"$JKS_PASS" --out release/EDOPro-release.apk build/outputs/apk/release/EDOPro-release-unsigned-aligned.apk
