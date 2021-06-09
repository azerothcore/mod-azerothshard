#AZTH_LOAD_SRC("libs")
AZTH_LOAD_SRC("mod-as-common/src/game")
AZTH_LOAD_SRC("mod-as-common/src/plugins")
AZTH_LOAD_SRC("mod-pvp-mode/src")

if( NOT WIN32 )
  message("defined: ${_AZTH_MOD_CONFIG}")
  if (NOT DEFINED _AZTH_MOD_CONFIG)
    set(_AZTH_MOD_CONFIG "${CONF_DIR}/azth_mod.conf")
  endif()
  message("mod_config: ${_AZTH_MOD_CONFIG}")

  add_definitions(-D_AZTH_MOD_CONFIG="\\"${_AZTH_MOD_CONFIG}\\"")

  unset(_AZTH_MOD_CONFIG CACHE)
endif()

CU_ADD_INC_PATH("${azth_DIRS}")

CU_LOAD_INC_PATHS()

set(CMAKE_AZTH_SRCS "${CMAKE_AZTH_SRCS}" "${azth_STAT_SRCS}")

CU_SET_CACHE("CMAKE_AZTH_SRCS" "${CMAKE_AZTH_SRCS}")
