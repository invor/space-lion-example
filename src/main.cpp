#include <thread>
#include <future>

#include "EngineFrontend.hpp"
#include "OpenGL/ResourceManager.hpp"

#include "..\Editor\CameraController.hpp"


void createDemoScene(EngineCore::WorldState& world_state, EngineCore::Graphics::OpenGL::ResourceManager& resource_manager)
{
    auto& entity_mngr = world_state.accessEntityManager();
    auto& camera_mngr = world_state.accessCameraComponentManager();
    auto& gltf_mngr = world_state.accessGltfAssetComponentManager();
    auto& mtl_mngr = world_state.accessMaterialComponentManager();
    auto& mesh_mngr = world_state.accessMeshComponentManager();
    auto& rsrc_mngr = resource_manager;
    auto& renderTask_mngr = world_state.accessRenderTaskComponentManager();
    auto& transform_mngr = world_state.accessTransformManager();
    auto& turntable_mngr = world_state.accessTurntableManager();

    auto camera = entity_mngr.create();
    transform_mngr.addComponent(camera, Vec3(0.0, 0.0, 2.5));
    camera_mngr.addComponent(camera);
    camera_mngr.setActiveCamera(camera);

    //TODO create turntable animation
    //auto cube = entity_mngr.create();
    //transform_mngr.addComponent(cube, Vec3(0.0, 0.0, -5.0));
    //turntable_mngr.addComponent(cube, 0.5f);
    //auto mesh_data = Graphics::createBox();
    //auto mesh_rsrc = mesh_mngr.addComponent(
    //    cube,
    //    "demo_plane",
    //    std::get<0>(mesh_data),
    //    std::get<1>(mesh_data),
    //    std::make_shared<glowl::VertexLayout>(rsrc_mngr.convertGenericGltfVertexLayout(*std::get<2>(mesh_data))),
    //    GL_UNSIGNED_INT,
    //    GL_TRIANGLES);

    auto shader_names = std::make_shared<std::vector<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>>(
        std::initializer_list<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>{
            { "../space-lion/resources/shaders/simple_forward_vert.glsl", glowl::GLSLProgram::ShaderType::Vertex },
            { "../space-lion/resources/shaders/fwd_pbrMetallic_f.glsl", glowl::GLSLProgram::ShaderType::Fragment }
    });

    auto shader_rsrc = rsrc_mngr.createShaderProgramAsync(
        "debug_forward_shader",
        shader_names
    );

    //mtl_mngr.addComponent(cube, "debug_cube_material", shader_rsrc);

    //renderTask_mngr.addComponent(cube, mesh_rsrc, 0, shader_rsrc, 0);

    //Graphics::OpenGL::loadGLTFScene("../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", *m_world_state, rsrc_mngr);

    //auto gltf_root = entity_mngr.create();
    //transform_mngr.addComponent(gltf_root, Vec3(0.0, 0.0, 0.0));
    //turntable_mngr.addComponent(gltf_root, 0.5f);
    //
    //{
    //    auto gltf_subobj = entity_mngr.create();
    //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //    transform_mngr.setParent(transform_idx, gltf_root);
    //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "Hose_low", shader_rsrc);
    //}
    //
    //{
    //    auto gltf_subobj = entity_mngr.create();
    //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //    transform_mngr.setParent(transform_idx, gltf_root);
    //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "RubberWood_low", shader_rsrc);
    //}
    //
    //{
    //    auto gltf_subobj = entity_mngr.create();
    //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //    transform_mngr.setParent(transform_idx, gltf_root);
    //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "GlassPlastic_low", shader_rsrc);
    //}
    //
    //{
    //    auto gltf_subobj = entity_mngr.create();
    //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //    transform_mngr.setParent(transform_idx, gltf_root);
    //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "MetalParts_low", shader_rsrc);
    //}
    //
    //{
    //    auto gltf_subobj = entity_mngr.create();
    //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //    transform_mngr.setParent(transform_idx, gltf_root);
    //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "LeatherParts_low", shader_rsrc);
    //}
    //
    //{
    //    auto gltf_subobj = entity_mngr.create();
    //    size_t transform_idx = transform_mngr.addComponent(gltf_subobj, Vec3(0.0, 0.0, 0.0));
    //    transform_mngr.setParent(transform_idx, gltf_root);
    //    gltf_mngr.addComponent(gltf_subobj, "../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", "Lenses_low", shader_rsrc);
    //}

    gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", shader_rsrc);

    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/Walkman_01.blend/Walkman_01.gltf", shader_rsrc);

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