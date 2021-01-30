#!/usr/bin/env bash

set -euo pipefail

MINIO_ENDPOINT=${1:-$MINIO_ENDPOINT}
MINIO_BUCKET=${2:-$MINIO_BUCKET}
MINIO_ACCESS_KEY=${3:-$MINIO_ACCESS_KEY}
MINIO_SECRET_KEY=${4:-$MINIO_SECRET_KEY}
MINIO_UPLOAD=${5:-$MINIO_UPLOAD}

# if [[ "$TRAVIS_OS_NAME " == "Linux" ]]; then
    curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://dl.min.io/client/mc/release/linux-amd64/mc
    chmod +x mc
    MC=./mc
# elif [[ "$TRAVIS_OS_NAME " == "macOS" ]]; then
    # MC=mc
# else
    # curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://dl.min.io/client/mc/release/windows-amd64/mc.exe
    # MC=./mc.exe
# fi

$MC config host add ignition "$MINIO_ENDPOINT" "$MINIO_ACCESS_KEY" "$MINIO_SECRET_KEY"
$MC mb -p "ignition/$MINIO_BUCKET"
$MC cp --recursive "$MINIO_UPLOAD" "ignition/$MINIO_BUCKET"
