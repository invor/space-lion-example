#ifndef DirectXEngineFrontendUWP_hpp
#define DirectXEngineFrontendUWP_hpp

#include <memory>

#include "pch.h"
#include "DeviceResources.h"

#include "Dx11/Dx11Frame.hpp"
#include "Dx11/ResourceManager.hpp"
#include "InputEvent.hpp"
#include "TaskScheduler.hpp"
#include "WorldState.hpp"

class DirectXEngineFrontendUWP : public DX::IDeviceNotify
{
public:
    DirectXEngineFrontendUWP(std::shared_ptr<DX::DeviceResources>  const& device_resources);
    ~DirectXEngineFrontendUWP() = default;

    void init(::IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation);

    void update(size_t udpate_frameID, double dt, int window_width, int window_height);

    void render(size_t render_frameID, double dt, int window_width, int window_height);

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    /**
     * Grant access to world for the "driving" part of the application, e.g. editor, game, simulation etc.
     */
    EngineCore::WorldState& accessWorldState();

    /**
     * Grant access to frame data for the "driving" part of the application, e.g. editor, game, simulation etc.
     */
    EngineCore::Common::FrameManager<EngineCore::Graphics::Dx11::Frame>& accessFrameManager();

    /**
     * Grant access to gpu resources for the "driving" part of the application, e.g. editor, game, simulation etc.
     */
    EngineCore::Graphics::Dx11::ResourceManager& accessResourceManager();

    /**
     * Pass on an input action context to window handling in graphics backend
     */
    void addInputActionContext(EngineCore::Common::Input::InputActionContext const& input_action_context);

private:

    void createDeviceDependentResources();

    void createWindowSizeDependentResources();

    // Simple multi-thread task schedueler
    std::unique_ptr<EngineCore::Utility::TaskScheduler>                         m_task_schedueler;

    // The frame manager used to carry over snapshots of the world simulation to rendering.
    std::unique_ptr<EngineCore::Common::FrameManager<EngineCore::Graphics::Dx11::Frame>> m_frame_manager;

    // GPU resource manager
    std::unique_ptr<EngineCore::Graphics::Dx11::ResourceManager>                 m_resource_manager;

    // Collection of all component manager that make up the world (state)
    std::unique_ptr<EngineCore::WorldState>                                      m_world_state;

    // DirectX device resources
    std::shared_ptr<DX::DeviceResources>                                         m_device_resources;

};

#endif // !DirectXEngineFrontendUWP_hpp
