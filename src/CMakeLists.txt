cmake_minimum_required(VERSION 3.8)

project(SolaceApp)

set(CMAKE_CXX_STANDARD 17)

find_package(libnabla REQUIRED)

include_directories(${LIBNABLA_INCLUDE_DIRS})

set(ASSEMBLER_SOURCES
    ${LIBNABLA_LIBRARIES}
)

add_custom_target(copy-solace-asm ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/solace_code ${CMAKE_BINARY_DIR}/solace_code
)

add_custom_target(copy-solace-project ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/solace_project ${CMAKE_BINARY_DIR}/solace_project
)

add_executable(solace
        main.cpp
)

target_link_libraries(solace
    PRIVATE 
        ${ASSEMBLER_SOURCES}
)