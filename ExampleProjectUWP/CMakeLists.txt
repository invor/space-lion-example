add_compile_definitions(_UWP)
set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0)
add_executable(ExampleProjectUWP WIN32 "")
add_definitions(-DNOMINMAX)

file(GLOB flighthelmet-rsrc
     "resources/FlightHelmet/*"
)

set(ResourceFiles
# resources/Avocado/Avocado.bin
#  resources/Avocado/Avocado.gltf
#  resources/Avocado/Avocado_baseColor.png
#  resources/Avocado/Avocado_normal.png
#  resources/Avocado/Avocado_roughnessMetallic.png
#  ${flighthelmet-rsrc}
  )

set_property(SOURCE ${ResourceFiles} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property(SOURCE ${ResourceFiles} PROPERTY VS_DEPLOYMENT_LOCATION "Resources")

source_group("Resources" FILES ${ResourceFiles})

target_sources(ExampleProjectUWP
    PRIVATE
        mainuwp.cpp
        DirectXEngineFrontendUWP.hpp
        DirectXEngineFrontendUWP.cpp
        DeviceResources.h
        DeviceResources.cpp
        pch.h
        pch.cpp
        imgui_impl_uwp.h
        imgui_impl_uwp.cpp
        ImPlotOffscreenRenderPass.h
        ImPlotOffscreenRenderPass.cpp
        DemoScene.hpp
        DemoScene.cpp
        platformUtility.hpp
        ${ResourceFiles}
)
target_link_libraries(ExampleProjectUWP PUBLIC SpaceLion d3d11 dxgi dxguid d2d1)
install(TARGETS ExampleProjectUWP DESTINATION bin)
