#ifndef DirectXEngineFrontendUWP_hpp
#define DirectXEngineFrontendUWP_hpp

#include <memory>

#include "Frame.hpp"
#include "Dx11/ResourceManager.hpp"
#include "TaskSchedueler.hpp"
#include "WorldState.hpp"

class DirectXEngineFrontendUWP
{
public:
    DirectXEngineFrontendUWP() = default;
    ~DirectXEngineFrontendUWP() = default;

private:

    // Simple multi-thread task schedueler
    std::unique_ptr<EngineCore::Utility::TaskSchedueler> m_task_schedueler;

    // The frame manager used to carry over snapshots of the world simulation to rendering.
    std::unique_ptr<EngineCore::Common::FrameManager<EngineCore::Common::Frame>> m_frame_manager;

    // GPU resource manager
    std::unique_ptr<EngineCore::Graphics::Dx11::ResourceManager> m_resource_manager;

    // Collection of all component manager that make up the world (state)
    std::unique_ptr<EngineCore::WorldState> m_world_state;

};

#endif // !DirectXEngineFrontendUWP_hpp
