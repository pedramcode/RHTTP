cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
project(rhttp C)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/csocket)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/chttp)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/credis)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cnetwork)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/ctime)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/chttpmsg)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE
        pthread
        csocket
        cargs
        credis
        cnetwork
        ctime
        chttpmsg
        chttp)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
