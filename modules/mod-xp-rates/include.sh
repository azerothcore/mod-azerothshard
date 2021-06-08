#!/usr/bin/env bash

XPRATES_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_CHARACTERS_CUSTOM_PATHS+=(
    $XPRATES_MODULE_PATH"/data/sql/db-chars/"
)