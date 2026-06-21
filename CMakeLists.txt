cmake_minimum_required(VERSION 3.30)

project(TrabalhoPC VERSION 1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/project/")

file(GLOB_RECURSE SOURCES 
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.c"
)

add_executable(trabalho_pc ${SOURCES})

target_compile_options(trabalho_pc PRIVATE 
    -Wall
    -Wextra
    -Werror
    -O3
    -march=native
)

target_include_directories(trabalho_pc PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/project/lib/")

target_link_libraries(trabalho_pc PRIVATE Threads::Threads)


set(SEPARATOR "===================================================================================================")
add_custom_command(TARGET trabalho_pc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ${SEPARATOR}
    COMMAND ${CMAKE_COMMAND} -E echo "Executável gerado em: $<TARGET_FILE:trabalho_pc>"
    COMMAND ${CMAKE_COMMAND} -E echo ${SEPARATOR}
)