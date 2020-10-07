project(combatris)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
include_directories(combatris src/)

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(combatris ${SourceFiles})

target_link_libraries(combatris ${SDL2_LIBRARY})
target_link_libraries(combatris ${SDL2_TTF_LIBRARIES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(combatris -lc++)

  if (UNIX)
    target_link_libraries(combatris -lm)
  endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(combatris -lstdc++)
  target_link_libraries(combatris -lm)
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET combatris PROPERTY CXX_STANDARD 17)
endif()

# Build the test
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

file(GLOB_RECURSE SourceFiles src/game/* src/utility/*.cpp src/network/*.cpp test/*.cpp)

add_executable(combatris_test ${SourceFiles})
add_dependencies(combatris_test catch)

target_link_libraries(combatris_test ${SDL2_LIBRARY})
target_link_libraries(combatris_test ${SDL2_TTF_LIBRARIES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(combatris_test -lc++)
  if (UNIX)
    target_link_libraries(combatris_test -lm)
  endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(combatris_test -lstdc++)
  target_link_libraries(combatris_test -lm)
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET combatris_test PROPERTY CXX_STANDARD 17)
endif()
