cmake_minimum_required(VERSION 3.15)
project(younotgonnasleepwelltoday)

add_subdirectory(${PROJECT_SOURCE_DIR}/deps/raylib)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*)
add_executable(${PROJECT_NAME} ${SRC})

target_compile_options(${PROJECT_NAME} PUBLIC "-Wall" "-Wextra" "-Werror")

target_link_libraries(${PROJECT_NAME} raylib)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

