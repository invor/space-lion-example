#include "DirectXEngineFrontendUWP.hpp"

#include "AirplanePhysicsComponent.hpp"
#include "AnimationSystems.hpp"
#include "CameraComponent.hpp"
#include "gltfAssetComponentManager.hpp"
#include "MaterialComponentManager.hpp"
#include "MeshComponentManager.hpp"
#include "NameComponentManager.hpp"
#include "PointlightComponent.hpp"
#include "RenderTaskComponentManager.hpp"
#include "Dx11/Dx11Frame.hpp"
#include "Dx11/SimpleForwardRenderingPipeline.hpp"
#include "SunlightComponentManager.hpp"
#include "SkinComponentManager.hpp"
#include "TransformComponentManager.hpp"

DirectXEngineFrontendUWP::DirectXEngineFrontendUWP(std::shared_ptr<DX::DeviceResources>  const& device_resources)
    : m_task_schedueler(std::make_unique<EngineCore::Utility::TaskSchedueler>()),
    m_frame_manager(std::make_unique<EngineCore::Common::FrameManager<EngineCore::Graphics::Dx11::Frame>>()),
    m_resource_manager(std::make_unique<EngineCore::Graphics::Dx11::ResourceManager>()),
    m_world_state(std::make_unique<EngineCore::WorldState>()),
    m_device_resources(device_resources)
{
    m_device_resources->RegisterDeviceNotify(this);

    m_world_state->add<EngineCore::Physics::AirplanePhysicsComponentManager>(std::make_unique<EngineCore::Physics::AirplanePhysicsComponentManager>(128, *m_world_state.get()));
    //m_world_state->add<EngineCore::Graphics::AtmosphereComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(std::make_unique<EngineCore::Graphics::AtmosphereComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(8, *m_resource_manager.get()));
    m_world_state->add<EngineCore::Graphics::CameraComponentManager>(std::make_unique<EngineCore::Graphics::CameraComponentManager>(8));
    m_world_state->add<EngineCore::Graphics::GltfAssetComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>(std::make_unique<EngineCore::Graphics::GltfAssetComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>(*m_resource_manager.get(), *m_world_state.get()));
    m_world_state->add<EngineCore::Graphics::MaterialComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>(std::make_unique<EngineCore::Graphics::MaterialComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>(m_resource_manager.get()));
    m_world_state->add<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>(std::make_unique<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>(m_resource_manager.get()));
    m_world_state->add<EngineCore::Common::NameComponentManager>(std::make_unique<EngineCore::Common::NameComponentManager>());
    m_world_state->add<EngineCore::Graphics::PointlightComponentManager>(std::make_unique<EngineCore::Graphics::PointlightComponentManager>(16000));
    m_world_state->add<EngineCore::Graphics::SunlightComponentManager>(std::make_unique<EngineCore::Graphics::SunlightComponentManager>(1));
    m_world_state->add<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>(std::make_unique<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>());
    m_world_state->add<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::SkinnedMesh>>(std::make_unique<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::SkinnedMesh>>());
    m_world_state->add<EngineCore::Common::TransformComponentManager>(std::make_unique<EngineCore::Common::TransformComponentManager>(250000));
    m_world_state->add<EngineCore::Animation::TurntableComponentManager>(std::make_unique<EngineCore::Animation::TurntableComponentManager>());
    m_world_state->add<EngineCore::Animation::SkinComponentManager>(std::make_unique<EngineCore::Animation::SkinComponentManager>());
    //m_world_state->add<EngineCore::Graphics::Landscape::FeatureCurveComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(std::make_unique<EngineCore::Graphics::Landscape::FeatureCurveComponentManager<EngineCore::Graphics::OpenGL::ResourceManager>>(*m_world_state.get(), *m_resource_manager.get()));

    m_world_state->add([](EngineCore::WorldState& world_state, double dt) {
        auto& transform_mngr = world_state.get<EngineCore::Common::TransformComponentManager>();
        auto& turntable_mngr = world_state.get<EngineCore::Animation::TurntableComponentManager>();
        EngineCore::Animation::animateTurntables(transform_mngr, turntable_mngr, dt);
        }
    );

    // start task schedueler with 1 thread
    m_task_schedueler->run(1);
}

