#include <thread>
#include <future>

#include "InputEvent.hpp"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLEngineFrontend.hpp"
#include "OpenGL/ResourceManager.hpp"

#include "AtmosphereComponentManager.hpp"
#include "CameraComponent.hpp"
#include "gltfAssetComponentManager.hpp"
#include "MaterialComponentManager.hpp"
#include "MeshComponentManager.hpp"
#include "PointlightComponent.hpp"
#include "RenderTaskComponentManager.hpp"
#include "SunlightComponentManager.hpp"
#include "TransformComponentManager.hpp"
#include "TurntableComponentManager.hpp"

#include "../Editor/CameraController.hpp"


void createDemoScene(EngineCore::WorldState& world_state, EngineCore::Graphics::OpenGL::ResourceManager& resource_manager)
{
    auto& entity_mngr = world_state.accessEntityManager();
    auto& atmosphere_mngr = world_state.get<EngineCore::Graphics::AtmosphereComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
    auto& camera_mngr = world_state.get<EngineCore::Graphics::CameraComponentManager>();
    auto& gltf_mngr = world_state.get<EngineCore::Graphics::GltfAssetComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
    auto& mtl_mngr = world_state.get< EngineCore::Graphics::MaterialComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
    auto& mesh_mngr = world_state.get<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
    auto& pointlight_mngr = world_state.get<EngineCore::Graphics::PointlightComponentManager>();
    auto& rsrc_mngr = resource_manager;
    auto& renderTask_mngr = world_state.get<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>();
    auto& sunlight_mngr = world_state.get<EngineCore::Graphics::SunlightComponentManager>();
    auto& transform_mngr = world_state.get<EngineCore::Common::TransformComponentManager>();
    auto& turntable_mngr = world_state.get<EngineCore::Animation::TurntableComponentManager>();

    auto camera = entity_mngr.create();
    transform_mngr.addComponent(camera, Vec3(0.0, 0.0, 2.5));
    camera_mngr.addComponent(camera);
    camera_mngr.setActiveCamera(camera);

    Entity sun_entity = entity_mngr.create();
    transform_mngr.addComponent(sun_entity, Vec3(105783174465.5f, 105783174465.5f, 5783174465.5f), Quat(), Vec3(1.0f));
    sunlight_mngr.addComponent(sun_entity, Vec3(1.0f), 3.75f * std::pow(10.0f, 28.0f), 696342000.0f);

    Entity pointlight_entity = entity_mngr.create();
    transform_mngr.addComponent(pointlight_entity, Vec3(0.0f, 2.0f, 0.0f), Quat(), Vec3(1.0f));
    pointlight_mngr.addComponent(pointlight_entity, Vec3(1.0f), 2000.0f, 100.0f);

    Entity atmoshphere_entity = entity_mngr.create();
    //transform_mngr.addComponent(atmoshphere_entity, Vec3(0.0, -6361000.0, 0.0), Quat(), Vec3(6961000.0)); // check the correct size for the earth's atmosphere
    transform_mngr.addComponent(atmoshphere_entity, Vec3(0.0, -6360005.0, 0.0), Quat(), Vec3(6800000.0)); // check the correct size for the earth's atmosphere
    atmosphere_mngr.addComponent(atmoshphere_entity,
        //Vec3(0.0058,0.0135,0.0331),
        //Vec3(0.00444,0.00444,0.00444),
        Vec3(0.0000058, 0.0000135, 0.0000331),
        //Vec3(0.00000444, 0.00000444, 0.00000444),
        Vec3(0.000003, 0.000003, 0.000003),
        //Vec3(0.00002,0.00002,0.00002),
        8000.0,
        1200.0,
        6360000.0,
        //6359800.0,
        6420000.0);

    // Create "no-texture" debugging textures
    {
        GenericTextureLayout layout;

        layout.width = 1;
        layout.height = 1;
        layout.depth = 1;
        layout.levels = 1;
        layout.type = 0x1401; // GL_UNSIGNED_BYTE;
        layout.format = 0x1908; // GL_RGBA, apparently tinygltf enforces 4 components for better vulkan compability anyway
        layout.internal_format = 0x8058;// GL_RGBA8

        auto APIlayout = resource_manager.convertGenericTextureLayout(layout);

        auto tx_data = std::make_shared<std::array<uint8_t, 4>>();
        (*tx_data) = { 255,0,255,255 };

        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "noTexture_baseColor",
            APIlayout,
            tx_data);
    }

    {
        GenericTextureLayout layout;
    
        layout.width = 1;
        layout.height = 1;
        layout.depth = 1;
        layout.levels = 1;
        layout.type = 0x1401; // GL_UNSIGNED_BYTE;
        layout.format = 0x1908; // GL_RGBA, apparently tinygltf enforces 4 components for better vulkan compability anyway
        layout.internal_format = 0x8058;// GL_RGBA8
    
        auto APIlayout = resource_manager.convertGenericTextureLayout(layout);
    
        auto tx_data = std::make_shared<std::array<uint8_t, 4>>();
        (*tx_data) = { 255,255,0,255 };
    
        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "noTexture_metallicRoughness",
            APIlayout,
            tx_data);
    }
    
    {
        GenericTextureLayout layout;
    
        layout.width = 1;
        layout.height = 1;
        layout.depth = 1;
        layout.levels = 1;
        layout.type = 0x1401; // GL_UNSIGNED_BYTE;
        layout.format = 0x1908; // GL_RGBA, apparently tinygltf enforces 4 components for better vulkan compability anyway
        layout.internal_format = 0x8058;// GL_RGBA8
    
        auto APIlayout = resource_manager.convertGenericTextureLayout(layout);
    
        auto tx_data = std::make_shared<std::array<uint8_t, 4>>();
        (*tx_data) = { 127,127,255,255 };
    
        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "noTexture_normalMap",
            APIlayout,
            tx_data);
    }

    auto shader_names = std::make_shared<std::vector<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>>(
        std::initializer_list<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>{
            //{ "../space-lion/resources/shaders/simple_forward_vert.glsl", glowl::GLSLProgram::ShaderType::Vertex },
            //{ "../space-lion/resources/shaders/fwd_pbrMetallic_f.glsl", glowl::GLSLProgram::ShaderType::Fragment }
            { "../space-lion/resources/shaders/simple_forward_vert.glsl", glowl::GLSLProgram::ShaderType::Vertex },
            { "../space-lion/resources/shaders/dfr_geomPass_f.glsl", glowl::GLSLProgram::ShaderType::Fragment }
    });

    auto shader_rsrc = rsrc_mngr.createShaderProgramAsync(
        "debug_forward_shader",
        shader_names
    );

    auto skinned_mesh_shader_names = std::make_shared<std::vector<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>>(
        std::initializer_list<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>{
            { "../space-lion/resources/shaders/skinned_mesh_notx_v.glsl", glowl::GLSLProgram::ShaderType::Vertex },
            { "../space-lion/resources/shaders/skinned_mesh_notx_f.glsl", glowl::GLSLProgram::ShaderType::Fragment }
    });

    auto skinned_mesh_shader_rsrc = rsrc_mngr.createShaderProgramAsync(
        "skinned_mesh_shader",
        skinned_mesh_shader_names
    );

    //for (int x = -1; x <= 1; ++x)
    //{
    //    for (int y = -1; y <= 1; ++y)
    //    {
    //        for (int z = -1; z <= 1; ++z)
    //        {
    //            auto gltf_root = entity_mngr.create();
    //            transform_mngr.addComponent(gltf_root, Vec3(x, y, z));
    //            turntable_mngr.addComponent(gltf_root, 0.5f);
    //
    //            //{
    //            //    auto gltf_subobj = entity_mngr.create();
    //            //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //            //    transform_mngr.setParent(transform_idx, gltf_root);
    //            //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf", "Avocado", shader_rsrc);
    //            //}
    //
    //            {
    //                auto gltf_subobj = entity_mngr.create();
    //                size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //                transform_mngr.setParent(transform_idx, gltf_root);
    //                gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "Hose_low", shader_rsrc);
    //            }
    //            
    //            {
    //                auto gltf_subobj = entity_mngr.create();
    //                size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //                transform_mngr.setParent(transform_idx, gltf_root);
    //                gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "RubberWood_low", shader_rsrc);
    //            }
    //            
    //            {
    //                auto gltf_subobj = entity_mngr.create();
    //                size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //                transform_mngr.setParent(transform_idx, gltf_root);
    //                gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "GlassPlastic_low", shader_rsrc);
    //            }
    //            
    //            {
    //                auto gltf_subobj = entity_mngr.create();
    //                size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //                transform_mngr.setParent(transform_idx, gltf_root);
    //                gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "MetalParts_low", shader_rsrc);
    //            }
    //            
    //            {
    //                auto gltf_subobj = entity_mngr.create();
    //                size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //                transform_mngr.setParent(transform_idx, gltf_root);
    //                gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "LeatherParts_low", shader_rsrc);
    //            }
    //            
    //            {
    //                auto gltf_subobj = entity_mngr.create();
    //                size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //                transform_mngr.setParent(transform_idx, gltf_root);
    //                gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "Lenses_low", shader_rsrc);
    //            }
    //        }
    //    }
    //}

    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/Main/NewSponza_Main_Blender_glTF.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/OrientationTest/glTF/OrientationTest.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/RiggedFigure/glTF/RiggedFigure.gltf", skinned_mesh_shader_rsrc);
    gltf_mngr.importGltfScene("C:/Users/micha/Downloads/potted_plant_01_4k.gltf/potted_plant_01_4k.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/carved_wooden_elephant_1k.blend/carved_wooden_elephant_1k.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/wine_barrel_01_4k.gltf/wine_barrel_01_4k.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/brass_vase_04_4k.gltf/brass_vase_04_4k.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/korean_public_payphone_01_8k.gltf/korean_public_payphone_01_8k.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/alarm_clock_01_4k.gltf/alarm_clock_01_4k.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/CherryBlossoms/CherryBlossoms.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/pz_iv_g.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/JAPANESE_PACK_FINAL/temple_main.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/CG-Scene/the_scene/scene.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/CesiumMan/glTF/CesiumMan.gltf", skinned_mesh_shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/rungholt/rungholt.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/scifi_girl_v.01/scene.gltf", shader_rsrc);

    //gltf_mngr.clearModelCache();
}


