CU_SET_PATH("CMAKE_AZTH_DIR" "${CMAKE_CURRENT_LIST_DIR}")
CU_SET_PATH("CMAKE_AZTH_SRC_DIR" "${CMAKE_CURRENT_LIST_DIR}/src/")

# Add config file
AC_ADD_CONFIG_FILE("${CMAKE_AZTH_DIR}/conf/azth_mod.conf.dist")

include("${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt")


