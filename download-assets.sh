#!/usr/bin/env bash

# Modified from Ignition

set -euo pipefail

ASSETS=src/main/assets/defaults

mkdir -p $ASSETS

# TODO: copy code licenses

git clone https://$DEPLOY_TOKEN@github.com/kevinlul/edopro-config.git
cd edopro-config
git submodule sync
git submodule update --init --recursive --force
cd ..

rsync -ar --exclude='.*' --exclude=update.sh --exclude='script/.*' \
	--exclude='expansions/.*' --exclude='expansions/ci' --exclude='expansions/README.md' \
	--exclude=textures/Backup --exclude=puzzles \
	edopro-config/ $ASSETS

mkdir -p $ASSETS/WindBot
curl --retry 5 --connect-timeout 30 --location --remote-header-name -o $ASSETS/WindBot/bots.json https://raw.githubusercontent.com/ProjectIgnis/windbot/master/bots.json

find $ASSETS \( -name '*.md' -o -name 'COPYING*' -o -name 'LICENSE*' \) -exec mv {} {}.txt \;
# Purge extra decks
mkdir -p $ASSETS/deck
find $ASSETS/deck ! \( -name 'Starter*' -o -name 'Structure*' \) -type f -exec rm {} \;
# Unsupported for release
rm -rf $ASSETS/config/*.json $ASSETS/config/languages/Deutsch $ASSETS/config/languages/Thai
cp edopro-config/config/configs.prod.json $ASSETS/config/configs.json
sed -i 's/dpi_scale = .*/dpi_scale = 2.000000/' $ASSETS/config/system.conf
