#!/usr/bin/env bash

GUILDHOUSE_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_EXTRA_CUSTOM_PATHS+=(
    $GUILDHOUSE_MODULE_PATH"/data/sql/db-extra/"
)

DB_CHARACTERS_CUSTOM_PATHS+=(
    $GUILDHOUSE_MODULE_PATH"/data/sql/db-char/"
)