struct App {
    App() : m_active_window(nullptr), m_window_width(0), m_window_height(0), m_engine_frontend(std::make_unique<OpenGLEngineFrontend>())
    {
        std::lock_guard<std::mutex> lk(m_window_mutex);

        // Initialize GLFW
        if (!glfwInit())
        {
            std::cout << "-----\n"
                << "The time is out of joint - O cursed spite,\n"
                << "That ever I was born to set it right!\n"
                << "-----\n"
                << "Error: Couldn't initialize glfw.";
        }
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#if EDITOR_MODE
        m_active_window = glfwCreateWindow(1600, 900, "Space-Lion", NULL, NULL);
        m_window_width = 1600;
        m_window_height = 900;
        //m_active_window = glfwCreateWindow(1920, 1080, "Space-Lion", glfwGetPrimaryMonitor(), NULL);
#else
        //m_active_window = glfwCreateWindow(1920, 1080, "Space-Lion", glfwGetPrimaryMonitor(), NULL);
        m_active_window = glfwCreateWindow(1280, 720, "Space-Lion", NULL, NULL);
        m_window_width = 1280;
        m_window_height = 720;
#endif

        if (!m_active_window)
        {
            std::cout << "-----\n"
                << "The time is out of joint - O cursed spite,\n"
                << "That ever I was born to set it right!\n"
                << "-----\n"
                << "Error: Couldn't open glfw window";

            glfwTerminate();
        }

        glfwMakeContextCurrent(m_active_window);

        //glfwSwapInterval(1);

        // Get context version information
        int major = glfwGetWindowAttrib(m_active_window, GLFW_CONTEXT_VERSION_MAJOR);
        int minor = glfwGetWindowAttrib(m_active_window, GLFW_CONTEXT_VERSION_MINOR);

        std::cout << "OpenGL context " << major << "." << minor << std::endl;

        // Register callback functions
        glfwSetWindowSizeCallback(m_active_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_active_window, windowCloseCallback);
        glfwSetKeyCallback(m_active_window, keyCallback);

        glfwSetWindowUserPointer(m_active_window, this);

        // Initialize glad
        if (!gladLoadGL()) {
            std::cout << "-----\n"
                << "The time is out of joint - O cursed spite,\n"
                << "That ever I was born to set it right!\n"
                << "-----\n"
                << "Error during gladLoadGL.\n";
            exit(-1);
        }
        std::cout << "OpenGL %d.%d\n" << GLVersion.major << " " << GLVersion.minor;

        assert((glGetError() == GL_NO_ERROR));

        //glDebugMessageCallback(opengl_debug_message_callback, NULL);
    }

