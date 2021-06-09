#!/usr/bin/env bash

PVPMODE_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_EXTRA_CUSTOM_PATHS+=(
    $PVPMODE_MODULE_PATH"/data/sql/extra/"
)

DB_WORLD_CUSTOM_PATHS+=(
    $PVPMODE_MODULE_PATH"/data/sql/world"
)
