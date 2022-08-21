#include "DirectXEngineFrontendUWP.hpp"

DirectXEngineFrontendUWP::DirectXEngineFrontendUWP(std::shared_ptr<DX::DeviceResources>  const& device_resources)
    : m_task_schedueler(std::make_unique<EngineCore::Utility::TaskSchedueler>()),
    m_frame_manager(std::make_unique<EngineCore::Common::FrameManager<EngineCore::Common::Frame>>()),
    m_resource_manager(std::make_unique<EngineCore::Graphics::Dx11::ResourceManager>(device_resources->GetD3DDevice(),device_resources->GetD3DDeviceContext())),
    m_world_state(std::make_unique<EngineCore::WorldState>()),
    m_device_resources(device_resources)
{
    m_device_resources->RegisterDeviceNotify(this);
}

void DirectXEngineFrontendUWP::init(::IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation)
{
    m_device_resources->SetWindow(window, width, height, rotation);

    m_device_resources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_device_resources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
}

void DirectXEngineFrontendUWP::run()
{
    while (true)
    {
        m_device_resources->Present();
    }
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

EngineCore::Common::FrameManager<EngineCore::Common::Frame>& DirectXEngineFrontendUWP::accessFrameManager()
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

void DirectXEngineFrontendUWP::CreateDeviceDependentResources()
{
    //TODO
}

void DirectXEngineFrontendUWP::CreateWindowSizeDependentResources()
{
    //TODO
}
