project(combatris)

add_definitions(-DASSETS_FOLDER="${combatris_SOURCE_DIR}/assets/" )

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(combatris ${SourceFiles})

target_link_libraries(combatris ${SDL2_LIBRARY})
target_link_libraries(combatris ${SDL2_TTF_LIBRARIES})
target_link_libraries(combatris ${SDL2_MIXER_LIBRARIES})

# Build the test
include_directories(combatris src/)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

add_executable(combatris_test test/combatris_test.cpp)
add_dependencies(combatris_test catch)
target_link_libraries(combatris_test ${SDL2_LIBRARY})
