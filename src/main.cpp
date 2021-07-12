#include <thread>
#include <future>

#include "EngineFrontend.hpp"
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

#include "..\Editor\CameraController.hpp"


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

    gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", shader_rsrc);
    gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/OrientationTest/glTF/OrientationTest.gltf", shader_rsrc);
    gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/RiggedFigure/glTF/RiggedFigure.gltf", skinned_mesh_shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/CesiumMan/glTF/CesiumMan.gltf", skinned_mesh_shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/rungholt/rungholt.gltf", shader_rsrc);
}


int main() {

    EngineCore::Common::EngineFrontend engine_frontend;

    std::thread engine_thread(&(EngineCore::Common::EngineFrontend::startEngine), &engine_frontend);

    engine_frontend.waitForEngineStarted();

    auto& world_state = engine_frontend.accessWorldState();
    auto& resource_manager = engine_frontend.accessResourceManager();

    createDemoScene(world_state, resource_manager);

    Editor::Controls::CameraController cam_ctrl(engine_frontend.accessWorldState(),engine_frontend.accessFrameManager());

    engine_frontend.addInputActionContext(cam_ctrl.getInputActionContext());

    engine_thread.join();

    return 0;
}