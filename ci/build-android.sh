#!/usr/bin/env bash

set -euxo pipefail

PROCS=""
if [[ "$OS_NAME" == "osx" ]]; then
    PROCS=$(sysctl -n hw.ncpu)
else
    PROCS=$(nproc)
fi

SDKMANAGER=$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager
echo y | $SDKMANAGER "ndk;21.4.7075529"
ln -sfn $ANDROID_HOME/ndk/21.4.7075529 $ANDROID_HOME/ndk-bundle
$ANDROID_HOME/ndk-bundle/ndk-build -j$PROCS
