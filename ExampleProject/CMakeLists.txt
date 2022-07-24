
add_executable(ExampleProject "")
target_sources(ExampleProject
    PRIVATE
        main.cpp
        OpenGLEngineFrontend.hpp
        OpenGLEngineFrontend.cpp
)
set_property(TARGET ExampleProject PROPERTY OUTPUT_NAME "ExampleProject")
get_target_property(executable_name ExampleProject OUTPUT_NAME)
set_property(TARGET ExampleProject PROPERTY VS_DEBUGGER_COMMAND "${PROJECT_SOURCE_DIR}/bin/${executable_name}.exe")
set_property(TARGET ExampleProject PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")
target_link_libraries(ExampleProject PUBLIC SpaceLion)
install(TARGETS ExampleProject DESTINATION bin)