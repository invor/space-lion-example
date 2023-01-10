#ifndef ImPlotOffscreenRenderPass_h
#define ImPlotOffscreenRenderPass_h

#include "WorldState.hpp"
#include "Dx11/Dx11Frame.hpp"
#include "Dx11/ResourceManager.hpp"

namespace Graphics {
namespace Dx11 {

    void addImPlotOffscreenRenderPass(
        EngineCore::Graphics::Dx11::Frame& frame,
        EngineCore::WorldState& world_state,
        EngineCore::Graphics::Dx11::ResourceManager& resource_mngr);
}
}

#endif // !ImPlotOffscreenRenderPass_h
