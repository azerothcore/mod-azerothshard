#!/usr/bin/env bash

BGITEMLVL_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_WORLD_CUSTOM_PATHS+=(
    $BGITEMLVL_MODULE_PATH"/data/sql/world/"
)

DB_CHARACTERS_CUSTOM_PATHS+=(
    $BGITEMLVL_MODULE_PATH"/data/sql/characters/"
)