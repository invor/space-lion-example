#ifndef DemoScene_hpp
#define DemoScene_hpp

namespace EngineCore {
    class WorldState;

    namespace Graphics {
        namespace Dx11 {
            class ResourceManager;
        }
    }
}

void createDemoScene(EngineCore::WorldState& world_state, EngineCore::Graphics::Dx11::ResourceManager& resource_manager);

#endif // !DemoScene_hpp
