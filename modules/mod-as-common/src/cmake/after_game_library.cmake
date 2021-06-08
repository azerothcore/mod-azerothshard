CollectIncludeDirectories(
  ${CMAKE_SOURCE_DIR}/modules
  _PUBLIC_INCLUDES)

add_library(modules-interface INTERFACE)

target_include_directories(modules-interface
  INTERFACE
    ${_PUBLIC_INCLUDES})

target_link_libraries(modules-interface
  INTERFACE
    common)

CU_LOAD_INC_PATHS()

add_library(azth_mod STATIC
  ${CMAKE_AZTH_SRCS})

target_link_libraries(azth_mod
  PRIVATE
    modules-interface
    game-interface)

target_link_libraries(game-interface
  INTERFACE
    modules-interface)

target_link_libraries(game
  PRIVATE
    azth_mod)

