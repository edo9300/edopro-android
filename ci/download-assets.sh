#!/usr/bin/env bash

# Modified from Ignition

set -euo pipefail

ASSETS=src/main/assets/defaults

mkdir -p $ASSETS

# TODO: copy code licenses
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/edo9300/edopro/master/COPYING
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/edo9300/edopro/master/LICENSE

git clone https://$DEPLOY_TOKEN@github.com/ProjectIgnis/Distribution.git
cd Distribution
git submodule sync
git submodule update --init --recursive --force
cd ..

rsync -ar --exclude='.*' --exclude='*.sh' --exclude='script/.*' \
	--exclude='expansions/.*' --exclude='expansions/ci' --exclude='expansions/README.md' \
	--exclude=textures/Backup --exclude=puzzles \
	Distribution/ $ASSETS

curl --retry 5 --connect-timeout 30 --location --remote-header-name -o WindBotIgnite-Resources.7z $LIBWINDBOT_RESOURCES
7z x WindBotIgnite-Resources.7z -o$ASSETS

find $ASSETS \( -name '*.md' -o -name 'COPYING*' -o -name 'LICENSE*' \) -exec mv {} {}.txt \;
# Purge extra decks
mkdir -p $ASSETS/deck
find $ASSETS/deck ! \( -name 'Starter*' -o -name 'Structure*' \) -type f -exec rm {} \;
# Unsupported for release
rm -rf $ASSETS/config/*.json $ASSETS/config/languages/Thai
cp Distribution/config/configs.prod.json $ASSETS/config/configs.json
sed -i 's/dpi_scale = .*/dpi_scale = 2.000000/' $ASSETS/config/system.conf
