#include "DemoScene.hpp"

#include "CameraComponent.hpp"
#include "gltfAssetComponentManager.hpp"
#include "MaterialComponentManager.hpp"
#include "NameComponentManager.hpp"
#include "ParticlesComponentManager.hpp"
#include "PointlightComponent.hpp"
#include "ResourceLoading.hpp"
#include "Dx11/ResourceManager.hpp"
#include "SunlightComponentManager.hpp"
#include "TurntableComponentManager.hpp"
#include "WorldState.hpp"

#include <random>

void createDemoScene(EngineCore::WorldState& world_state, EngineCore::Graphics::Dx11::ResourceManager& resource_manager)
{
    auto& entity_mngr = world_state.accessEntityManager();
    //  auto& atmosphere_mngr = world_state.get<EngineCore::Graphics::AtmosphereComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
    auto& camera_mngr = world_state.get<EngineCore::Graphics::CameraComponentManager>();
    auto& gltf_mngr = world_state.get<EngineCore::Graphics::GltfAssetComponentManager>();
    auto& mtl_mngr = world_state.get<EngineCore::Graphics::MaterialComponentManager>();
    auto& mesh_mngr = world_state.get<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
    auto& name_mngr = world_state.get<EngineCore::Common::NameComponentManager>();
    auto& pointlight_mngr = world_state.get<EngineCore::Graphics::PointlightComponentManager>();
    auto& rsrc_mngr = resource_manager;
    auto& staticMesh_renderTask_mngr = world_state.get<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::StaticMesh>>();
    auto& unlit_renderTask_mngr = world_state.get<EngineCore::Graphics::RenderTaskComponentManager < EngineCore::Graphics::RenderTaskTags::Unlit>>();
    auto& sunlight_mngr = world_state.get<EngineCore::Graphics::SunlightComponentManager>();
    auto& transform_mngr = world_state.get<EngineCore::Common::TransformComponentManager>();
    auto& turntable_mngr = world_state.get<EngineCore::Animation::TurntableComponentManager>();
    auto& particles_mngr = world_state.get<EngineCore::Graphics::ParticlesComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();

    auto camera = entity_mngr.create();
    transform_mngr.addComponent(camera, Vec3(0.0, 0.0, 10.0));
    camera_mngr.addComponent(camera);
    camera_mngr.setActiveCamera(camera);

    Entity sun_entity = entity_mngr.create();
    transform_mngr.addComponent(sun_entity, Vec3(105783174465.5f, 105783174465.5f, 5783174465.5f), Quat(), Vec3(1.0f));
    sunlight_mngr.addComponent(sun_entity, Vec3(1.0f), 3.75f * std::pow(10.0f, 28.0f), 696342000.0f);

    Entity pointlight_entity = entity_mngr.create();
    transform_mngr.addComponent(pointlight_entity, Vec3(0.0f, 2.0f, 0.0f), Quat(), Vec3(1.0f));
    pointlight_mngr.addComponent(pointlight_entity, Vec3(1.0f), 2000.0f, 100.0f);

    //  Entity atmoshphere_entity = entity_mngr.create();
    //  //transform_mngr.addComponent(atmoshphere_entity, Vec3(0.0, -6361000.0, 0.0), Quat(), Vec3(6961000.0)); // check the correct size for the earth's atmosphere
    //  transform_mngr.addComponent(atmoshphere_entity, Vec3(0.0, -6360005.0, 0.0), Quat(), Vec3(6800000.0)); // check the correct size for the earth's atmosphere
    //  atmosphere_mngr.addComponent(atmoshphere_entity,
    //      //Vec3(0.0058,0.0135,0.0331),
    //      //Vec3(0.00444,0.00444,0.00444),
    //      Vec3(0.0000058, 0.0000135, 0.0000331),
    //      //Vec3(0.00000444, 0.00000444, 0.00000444),
    //      Vec3(0.000003, 0.000003, 0.000003),
    //      //Vec3(0.00002,0.00002,0.00002),
    //      8000.0,
    //      1200.0,
    //      6360000.0,
    //      //6359800.0,
    //      6420000.0);

    // Create "no-texture" debugging textures
    {
        D3D11_TEXTURE2D_DESC tx_desc;
        tx_desc.Width = 1;
        tx_desc.Height = 1;
        tx_desc.MipLevels = 1;
        tx_desc.ArraySize = 1;
        tx_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.SampleDesc.Count = 1;
        tx_desc.SampleDesc.Quality = 0;
        tx_desc.Usage = D3D11_USAGE_DEFAULT;
        tx_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
        SMViewDesc.Format = tx_desc.Format;
        SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SMViewDesc.TextureCube.MipLevels = tx_desc.MipLevels;
        SMViewDesc.TextureCube.MostDetailedMip = 0;

        auto tx_data = std::make_shared<std::vector<std::array<uint8_t, 4>>>();
        (*tx_data) = { { 255,0,255,255 } };

        //TODO add texture creation that doesn't use a vector of data storage containers
        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "noTexture_baseColor",
            tx_data, tx_desc, SMViewDesc);
    }

    {
        D3D11_TEXTURE2D_DESC tx_desc;
        tx_desc.Width = 1;
        tx_desc.Height = 1;
        tx_desc.MipLevels = 1;
        tx_desc.ArraySize = 1;
        tx_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.SampleDesc.Count = 1;
        tx_desc.SampleDesc.Quality = 0;
        tx_desc.Usage = D3D11_USAGE_DEFAULT;
        tx_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
        SMViewDesc.Format = tx_desc.Format;
        SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SMViewDesc.TextureCube.MipLevels = tx_desc.MipLevels;
        SMViewDesc.TextureCube.MostDetailedMip = 0;

        auto tx_data = std::make_shared<std::vector<std::array<uint8_t, 4>>>();
        (*tx_data) = { { 255,255,0,255 } };

        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "noTexture_metallicRoughness",
            tx_data, tx_desc, SMViewDesc);
    }

    {
        D3D11_TEXTURE2D_DESC tx_desc;
        tx_desc.Width = 1;
        tx_desc.Height = 1;
        tx_desc.MipLevels = 1;
        tx_desc.ArraySize = 1;
        tx_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.SampleDesc.Count = 1;
        tx_desc.SampleDesc.Quality = 0;
        tx_desc.Usage = D3D11_USAGE_DEFAULT;
        tx_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
        SMViewDesc.Format = tx_desc.Format;
        SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SMViewDesc.TextureCube.MipLevels = tx_desc.MipLevels;
        SMViewDesc.TextureCube.MostDetailedMip = 0;

        auto tx_data = std::make_shared<std::vector<std::array<uint8_t, 4>>>();
        (*tx_data) = { { 127,127,255,255 } };

        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "noTexture_normalMap",
            tx_data, tx_desc, SMViewDesc);
    }

    //auto shader_names = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::ShaderFilename>>();
    //shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/static_mesh.vertex.cso", dxowl::ShaderProgram::VertexShader });
    //shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/forward_pbr.pixel.cso", dxowl::ShaderProgram::PixelShader });
    //
    //
    //Entity debug_entity = entity_mngr.create();
    //
    //transform_mngr.addComponent(debug_entity, Vec3(0.0f, 0.0f, -2.0f), Quat(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(1.0f, 1.0f, 1.0f));
    //turntable_mngr.addComponent(debug_entity, 0.5f);
    //
    //auto triangle_vertices = std::make_shared<std::vector<std::vector<float>>>();
    //(*triangle_vertices) = { {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    //                         {-0.25f, 0.0f, 0.0f, 0.25f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f},
    //                         {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    //                         {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f} };
    //
    //auto triangle_indices = std::make_shared<std::vector<uint32_t>>(3);
    //(*triangle_indices) = { 0,1,2 };
    //
    //auto vertex_layout = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::VertexLayout>>();
    //*vertex_layout = {
    //    EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
    //    EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
    //    EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{16, {{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
    //    EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{8, {{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}}
    //};
    //
    //auto mesh_rsrc = mesh_mngr.addComponent(debug_entity, "debug_mesh", triangle_vertices, triangle_indices, vertex_layout, DXGI_FORMAT_R32_UINT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //
    //auto shader_rsrc = resource_manager.createShaderProgramAsync(
    //    "gltf_debug_shader",
    //    shader_names,
    //    vertex_layout);
    //
    //mtl_mngr.addComponent(debug_entity, "debug_material", shader_rsrc);
    //
    //auto transform_cached_idx = transform_mngr.getIndex(debug_entity);
    //auto mesh_cached_idx = mesh_mngr.getIndex(debug_entity);
    //auto mtl_cached_idx = mtl_mngr.getIndex(debug_entity);
    //staticMesh_renderTask_mngr.addComponent(debug_entity, mesh_rsrc, 0, shader_rsrc, 0, transform_cached_idx, mesh_cached_idx.front(), mtl_cached_idx.front(), true);

    {
        Entity cylinder = entity_mngr.create();
        transform_mngr.addComponent(cylinder, Vec3(0.0f, 0.0f, -2.0f), Quat(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(1.0f, 1.0f, 1.0f));
        turntable_mngr.addComponent(cylinder, 0.5f);

        auto cylinder_geometry = EngineCore::Graphics::createCylinder(0.5f, 2.0f, 64);
        auto vertex_layout = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::VertexLayout>>();
        *vertex_layout = {
            EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
            EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}}
            //EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{24, {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
        };

        auto triangle_vertices = std::make_shared<std::vector<std::vector<float>>>();
        (*triangle_vertices) = { {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
                                 {-0.25f, 0.0f, 0.0f, 0.25f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
                                 {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f} };

        auto triangle_indices = std::make_shared<std::vector<uint32_t>>(3);
        (*triangle_indices) = { 0,1,2 };

        auto mesh_rsrc = mesh_mngr.addComponent(cylinder, "debug_mesh", std::get<0>(cylinder_geometry), std::get<1>(cylinder_geometry), vertex_layout, DXGI_FORMAT_R32_UINT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto shader_names = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::ShaderFilename>>();
        shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/static_mesh_untextured.vertex.cso", dxowl::ShaderProgram::VertexShader });
        shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/forward_pbr.pixel.cso", dxowl::ShaderProgram::PixelShader });
        auto shader_rsrc = resource_manager.createShaderProgramAsync(
            "static_mesh_untextured_debug_shader",
            shader_names,
            vertex_layout);

        mtl_mngr.addComponent(cylinder, "debug_material", shader_rsrc);

        auto transform_cached_idx = transform_mngr.getIndex(cylinder);
        auto mesh_cached_idx = mesh_mngr.getIndex(cylinder);
        auto mtl_cached_idx = mtl_mngr.getIndex(cylinder);
        staticMesh_renderTask_mngr.addComponent(cylinder, mesh_rsrc, 0, shader_rsrc, 0, transform_cached_idx, mesh_cached_idx.front(), mtl_cached_idx.front(), true);
    }

    // prototyping textured spatial window
    {
        Entity spatial_window = entity_mngr.create();
        name_mngr.addComponent(spatial_window, "spatial_window");
        transform_mngr.addComponent(spatial_window, Vec3(2.0f, 0.0f, -2.0f), Quat(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(1.0f, 1.0f, 1.0f));

        auto quad_vertices = std::make_shared<std::vector<std::vector<float>>>();
        (*quad_vertices) = { {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f},
                                 {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f} };

        auto quad_indices = std::make_shared<std::vector<uint32_t>>(6);
        (*quad_indices) = { 0,1,2,2,3,0 };

        auto vertex_layout = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::VertexLayout>>();
        *vertex_layout = {
            EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
            EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{8, {{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}}
        };

        auto mesh_rsrc = mesh_mngr.addComponent(spatial_window, "spatial_window_mesh", quad_vertices, quad_indices, vertex_layout, DXGI_FORMAT_R32_UINT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto shader_names = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::ShaderFilename>>();
        shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/static_mesh_unlit.vertex.cso", dxowl::ShaderProgram::VertexShader });
        shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/textured_unlit.pixel.cso", dxowl::ShaderProgram::PixelShader });

        auto shader_rsrc = resource_manager.createShaderProgramAsync(
            "spatial_window_shader",
            shader_names,
            vertex_layout);

        D3D11_TEXTURE2D_DESC tx_desc;
        tx_desc.Width = 512;
        tx_desc.Height = 512;
        tx_desc.MipLevels = 1;
        tx_desc.ArraySize = 1;
        tx_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.SampleDesc.Count = 1;
        tx_desc.SampleDesc.Quality = 0;
        tx_desc.Usage = D3D11_USAGE_DEFAULT;
        tx_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tx_desc.CPUAccessFlags = 0;
        tx_desc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
        SMViewDesc.Format = tx_desc.Format;
        SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SMViewDesc.TextureCube.MipLevels = tx_desc.MipLevels;
        SMViewDesc.TextureCube.MostDetailedMip = 0;

        auto tx_data = std::make_shared<std::vector<std::array<uint8_t, 4>>>(512 * 512);
        //(*tx_data) = { { 255,0,255,255 } };

        //TODO add texture creation that doesn't use a vector of data storage containers
        auto tx_rsrcID = resource_manager.createTexture2DAsync(
            "spatial_window_texture",
            tx_data, tx_desc, SMViewDesc);
        std::vector<std::pair<EngineCore::Graphics::MaterialComponentManager::TextureSemantic, EngineCore::Graphics::ResourceID>> textures
            = { { EngineCore::Graphics::MaterialComponentManager::TextureSemantic::ALBEDO, tx_rsrcID } };

        mtl_mngr.addComponent(
            spatial_window,
            "spatial_window_mtl",
            shader_rsrc,
            { 0.0f,0.0f,0.0f,0.0f },
            { 0.0f,0.0f,0.0f,0.0f },
            0.0f,
            textures
        );

        auto transform_cached_idx = transform_mngr.getIndex(spatial_window);
        auto mesh_cached_idx = mesh_mngr.getIndex(spatial_window);
        auto mtl_cached_idx = mtl_mngr.getIndex(spatial_window);
        unlit_renderTask_mngr.addComponent(spatial_window, mesh_rsrc, 0, shader_rsrc, 0, transform_cached_idx, mesh_cached_idx.front(), mtl_cached_idx.front(), true);
    }

    //auto skinned_mesh_shader_names = std::make_shared<std::vector<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>>(
    //    std::initializer_list<EngineCore::Graphics::OpenGL::ResourceManager::ShaderFilename>{
    //        { "../space-lion/resources/shaders/skinned_mesh_notx_v.glsl", glowl::GLSLProgram::ShaderType::Vertex },
    //        { "../space-lion/resources/shaders/skinned_mesh_notx_f.glsl", glowl::GLSLProgram::ShaderType::Fragment }
    //});
    //
    //auto skinned_mesh_shader_rsrc = rsrc_mngr.createShaderProgramAsync(
    //    "skinned_mesh_shader",
    //    skinned_mesh_shader_names
    //);

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

    //gltf_mngr.importGltfScene(EngineCore::Utility::GetAppFolder().string() + "Resources/FlightHelmet.gltf", shader_rsrc);


    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/Main/NewSponza_Main_Blender_glTF.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/OrientationTest/glTF/OrientationTest.gltf", shader_rsrc);
    //gltf_mngr.importGltfScene("../../glTF-Sample-Models/2.0/RiggedFigure/glTF/RiggedFigure.gltf", skinned_mesh_shader_rsrc);
    //gltf_mngr.importGltfScene("C:/Users/micha/Downloads/potted_plant_01_4k.gltf/potted_plant_01_4k.gltf", shader_rsrc);
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


    // test and debug particles rendering
    {
        auto& particles_mngr = world_state.get<EngineCore::Graphics::ParticlesComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
        auto& particles_renderTask_mngr = world_state.get<EngineCore::Graphics::RenderTaskComponentManager<EngineCore::Graphics::RenderTaskTags::Particles>>();

        // empty vertex layout?
        auto vertex_layout = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::VertexLayout>>();
        *vertex_layout = {
        EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}}},
        EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{8,  {{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}}}
        };

        typedef std::pair < std::string, dxowl::ShaderProgram::ShaderType > ShaderFilename;

        auto shader_names = std::make_shared<std::vector<ShaderFilename>>(
            std::initializer_list<ShaderFilename>{
                { EngineCore::Utility::GetAppFolder().string() + "SpaceLion/particles.vertex.cso", dxowl::ShaderProgram::VertexShader },
                { EngineCore::Utility::GetAppFolder().string() + "SpaceLion/particles.pixel.cso", dxowl::ShaderProgram::PixelShader }
        }
        );

        auto shader_rsrc = resource_manager.createShaderProgramAsync(
            "area_indicator",
            shader_names,
            vertex_layout);

        auto particles_entity = entity_mngr.create();
        auto xform_idx = transform_mngr.addComponent(particles_entity);
        mtl_mngr.addComponent(particles_entity, "particles", shader_rsrc);
        auto mtl_idx = mtl_mngr.getIndex(particles_entity).front();
        struct Particle {
            float position[3];
            float radius;
        };
        auto particles_data = std::make_shared<std::vector<Particle>>();
        size_t particles_cnt = 1000000;
        particles_data->reserve(particles_cnt);

        std::uniform_real_distribution<float> pos_dist(0, 1);
        std::uniform_real_distribution<float> rad_dist(0.05, 0.15);
        std::mt19937 prng;

        for (int i = 0; i < particles_cnt; ++i) {
            float x = pos_dist(prng) * 10.0f - 0.5f * 10.0f;
            float y = pos_dist(prng) * 10.0f - 0.5f * 10.0f;
            float z = pos_dist(prng) * 10.0f - 0.5f * 10.0f;
            float r = rad_dist(prng);
            particles_data->push_back({ {x,y,z},r });
        }

        particles_mngr.addComponent(particles_entity, particles_data);
        particles_renderTask_mngr.addComponent(particles_entity, EngineCore::Graphics::ResourceID(), 0, shader_rsrc, 0, xform_idx, 0, mtl_idx, true);
    }
}
