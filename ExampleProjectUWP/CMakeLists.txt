add_compile_definitions(_UWP)
set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0)
add_executable(ExampleProjectUWP WIN32 "")
add_definitions(-DNOMINMAX)
target_sources(ExampleProjectUWP
    PRIVATE
        mainuwp.cpp
        DirectXEngineFrontendUWP.hpp
        DirectXEngineFrontendUWP.cpp
        DeviceResources.h
        DeviceResources.cpp
        pch.h
        pch.cpp
)
target_link_libraries(ExampleProjectUWP PUBLIC SpaceLion d3d11 dxgi dxguid d2d1)
install(TARGETS ExampleProjectUWP DESTINATION bin)