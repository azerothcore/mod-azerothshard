CU_ADD_HOOK(BEFORE_SRC_LOAD "${CMAKE_AZTH_SRC_DIR}/cmake/before_src_load.cmake")

CU_ADD_HOOK(AFTER_GAME_LIBRARY "${CMAKE_AZTH_SRC_DIR}/cmake/after_game_library.cmake")

AC_ADD_SCRIPT_LOADER("Azth" "AzthPlgLoader.h")