    void run()
    {
        auto perf_cruncher = [this](std::shared_future<void> render_exec)
        {
            std::cout << "  thread_id : " << std::this_thread::get_id() << "\n";

            while (true) {
                auto& world_state = m_engine_frontend->accessWorldState();
                auto& resource_manager = m_engine_frontend->accessResourceManager();
                auto& frame_manager = m_engine_frontend->accessFrameManager();
                
                auto& entity_mngr = world_state.accessEntityManager();
                auto const& camera_mngr = world_state.get<EngineCore::Graphics::CameraComponentManager>();
                auto& mtl_mngr = world_state.get<EngineCore::Graphics::MaterialComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
                auto& mesh_mngr = world_state.get<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
                auto& renderTask_mngr = world_state.get<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>();
                auto const& transform_mngr = world_state.get<EngineCore::Common::TransformComponentManager>();
                auto& turntable_mngr = world_state.get<EngineCore::Animation::TurntableComponentManager>();
                
                //auto cam = camera_mngr.getActiveCamera();

                //int i = 23 + 23;
                //int j = i + 12;
                //
                //std::cout << j << std::endl;
            }
        };

        auto game_update_loop = [this](std::shared_future<void> render_exec)
        {
            std::cout << "  thread_id : " << std::this_thread::get_id() << "\n";

            // inplace construct an input action context to test the new concept
            auto evt_func = [](EngineCore::Common::Input::Event const& evt, EngineCore::Common::Input::HardwareState const& state) {
                std::cout << "Paying respect to new input system" << "\n";
            };
            EngineCore::Common::Input::EventDrivenAction evt_action = { {EngineCore::Common::Input::Device::KEYBOARD,EngineCore::Common::Input::KeyboardKeys::KEY_F,EngineCore::Common::Input::EventTrigger::PRESS}, evt_func };
            EngineCore::Common::Input::InputActionContext input_context = { "test_input_context", true, {evt_action}, {} };
            m_input_action_contexts.push_back(input_context);

            auto& world_state = m_engine_frontend->accessWorldState();
            auto& resource_manager = m_engine_frontend->accessResourceManager();
            auto& frame_manager = m_engine_frontend->accessFrameManager();

            Editor::Controls::CameraController cam_ctrl(world_state, frame_manager);
            m_input_action_contexts.push_back(cam_ctrl.getInputActionContext());
            //m_engine_frontend->addInputActionContext(cam_ctrl.getInputActionContext());
            createDemoScene(world_state, resource_manager);

            while (render_exec.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
                // do nothing so far...
            }
        };

        auto engine_update_loop = [this](std::shared_future<void> render_exec)
        {
            std::cout << "  thread_id : " << std::this_thread::get_id() << "\n";

            size_t update_frameID = 0;
            auto t_0 = std::chrono::high_resolution_clock::now();
            auto t_1 = std::chrono::high_resolution_clock::now();

            auto render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));
            while (render_exec_status != std::future_status::ready)
            {
                double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t_1 - t_0).count();
                t_0 = std::chrono::high_resolution_clock::now();

