cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
project(server C)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cserver)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/chttp)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE
        pthread
        cserver
        cargs
        chttp)