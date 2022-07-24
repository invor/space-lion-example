add_compile_definitions(_UWP)
set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0)
add_executable(ExampleProjectUWP WIN32 "")
target_sources(ExampleProjectUWP
    PRIVATE
        mainuwp.cpp
        DirectXEngineFrontendUWP.hpp
        DirectXEngineFrontendUWP.cpp
)
target_link_libraries(ExampleProjectUWP PUBLIC SpaceLion)
install(TARGETS ExampleProjectUWP DESTINATION bin)