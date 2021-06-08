#!/usr/bin/env bash

HEARTHSTONEMODE_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_EXTRA_CUSTOM_PATHS+=(
    $HEARTHSTONEMODE_MODULE_PATH"/data/sql/extra/"
)

DB_CHARACTERS_CUSTOM_PATHS+=(
    $HEARTHSTONEMODE_MODULE_PATH"/data/sql/chars/"
)

DB_WORLD_CUSTOM_PATHS+=(
    $HEARTHSTONEMODE_MODULE_PATH"/data/sql/world/"
)