                auto window_resolution = this->getWindowResolution();

                m_engine_frontend->update(update_frameID++, dt, std::get<0>(window_resolution), std::get<1>(window_resolution));

                render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));

                t_1 = std::chrono::high_resolution_clock::now();
            }
        };

        auto engine_render_loop = [this]()
        {
            std::cout << "  thread_id : " << std::this_thread::get_id() << "\n";

            // Init ImGui, set install_callbacks to false cause I call them myself
                //IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            if (!ImGui_ImplGlfw_InitForOpenGL(m_active_window, false))
                std::cerr << "Error during imgui init " << std::endl;
            ImGui_ImplOpenGL3_Init("#version 450");

            size_t render_frameID = 0;
            double t0, t1 = 0.0;

            while (!glfwWindowShouldClose(m_active_window))
            {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                int width, height;
                glfwGetFramebufferSize(m_active_window, &width, &height);

                t0 = t1;
                t1 = glfwGetTime();
                double dt = t1 - t0;

                // Get current frame for rendering
                auto& frame = m_engine_frontend->accessFrameManager().getRenderFrame();
                frame.m_render_frameID = render_frameID;
                frame.m_render_dt = dt;

                processInputActions();

                m_engine_frontend->render(render_frameID++, dt, width, height);

                ImGui::SetNextWindowPos(ImVec2(width - 375.0f, height - 100.0f));
                bool p_open = true;
                if (!ImGui::Begin("FPS", &p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
                {
                    ImGui::End();
                    return;
                }
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                glfwSwapBuffers(m_active_window);
                glfwPollEvents();
            }

            // TODO some sort of exit call to frontend, e.g. to delete GPU resources
        };

        // Deferred starting and running of rendering on current thread (window and context usually live here)
        auto engine_render_exec = std::async(std::launch::deferred, engine_render_loop).share();

        auto perf_crunch_0 = std::async(std::launch::async, perf_cruncher, engine_render_exec);
        auto perf_crunch_1 = std::async(std::launch::async, perf_cruncher, engine_render_exec);
        auto perf_crunch_2 = std::async(std::launch::async, perf_cruncher, engine_render_exec);

        // Start and run engine update in seperate thread
        auto engine_update_exec = std::async(std::launch::async, engine_update_loop, engine_render_exec);

        // Start "game"
        auto game_update_exec = std::async(std::launch::async, game_update_loop, engine_render_exec);

        // trigger deferred start of render loop
        engine_render_exec.get();
    }

    std::pair<int, int> getWindowResolution() {
        std::lock_guard<std::mutex> lk(m_window_mutex);
        return { m_window_width,m_window_height };
    }

    void processInputActions()
    {
        for (auto& input_context : m_input_action_contexts)
        {
            if (input_context.m_is_active)
            {
                for (auto& state_action : input_context.m_state_actions)
                {
                    std::vector<EngineCore::Common::Input::HardwareState> states;

                    for (auto& part : state_action.m_state_query)
                    {
                        if (std::get<0>(part) == EngineCore::Common::Input::Device::KEYBOARD)
                        {
                            states.emplace_back(glfwGetKey(m_active_window, std::get<1>(part)) == GLFW_PRESS ? 1.0f : 0.0);
                        }
                        else if (std::get<0>(part) == EngineCore::Common::Input::Device::MOUSE_AXES)
                        {
                            double x, y;
                            glfwGetCursorPos(m_active_window, &x, &y);

                            if (std::get<1>(part) == EngineCore::Common::Input::MouseAxes::MOUSE_CURSOR_X)
                            {
                                states.emplace_back(x);
                            }
                            else if (std::get<1>(part) == EngineCore::Common::Input::MouseAxes::MOUSE_CURSOR_Y)
                            {
                                states.emplace_back(y);
                            }
                        }
                        else if (std::get<0>(part) == EngineCore::Common::Input::Device::MOUSE_BUTTON)
                        {
                            if (std::get<1>(part) == EngineCore::Common::Input::MouseButtons::MOUSE_BUTTON_RIGHT)
                            {
                                states.emplace_back(glfwGetMouseButton(m_active_window, std::get<1>(part)) == GLFW_PRESS ? 1.0f : 0.0f);
                            }
                        }
                    }

                    state_action.m_action(state_action.m_state_query, states);

                }
            }
        }
    }

    void addInputActionContext(EngineCore::Common::Input::InputActionContext const& context);

    void removeInputActionContext(std::string const& context_name);

    void setInputActionContextActive(std::string const& context_name);

    void setInputActionContextInactive(std::string const& context_name);

    /** Pointer to active window */
    GLFWwindow* m_active_window;
    int m_window_width;
    int m_window_height;

    std::mutex m_window_mutex;
    //std::condition_variable m_winodw_cVar;

    /** List of input contexts */
    std::list<EngineCore::Common::Input::InputActionContext> m_input_action_contexts;

    /** Frontend for space lion engine */
    std::unique_ptr<OpenGLEngineFrontend> m_engine_frontend;

    /**************************************************************************
     * (Static) callbacks functions
     *************************************************************************/

    static void windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));

        std::lock_guard<std::mutex> lk(app->m_window_mutex);
        app->m_window_width = width;
        app->m_window_height = height;
    }

    static void windowCloseCallback(GLFWwindow* window) {

    }

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {

    }

    static void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos)
    {

    }

    static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {

    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));

        for (auto& input_context : app->m_input_action_contexts)
        {
            if (input_context.m_is_active)
            {
                for (auto& event_action : input_context.m_event_actions)
                {
                    if (std::get<0>(event_action.m_event) == EngineCore::Common::Input::Device::KEYBOARD &&
                        std::get<1>(event_action.m_event) == key &&
                        std::get<2>(event_action.m_event) == action
                        )
                    {
                        event_action.m_action(event_action.m_event,/*TODO map action to meanigful state?*/1.0);
                    }
                }
            }
        }
    }
};


int main() {

    App app;
    app.run();

    return 0;
}