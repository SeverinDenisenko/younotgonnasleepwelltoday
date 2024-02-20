cmake_minimum_required(VERSION 3.15)
project(younotgonnasleepwelltoday)

set(CMAKE_C_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE "Release")

set(BOX2D_BUILD_TESTBED OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/raylib)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/box2d)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/fmt)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/* ${PROJECT_SOURCE_DIR}/src/engine/*)
add_executable(${PROJECT_NAME} ${SRC})

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PUBLIC "-Wall" "-Wextra" "-Werror" "-Wno-unused-private-field" "-fsanitize=address")
    target_link_options(${PROJECT_NAME} PUBLIC "-fsanitize=address")
else()
    target_compile_options(${PROJECT_NAME} PUBLIC "-O3" "-Wall" "-Wextra" "-Werror" "-Wno-unused-private-field" "-flto")
endif()

target_link_libraries(${PROJECT_NAME} raylib box2d fmt)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

