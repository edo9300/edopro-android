#!/usr/bin/env bash

# Modified from Ignition

set -euo pipefail

ASSETS=src/main/assets/defaults
UPDATES=src/main/assets/update

mkdir -p $ASSETS
mkdir -p $UPDATES

# TODO: copy code licenses
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/edo9300/edopro/master/COPYING
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/edo9300/edopro/master/LICENSE

git clone https://$DEPLOY_TOKEN@github.com/ProjectIgnis/Distribution.git
cd Distribution
git checkout -qf $LATEST_REF
sed -i='' "s|https://github.com|https://$DEPLOY_TOKEN:$DEPLOY_TOKEN@github.com|" .gitmodules
git submodule sync
git submodule update --init --recursive --force
cd ..

curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/ProjectIgnis/Distribution/master/fonts/NotoSansJP-Regular.otf
lua ci/bin2c.lua "NotoSansJP-Regular.otf" "deps/gframe/CGUITTFont/bundled_font.cpp"

rsync -ar --exclude='.*' --exclude='*.sh' --exclude='script/.*' \
	--exclude='expansions/.*' --exclude='expansions/ci' --exclude='expansions/*rerelease*' --exclude='expansions/README.md' \
	--exclude='expansions/unofficial-fossil.cdb' --exclude='expansions/release.cdb' --exclude='script/pre-release' \
	--exclude=textures/Backup --exclude=puzzles \
	Distribution/ $ASSETS

cd Distribution

git diff -z --name-only --diff-filter=d $BASE_REF | xargs -0 -I {} \
	rsync -arR --exclude='.*' --exclude='*.sh'  --exclude='config/system.conf' \
		--exclude='expansions/**' --exclude='script/**' \
		--exclude=textures/Backup --exclude='puzzles/**' \
		{} ../$UPDATES/
	rsync -ar --exclude='.*' --exclude='*.sh' \
		--exclude='ci' --exclude='*rerelease*' --exclude='README.md' \
		--exclude='unofficial-fossil.cdb' --exclude='release.cdb' \
		./expansions/ ../$UPDATES/expansions
cd ..

curl --retry 5 --connect-timeout 30 --location --remote-header-name -o WindBotIgnite-Resources.7z $LIBWINDBOT_RESOURCES
7z x WindBotIgnite-Resources.7z -o$ASSETS
7z x WindBotIgnite-Resources.7z -o$UPDATES

find $ASSETS \( -name '*.md' -o -name 'COPYING*' -o -name 'LICENSE*' \) -exec mv {} {}.txt \;
# Purge extra decks
mkdir -p $ASSETS/deck
# Unsupported for release
rm -rf $ASSETS/config/*.json $ASSETS/config/languages/Thai
cp Distribution/config/configs.prod.json $ASSETS/config/configs.json
rm -rf $UPDATES/config/*.json $UPDATES/config/languages/Thai
cp Distribution/config/configs.prod.json $UPDATES/config/configs.json
sed -i 's/dpi_scale = .*/dpi_scale = 2.000000/' $ASSETS/config/system.conf
