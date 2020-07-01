cmake_minimum_required(VERSION 3.16)

project(space-lion-example)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(space-lion)

add_executable(ExampleProject "")

target_sources(ExampleProject
    PRIVATE
        src/main.cpp
)

target_link_libraries(ExampleProject PUBLIC SpaceLion)

SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD TRUE)
install(TARGETS ExampleProject DESTINATION bin)