#
# Copyright (С) since 2019 Andrei Guluaev (Winfidonarleyan/Kargatum) https://github.com/Winfidonarleyan
#

# Set global dir
CU_SET_PATH("MOD_AZTH_DIR" "${CMAKE_CURRENT_LIST_DIR}")

# Add includes in game-interface lib
CU_ADD_HOOK(AFTER_GAME_LIBRARY "${MOD_AZTH_DIR}/cmake/TargetInclude.cmake")

# Add cpp files
AC_ADD_SCRIPT("${MOD_AZTH_DIR}/src/AZTH.cpp")
AC_ADD_SCRIPT("${MOD_AZTH_DIR}/src/AZTH.h")
AC_ADD_SCRIPT("${MOD_AZTH_DIR}/src/AZTH_SC.cpp")

# Add scripts to script loader
#AC_ADD_SCRIPT_LOADER("AZTH" "${MOD_AZTH_DIR}/src/AZTHLoad.h")

# Add config file
# AC_ADD_CONFIG_FILE("${MOD_AZTH_DIR}/conf/AZTH.conf.dist")
