#include "OpenGLEngineFrontend.hpp"

#include <thread>
#include <vector>
#include <future>
#include <chrono>

#include "AirplanePhysicsComponent.hpp"
#include "AtmosphereComponentManager.hpp"
#include "AnimationSystems.hpp"
#include "CameraComponent.hpp"
#include "OpenGL/BasicRenderingPipeline.hpp"
#include "GeometryBakery.hpp"
#include "gltfAssetComponentManager.hpp"
//#include "LandscapeFeatureCurveComponent.hpp"
//#include "LandscapeBrickComponent.hpp"
#include "MeshComponentManager.hpp"
#include "NameComponentManager.hpp"
#include "OceanComponent.hpp"
#include "PointlightComponent.hpp"
#include "RenderTaskComponentManager.hpp"
#include "SunlightComponentManager.hpp"
#include "SkinComponentManager.hpp"
#include "TaskSchedueler.hpp"

#include "InputEvent.hpp"

#include <GLFW/glfw3.h>


        OpenGLEngineFrontend::OpenGLEngineFrontend()
            : m_engine_started(false),
            m_task_schedueler(std::make_unique<EngineCore::Utility::TaskSchedueler>()),
            m_frame_manager(std::make_unique<EngineCore::Common::FrameManager<EngineCore::Common::Frame>>()),
            m_graphics_backend(std::make_unique<EngineCore::Graphics::OpenGL::GraphicsBackend>()),
            m_resource_manager(std::make_unique<EngineCore::Graphics::OpenGL::ResourceManager>()),
            m_world_state(std::make_unique<EngineCore::WorldState>())
        {
            m_world_state->add<EngineCore::Physics::AirplanePhysicsComponentManager>(std::make_unique<EngineCore::Physics::AirplanePhysicsComponentManager>(128, *m_world_state.get()));
            m_world_state->add<EngineCore::Graphics::AtmosphereComponentManager>(std::make_unique<EngineCore::Graphics::AtmosphereComponentManager>(8));
            m_world_state->add<EngineCore::Graphics::CameraComponentManager>(std::make_unique<EngineCore::Graphics::CameraComponentManager>(8));
            m_world_state->add<EngineCore::Graphics::GltfAssetComponentManager>(std::make_unique<EngineCore::Graphics::GltfAssetComponentManager>());
            m_world_state->add<EngineCore::Graphics::MaterialComponentManager>(std::make_unique<EngineCore::Graphics::MaterialComponentManager>());
            m_world_state->add<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(std::make_unique<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(m_resource_manager.get()));
            m_world_state->add<EngineCore::Common::NameComponentManager>(std::make_unique<EngineCore::Common::NameComponentManager>());
            m_world_state->add<OceanComponentManager>(std::make_unique<OceanComponentManager>());
            m_world_state->add<EngineCore::Graphics::PointlightComponentManager>(std::make_unique<EngineCore::Graphics::PointlightComponentManager>(16000));
            m_world_state->add<EngineCore::Graphics::SunlightComponentManager>(std::make_unique<EngineCore::Graphics::SunlightComponentManager>(1));
            m_world_state->add<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>(std::make_unique<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>());
            m_world_state->add<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::SkinnedMesh>>(std::make_unique<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::SkinnedMesh>>());
            //m_world_state->add<EngineCore::Common::TransformComponentManager>(std::make_unique<EngineCore::Common::TransformComponentManager>(5000000));
            m_world_state->add<EngineCore::Common::TransformComponentManager>(std::make_unique<EngineCore::Common::TransformComponentManager>());
            m_world_state->add<EngineCore::Animation::TurntableComponentManager>(std::make_unique<EngineCore::Animation::TurntableComponentManager>());
            m_world_state->add<EngineCore::Animation::SkinComponentManager>(std::make_unique<EngineCore::Animation::SkinComponentManager>());
            //m_world_state->add<EngineCore::Graphics::Landscape::FeatureCurveComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(std::make_unique<EngineCore::Graphics::Landscape::FeatureCurveComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(*m_world_state.get(), *m_resource_manager.get()));

            m_world_state->add([](EngineCore::WorldState& world_state, double dt, EngineCore::Utility::TaskSchedueler& task_schedueler) {
                    auto& transform_mngr = world_state.get<EngineCore::Common::TransformComponentManager>();
                    auto& turntable_mngr = world_state.get<EngineCore::Animation::TurntableComponentManager>();
                    EngineCore::Animation::animateTurntables(transform_mngr,turntable_mngr,dt,task_schedueler);
                }
            );

            // start task schedueler with 1 thread
            m_task_schedueler->run(3);
        }

        OpenGLEngineFrontend::~OpenGLEngineFrontend()
        {
            m_task_schedueler->stop();
        }

        void OpenGLEngineFrontend::startEngine()
        {
            // create initial frame
            size_t frameID = 0;

            // start rendering pipeline
            //std::thread render_thread(&(DeferredRenderingPipeline::run), &GEngineCore::renderingPipeline()));
            auto render_exec = std::async(std::launch::async, &(EngineCore::Graphics::OpenGL::GraphicsBackend::run), m_graphics_backend.get(), m_resource_manager.get(), m_frame_manager.get());
            auto render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));

            //auto render_exec = std::async(std::launch::async, &(GraphicsBackend::run), &GEngineCore::graphicsBackend());
            //auto render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));

            // start task schedueler with 1 thread
            //m_task_schedueler->run(1);

            auto t_0 = std::chrono::high_resolution_clock::now();
            auto t_1 = std::chrono::high_resolution_clock::now();

            auto t_2 = std::chrono::high_resolution_clock::now();
            auto t_3 = std::chrono::high_resolution_clock::now();

            auto& entity_mngr     = m_world_state->accessEntityManager();
            auto& camera_mngr     = m_world_state->get<EngineCore::Graphics::CameraComponentManager>();
            auto& mtl_mngr        = m_world_state->get<EngineCore::Graphics::MaterialComponentManager>();
            auto& mesh_mngr       = m_world_state->get<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>();
            auto& rsrc_mngr       = (*m_resource_manager);
            auto& renderTask_mngr = m_world_state->get<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>();
            auto& transform_mngr  = m_world_state->get<EngineCore::Common::TransformComponentManager>();
            auto& turntable_mngr  = m_world_state->get<EngineCore::Animation::TurntableComponentManager>();

            // inplace construct an input action context to test the new concept
            auto evt_func = [&camera_mngr,&transform_mngr](EngineCore::Common::Input::Event const& evt, EngineCore::Common::Input::HardwareState const& state) {
                std::cout << "Paying respect to new input system"<<"\n";
            };
            EngineCore::Common::Input::EventDrivenAction evt_action = { {EngineCore::Common::Input::Device::KEYBOARD,EngineCore::Common::Input::KeyboardKeys::KEY_F,EngineCore::Common::Input::EventTrigger::PRESS}, evt_func };
            EngineCore::Common::Input::InputActionContext input_context = { "test_input_context", true, {evt_action}, {} };
            m_graphics_backend->addInputActionContext(input_context);

            // wait for window creation
            m_graphics_backend->waitForWindowCreation();

            // if everything is up and running, notify whoever is waiting for the engine to start up
            {
                std::lock_guard<std::mutex> lk(m_engine_started_mutex);
                m_engine_started = true;
            }
            m_engine_started_cVar.notify_one();

            // engine update loop
            while (render_exec_status != std::future_status::ready)
            {
                double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t_1 - t_0).count();
                //std::cout << "dt: " << dt << std::endl;
                t_0 = std::chrono::high_resolution_clock::now();

                // update world
                auto active_systems = m_world_state->getSystems();
                for (auto& system : active_systems)
                {
                    auto& world_state = *m_world_state.get();
                    system(world_state, dt, *m_task_schedueler);
                    //m_task_schedueler->submitTask(
                    //    [&world_state, dt, system]() {
                    //        system(world_state, dt);
                    //    }
                    //);
                }

                // finalize engine update by creating a new frame
                EngineCore::Common::Frame new_frame;

                new_frame.m_frameID = frameID++;
                new_frame.m_simulation_dt = dt;

                auto window_res = m_graphics_backend->getActiveWindowResolution();

                //assert(std::get<0>(window_res) > 0);

                new_frame.m_window_width = std::get<0>(window_res);
                new_frame.m_window_height = std::get<1>(window_res);

                Entity camera_entity = camera_mngr.getActiveCamera();
                
                if (camera_entity != entity_mngr.invalidEntity())
                {
                    auto camera_idx = camera_mngr.getIndex(camera_entity).front();

                    size_t camera_transform_idx = transform_mngr.getIndex(camera_entity);
                    new_frame.m_view_matrix = glm::inverse(transform_mngr.getWorldTransformation(camera_transform_idx));
                    new_frame.m_projection_matrix = camera_mngr.getProjectionMatrix(camera_idx);
                    new_frame.m_fovy = camera_mngr.getFovy(camera_idx);
                    new_frame.m_aspect_ratio = camera_mngr.getAspectRatio(camera_idx);
                    new_frame.m_exposure = camera_mngr.getExposure(camera_idx);

                    EngineCore::Common::Frame& update_frame = m_frame_manager->setUpdateFrame(std::move(new_frame));

                    t_2 = std::chrono::high_resolution_clock::now();
                    //Graphics::OpenGL::setupBasicForwardRenderingPipeline(update_frame, *m_world_state, *m_resource_manager);
                    EngineCore::Graphics::OpenGL::setupBasicDeferredRenderingPipeline(update_frame, *m_world_state, *m_resource_manager);
                    t_3 = std::chrono::high_resolution_clock::now();

                    auto dt2 = std::chrono::duration_cast<std::chrono::duration<double>>(t_3 - t_2).count();
                    //std::cout << "dt: " << dt << std::endl;
                    //std::cout << "dt2: " << dt2 << std::endl;

                    m_frame_manager->swapUpdateFrame();
                }

                // check if rendering pipeline is still running
                render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));

                t_1 = std::chrono::high_resolution_clock::now();
            }

            m_task_schedueler->stop();
        }

        void OpenGLEngineFrontend::waitForEngineStarted()
        {
            std::unique_lock<std::mutex> lk(m_engine_started_mutex);
            m_engine_started_cVar.wait(lk, [this] {return m_engine_started; });
        }

        void OpenGLEngineFrontend::update(size_t udpate_frameID, double dt, int window_width, int window_height)
        {
            // update world
            auto active_systems = m_world_state->getSystems();
            for (auto& system : active_systems)
            {
                auto& world_state = *m_world_state.get();
                system(world_state, dt, *m_task_schedueler);
                //m_task_schedueler->submitTask(
                //    [&world_state, dt, system]() {
                //        system(world_state, dt);
                //    }
                //);
            }

            // finalize engine update by creating a new frame
            EngineCore::Common::Frame new_frame;

            new_frame.m_frameID = udpate_frameID;
            new_frame.m_simulation_dt = dt;
            new_frame.m_window_width = window_width;
            new_frame.m_window_height = window_height;

            auto& camera_mngr = m_world_state->get<EngineCore::Graphics::CameraComponentManager>();
            auto& entity_mngr = m_world_state->accessEntityManager();
            auto& transform_mngr = m_world_state->get<EngineCore::Common::TransformComponentManager>();

            Entity camera_entity = camera_mngr.getActiveCamera();

            if (camera_entity != entity_mngr.invalidEntity())
            {
                auto camera_idx = camera_mngr.getIndex(camera_entity).front();

                size_t camera_transform_idx = transform_mngr.getIndex(camera_entity);
                new_frame.m_view_matrix = glm::inverse(transform_mngr.getWorldTransformation(camera_transform_idx));
                new_frame.m_projection_matrix = camera_mngr.getProjectionMatrix(camera_idx);
                new_frame.m_fovy = camera_mngr.getFovy(camera_idx);
                new_frame.m_aspect_ratio = camera_mngr.getAspectRatio(camera_idx);
                new_frame.m_exposure = camera_mngr.getExposure(camera_idx);

                EngineCore::Common::Frame& update_frame = m_frame_manager->setUpdateFrame(std::move(new_frame));

                //Graphics::OpenGL::setupBasicForwardRenderingPipeline(update_frame, *m_world_state, *m_resource_manager);
                EngineCore::Graphics::OpenGL::setupBasicDeferredRenderingPipeline(update_frame, *m_world_state, *m_resource_manager);
                
                m_frame_manager->swapUpdateFrame();
            }
        }

        void OpenGLEngineFrontend::render(size_t render_frameID, double dt, int window_width, int window_height)
        {
            // Perform single execution tasks
            //processSingleExecutionTasks();

            auto gl_err = glGetError();
            if (gl_err != GL_NO_ERROR)
                std::cerr << "GL error after single exection tasks: " << gl_err << std::endl;

            // Perform resource manager async tasks
            m_resource_manager->executeRenderThreadTasks();

            gl_err = glGetError();
            if (gl_err != GL_NO_ERROR)
                std::cerr << "GL error after resource manager tasks: " << gl_err << std::endl;

            // TODO try getting update for render frame ?
            
            // Get current frame for rendering
            auto& frame = m_frame_manager->getRenderFrame();

            std::vector<double> setup_resource_timings;
            setup_resource_timings.reserve(frame.m_render_passes.size());
            // Call buffer phase for each render pass
            for (auto& render_pass : frame.m_render_passes)
            {
                auto t0 = glfwGetTime();

                render_pass.setupResources();

                auto t1 = glfwGetTime();
                setup_resource_timings.emplace_back(t1 - t0);
            }

            gl_err = glGetError();
            if (gl_err != GL_NO_ERROR)
                std::cerr << "GL error after resource setup of frame " << frame.m_frameID << " : " << gl_err << std::endl;

            std::vector<double> render_timings;
            render_timings.reserve(frame.m_render_passes.size());
            // Call execution phase for each render pass
            for (auto& render_pass : frame.m_render_passes)
            {
                auto t0 = glfwGetTime();

                render_pass.execute();

                auto t1 = glfwGetTime();
                render_timings.emplace_back(t1 - t0);
            }

            gl_err = glGetError();
            if (gl_err != GL_NO_ERROR)
                std::cerr << "GL error after execution of frame " << frame.m_frameID << " : " << gl_err << std::endl;

            m_frame_manager->swapRenderFrame();
        }

        EngineCore::WorldState & OpenGLEngineFrontend::accessWorldState()
        {
            return (*m_world_state.get());
        }

        EngineCore::Common::FrameManager<EngineCore::Common::Frame>& OpenGLEngineFrontend::accessFrameManager()
        {
            return (*m_frame_manager.get());
        }

        EngineCore::Graphics::OpenGL::ResourceManager& OpenGLEngineFrontend::accessResourceManager()
        {
            return (*m_resource_manager.get());
        }

        void OpenGLEngineFrontend::addInputActionContext(EngineCore::Common::Input::InputActionContext const & input_action_context)
        {
            m_graphics_backend->addInputActionContext(input_action_context);
        }