cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
project(cnetwork C)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC
        sqlite3)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION /usr/lib)
install(FILES ${HEADERS} DESTINATION /usr/include)