void DirectXEngineFrontendUWP::init(::IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation)
{
    m_device_resources->SetWindow(window, width, height, rotation);

    m_device_resources->CreateDeviceResources();
    createDeviceDependentResources();
    m_resource_manager->init(m_device_resources->GetD3DDevice(), m_device_resources->GetD3DDeviceContext());

    m_device_resources->CreateWindowSizeDependentResources();
    createWindowSizeDependentResources();
}

void DirectXEngineFrontendUWP::update(size_t udpate_frameID, double dt, int window_width, int window_height)
{
    // update world
    auto active_systems = m_world_state->getSystems();
    for (auto& system : active_systems)
    {
        auto& world_state = *m_world_state.get();
        m_task_schedueler->submitTask(
            [&world_state, dt, system]() {
                system(world_state, dt);
            }
        );
    }

    // TODO wait for world updates to finish...

    // finalize engine update by creating a new frame
    EngineCore::Graphics::Dx11::Frame new_frame;

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
        //auto camera_idx = camera_mngr.getIndex(camera_entity).front();
        //
        //size_t camera_transform_idx = transform_mngr.getIndex(camera_entity);
        //new_frame.m_view_matrix = glm::inverse(transform_mngr.getWorldTransformation(camera_transform_idx));
        //new_frame.m_projection_matrix = camera_mngr.getProjectionMatrix(camera_idx);
        //new_frame.m_fovy = camera_mngr.getFovy(camera_idx);
        //new_frame.m_aspect_ratio = camera_mngr.getAspectRatio(camera_idx);
        //new_frame.m_exposure = camera_mngr.getExposure(camera_idx);

        EngineCore::Graphics::Dx11::Frame& update_frame = m_frame_manager->setUpdateFrame(std::move(new_frame));

        //Graphics::OpenGL::setupBasicForwardRenderingPipeline(update_frame, *m_world_state, *m_resource_manager);
        EngineCore::Graphics::Dx11::setupSimpleForwardRenderingPipeline(update_frame, *m_world_state, *m_resource_manager);

        m_frame_manager->swapUpdateFrame();
    }
}

void DirectXEngineFrontendUWP::render(size_t render_frameID, double dt, int window_width, int window_height)
{
    // Get current frame for rendering
    auto& frame = m_frame_manager->getRenderFrame();

    // Get swapchain info from device resources
    frame.m_render_target_view = m_device_resources->GetRenderTargetView();
    frame.m_depth_stencil_view = m_device_resources->GetDepthStencilView();

    std::vector<double> setup_resource_timings;
    setup_resource_timings.reserve(frame.m_render_passes.size());
    // Call buffer phase for each render pass
    for (auto& render_pass : frame.m_render_passes)
    {
        //auto t0 = glfwGetTime();

        render_pass.setupResources();

        //auto t1 = glfwGetTime();
        //setup_resource_timings.emplace_back(t1 - t0);
    }

    std::vector<double> render_timings;
    render_timings.reserve(frame.m_render_passes.size());
    // Call execution phase for each render pass
    for (auto& render_pass : frame.m_render_passes)
    {
        //auto t0 = glfwGetTime();

        render_pass.execute();

        //auto t1 = glfwGetTime();
        //render_timings.emplace_back(t1 - t0);
    }

    m_frame_manager->swapRenderFrame();
}

void DirectXEngineFrontendUWP::OnDeviceLost()
{
}

void DirectXEngineFrontendUWP::OnDeviceRestored()
{
}

EngineCore::WorldState& DirectXEngineFrontendUWP::accessWorldState()
{
    return (*m_world_state);
}

EngineCore::Common::FrameManager<EngineCore::Graphics::Dx11::Frame>& DirectXEngineFrontendUWP::accessFrameManager()
{
    return (*m_frame_manager);
}

EngineCore::Graphics::Dx11::ResourceManager& DirectXEngineFrontendUWP::accessResourceManager()
{
    return (*m_resource_manager);
}

void DirectXEngineFrontendUWP::addInputActionContext(EngineCore::Common::Input::InputActionContext const& input_action_context)
{
}

void DirectXEngineFrontendUWP::createDeviceDependentResources()
{
    //TODO
}

void DirectXEngineFrontendUWP::createWindowSizeDependentResources()
{
    //TODO
}
