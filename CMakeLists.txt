CU_SET_PATH("CMAKE_AZTH_DIR" "${CMAKE_CURRENT_LIST_DIR}")

MACRO(AZTH_LOAD_SRC dir)
    CU_SUBDIRLIST(sub_DIRS  "${CMAKE_AZTH_DIR}/modules/${dir}" TRUE TRUE)
    FOREACH(subdir ${sub_DIRS})
        get_filename_component(azth_dir_name ${subdir} DIRECTORY)

        if (NOT (${azth_dir_name} STREQUAL "tests") OR AZTH_WITH_UNIT_TEST)
            file(GLOB sources "${subdir}/*.cpp" "${subdir}/*.h")

            set(azth_STAT_SRCS
                ${azth_STAT_SRCS}
                ${sources}
            )

            set(azth_DIRS
                ${subdir}
                ${azth_DIRS}
            )
        endif()
    ENDFOREACH()
ENDMACRO()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-as-platform")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-as-common")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-arena-solo-3v3")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-guildhouse")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-hearthstone-mode")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-playerstats")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-challenge-mode")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-pvpitemlevel")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-pvp-mode")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-smartstone")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-timewalking")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-tournament")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/modules/mod-xp-rates")

AC_ADD_SCRIPTS("${CMAKE_CURRENT_LIST_DIR}/src/")


# Add config file
AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/mod-as-common/conf/azth_mod.conf.dist")
