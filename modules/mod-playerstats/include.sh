#!/usr/bin/env bash

PLAYERSTATS_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_CHARACTERS_CUSTOM_PATHS+=(
    $PLAYERSTATS_MODULE_PATH"/data/sql/db-chars/"
)
