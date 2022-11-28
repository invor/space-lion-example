//#include <windows.h>
#include "pch.h"
#include "DirectXEngineFrontendUWP.hpp"
#include "Dx11/ResourceManager.hpp"
#include "ResourceLoading.hpp"

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

void createDemoScene(EngineCore::WorldState& world_state, EngineCore::Graphics::Dx11::ResourceManager& resource_manager)
{
    auto& entity_mngr = world_state.accessEntityManager();
    //  auto& atmosphere_mngr = world_state.get<EngineCore::Graphics::AtmosphereComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
    auto& camera_mngr = world_state.get<EngineCore::Graphics::CameraComponentManager>();
    auto& gltf_mngr = world_state.get<EngineCore::Graphics::GltfAssetComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
    auto& mtl_mngr = world_state.get< EngineCore::Graphics::MaterialComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
    auto& mesh_mngr = world_state.get<EngineCore::Graphics::MeshComponentManager<EngineCore::Graphics::Dx11::ResourceManager>>();
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

    auto shader_names = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::ShaderFilename>>();
    shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/static_mesh.vertex.cso", dxowl::ShaderProgram::VertexShader });
    shader_names->push_back({ EngineCore::Utility::GetAppFolder().string() + "SpaceLion/forward_pbr.pixel.cso", dxowl::ShaderProgram::PixelShader });


    Entity debug_entity = entity_mngr.create();

    transform_mngr.addComponent(debug_entity, Vec3(0.0f, 0.0f, -2.0f), Quat(0.0f, Vec3(0.0f, 1.0f, 0.0f)), Vec3(1.0f, 1.0f, 1.0f));

    auto triangle_vertices = std::make_shared<std::vector<std::vector<float>>>();
    (*triangle_vertices) = { {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
                             {-0.25f, 0.0f, 0.5f, 0.25f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f},
                             {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f} };

    auto triangle_indices = std::make_shared<std::vector<uint32_t>>(3);
    (*triangle_indices) = { 0,1,2 };

    auto vertex_layout = std::make_shared<std::vector<EngineCore::Graphics::Dx11::ResourceManager::VertexLayout>>();
    *vertex_layout = {
        EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
        EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{12, {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}},
        EngineCore::Graphics::Dx11::ResourceManager::VertexLayout{8, {{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }}}
    };

    auto mesh_rsrc = mesh_mngr.addComponent(debug_entity, "debug_mesh", triangle_vertices, triangle_indices, vertex_layout, DXGI_FORMAT_R32_UINT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    //co_await std::async(std::launch::async, []() ->std::future <void> {
    //    co_await DX::ReadDataAsync(L"ms-appx:///gltfPixelShader.cso");
    //});
    //EngineCore::Graphics::ResourceID shader_rsrc = m_rsrc_manager->invalidResourceID();

    auto shader_rsrc = resource_manager.createShaderProgramAsync(
        "gltf_debug_shader",
        shader_names,
        vertex_layout);


    mtl_mngr.addComponent(debug_entity, "debug_material", shader_rsrc);

    auto transform_cached_idx = transform_mngr.getIndex(debug_entity);
    auto mesh_cached_idx = mesh_mngr.getIndex(debug_entity);
    auto mtl_cached_idx = mtl_mngr.getIndex(debug_entity);
    renderTask_mngr.addComponent(debug_entity, mesh_rsrc, 0, shader_rsrc, 0, transform_cached_idx, mesh_cached_idx.front(), mtl_cached_idx.front(), true);

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
}

// Refer to https://github.com/walbourn/directx-vs-templates/tree/main/d3d11game_uwp_cppwinrt_dr
void GetWindowBounds(_In_::IUnknown* window, _Out_ RECT* rect);

namespace
{
    inline int ConvertDipsToPixels(float dips, float dpi) noexcept
    {
        return int(dips * dpi / 96.f + 0.5f);
    }

    inline float ConvertPixelsToDips(int pixels, float dpi) noexcept
    {
        return (float(pixels) * 96.f / dpi);
    }

    DXGI_MODE_ROTATION ComputeDisplayRotation(
        winrt::Windows::Graphics::Display::DisplayOrientations	native_orientation,
        winrt::Windows::Graphics::Display::DisplayOrientations	current_orientation) noexcept
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        switch (native_orientation)
        {
        case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
            switch (current_orientation)
            {
            case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_ROTATE270;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE90;
                break;

            default:
                break;
            }
            break;

        case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
            switch (current_orientation)
            {
            case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_ROTATE90;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE270;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        return rotation;
    }
}

struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
    winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& application_view) 
    {
        application_view.Activated({ this, &App::OnActivated });

        winrt::Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &App::OnSuspending });

        winrt::Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &App::OnResuming });

        // At this point we have access to the device. 
        // We can create the device-dependent resources.
        m_device_resources = std::make_shared<DX::DeviceResources>();
        m_engine_frontend = std::make_unique<DirectXEngineFrontendUWP>(m_device_resources);
    }

    void Uninitialize()
    {
        m_engine_frontend.reset();
    }

    void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window)
    {
        window.SizeChanged({ this, &App::OnWindowSizeChanged });

        try
        {
            window.ResizeStarted([this](auto&&, auto&&) { m_in_sizemove = true; });

            window.ResizeCompleted([this](auto&&, auto&&) { m_in_sizemove = false; HandleWindowSizeChanged(); });
        }
        catch (...)
        {
            // Requires Windows 10 Creators Update (10.0.15063) or later
        }

        window.VisibilityChanged({ this, &App::OnVisibilityChanged });

        window.Closed([this](auto&&, auto&&) { m_exit = true; });

        auto dispatcher = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher();

        dispatcher.AcceleratorKeyActivated({ this, &App::OnAcceleratorKeyActivated });

        auto navigation = winrt::Windows::UI::Core::SystemNavigationManager::GetForCurrentView();

        // UWP on Xbox One triggers a back request whenever the B button is pressed
        // which can result in the app being suspended if unhandled
        navigation.BackRequested([](
            const winrt::Windows::Foundation::IInspectable&,
            const winrt::Windows::UI::Core::BackRequestedEventArgs& args)
            {
                args.Handled(true);
            });

        auto currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

        currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });

        currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });

        winrt::Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });

        m_DPI = currentDisplayInformation.LogicalDpi();

        m_logical_width = window.Bounds().Width;
        m_logical_height = window.Bounds().Height;

        m_native_orientation = currentDisplayInformation.NativeOrientation();
        m_current_orientation = currentDisplayInformation.CurrentOrientation();

        m_pixel_width = ConvertDipsToPixels(m_logical_width, m_DPI);
        m_pixel_height = ConvertDipsToPixels(m_logical_height, m_DPI);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation(m_native_orientation, m_current_orientation);

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(m_pixel_width, m_pixel_height);
        }

        auto windowPtr = winrt::get_unknown(window);

        m_engine_frontend->init(windowPtr, m_pixel_width, m_pixel_height, rotation);
    }

    void Load(winrt::hstring const& /* entryPoint */)
    {
    }

    void OnActivated(
        winrt::Windows::ApplicationModel::Core::CoreApplicationView const& /* applicationView */,
        winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& /* args */)
    {
        winrt::Windows::UI::Core::CoreWindow window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
        window.Activate();
    }

    void OnSuspending(IInspectable const& /* sender */, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args) 
    {

    }

    void OnResuming(IInspectable const& /* sender */, IInspectable const& /* args */) 
    {

    }

    void OnWindowSizeChanged(
        winrt::Windows::UI::Core::CoreWindow const& sender,
        winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& /*args*/)
    {
        m_logical_width = sender.Bounds().Width;
        m_logical_height = sender.Bounds().Height;

        if (m_in_sizemove)
            return;

        HandleWindowSizeChanged();
    }

    void OnVisibilityChanged(
        winrt::Windows::UI::Core::CoreWindow const& /*sender*/,
        winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args)
    {
        m_visible = args.Visible();
        if (m_visible) {
            //m_game->OnActivated();
            //TODO
        }
        else {
            //m_game->OnDeactivated();
            //TODO
        }
    }

    void OnAcceleratorKeyActivated(
        winrt::Windows::UI::Core::CoreDispatcher const&,
        winrt::Windows::UI::Core::AcceleratorKeyEventArgs const& args)
    {
        if (args.EventType() == winrt::Windows::UI::Core::CoreAcceleratorKeyEventType::SystemKeyDown
            && args.VirtualKey() == winrt::Windows::System::VirtualKey::Enter
            && args.KeyStatus().IsMenuKeyDown
            && !args.KeyStatus().WasKeyDown)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            auto view = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();

            if (view.IsFullScreenMode())
                view.ExitFullScreenMode();
            else
                view.TryEnterFullScreenMode();

            args.Handled(true);
        }
    }

    void OnDpiChanged(
        winrt::Windows::Graphics::Display::DisplayInformation const& sender,
        IInspectable const& /*args*/)
    {
        m_DPI = sender.LogicalDpi();

        HandleWindowSizeChanged();
    }

    void OnOrientationChanged(
        winrt::Windows::Graphics::Display::DisplayInformation const& sender,
        IInspectable const& /*args*/)
    {
        auto resizeManager = winrt::Windows::UI::Core::CoreWindowResizeManager::GetForCurrentView();
        resizeManager.ShouldWaitForLayoutCompletion(true);

        m_current_orientation = sender.CurrentOrientation();

        HandleWindowSizeChanged();

        resizeManager.NotifyLayoutCompleted();
    }

    void OnDisplayContentsInvalidated(
        winrt::Windows::Graphics::Display::DisplayInformation const& /*sender*/,
        IInspectable const& /*args*/)
    {
        //m_game->ValidateDevice();
        //m_game->OnDisplayChange();
    }

    void Run()
    {
        auto game_update_loop = [this](std::shared_future<void> render_exec)
        {
            // inplace construct an input action context to test the new concept
            //  auto evt_func = [](EngineCore::Common::Input::Event const& evt, EngineCore::Common::Input::HardwareState const& state) {
            //      std::cout << "Paying respect to new input system" << "\n";
            //  };
            //  EngineCore::Common::Input::EventDrivenAction evt_action = { {EngineCore::Common::Input::Device::KEYBOARD,EngineCore::Common::Input::KeyboardKeys::KEY_F,EngineCore::Common::Input::EventTrigger::PRESS}, evt_func };
            //  EngineCore::Common::Input::InputActionContext input_context = { "test_input_context", true, {evt_action}, {} };
            //  m_input_action_contexts.push_back(input_context);

            auto& world_state = m_engine_frontend->accessWorldState();
            auto& resource_manager = m_engine_frontend->accessResourceManager();
            auto& frame_manager = m_engine_frontend->accessFrameManager();

            //  Editor::Controls::CameraController cam_ctrl(world_state, frame_manager);
            //  m_input_action_contexts.push_back(cam_ctrl.getInputActionContext());
            //m_engine_frontend->addInputActionContext(cam_ctrl.getInputActionContext());
            createDemoScene(world_state, resource_manager);

            while (render_exec.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
                // do nothing so far...
            }
        };

        auto engine_update_loop = [this] (std::shared_future<void> render_exec)
        {
            size_t update_frameID = 0;
            auto t_0 = std::chrono::high_resolution_clock::now();
            auto t_1 = std::chrono::high_resolution_clock::now();

            auto render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));
            while (render_exec_status != std::future_status::ready)
            {
                double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t_1 - t_0).count();
                t_0 = std::chrono::high_resolution_clock::now();

                auto window_resolution = this->getWindowResolution();

                m_engine_frontend->update(update_frameID++, dt, std::get<0>(window_resolution), std::get<1>(window_resolution));

                render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));

                t_1 = std::chrono::high_resolution_clock::now();
            }
        };

        auto engine_render_loop = [this]()
        {
            size_t render_frameID = 0;
            auto t_0 = std::chrono::high_resolution_clock::now();
            auto t_1 = std::chrono::high_resolution_clock::now();

            while (true)
            {
                double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t_1 - t_0).count();
                t_0 = std::chrono::high_resolution_clock::now();

                winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(
                    winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent
                );

                auto window_resolution = this->getWindowResolution();

                // temporarily clear render target here
                {
                    auto device_context = m_device_resources->GetD3DDeviceContext();

                    CD3D11_VIEWPORT viewport(
                        0.0, 0.0, (float)std::get<0>(window_resolution), (float)std::get<1>(window_resolution));
                    device_context->RSSetViewports(1, &viewport);

                    auto render_target_view = m_device_resources->GetRenderTargetView();
                    auto depth_stencil_view = m_device_resources->GetDepthStencilView();

                    const float clear_color[4] = { 1.0f, 0.2f, 0.4f, 1.0f };
                    const float clear_depth = 1.0f;

                    // Clear swapchain and depth buffer. NOTE: This will clear the entire render target view, not just the specified view.
                    device_context->ClearRenderTargetView(render_target_view, clear_color);
                    device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clear_depth, 0);
                    //device_context->OMSetDepthStencilState(reversedZ ? m_reversedZDepthNoStencilTest.get() : nullptr, 0);

                    ID3D11RenderTargetView* renderTargets[] = { render_target_view };
                    device_context->OMSetRenderTargets((UINT)std::size(renderTargets), renderTargets, depth_stencil_view);
                }

                m_engine_frontend->render(render_frameID++, dt, std::get<0>(window_resolution), std::get<1>(window_resolution));

                m_device_resources->Present();

                t_1 = std::chrono::high_resolution_clock::now();
            }
        };

        // Deferred starting and running of rendering on current thread (window and context usually live here)
        auto engine_render_exec = std::async(std::launch::deferred, engine_render_loop).share();

        // Start and run engine update in seperate thread
        auto engine_update_exec = std::async(std::launch::async, engine_update_loop, engine_render_exec);

        // Start "game"
        auto game_update_exec = std::async(std::launch::async, game_update_loop, engine_render_exec);

        // trigger deferred start of render loop
        engine_render_exec.get();
    }

    std::pair<int, int> getWindowResolution() {
        //std::lock_guard<std::mutex> lk(m_window_mutex);
        return { m_pixel_width, m_pixel_height };
    }

private:
    void HandleWindowSizeChanged()
    {
        m_pixel_width = ConvertDipsToPixels(m_logical_width, m_DPI);
        m_pixel_height = ConvertDipsToPixels(m_logical_height, m_DPI);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation(m_current_orientation, m_native_orientation);

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(m_pixel_width, m_pixel_height);
        }

        //m_game->OnWindowSizeChanged(outputWidth, outputHeight, rotation);
        //TODO
    }

    std::shared_ptr<DX::DeviceResources>      m_device_resources;
    std::unique_ptr<DirectXEngineFrontendUWP> m_engine_frontend;

    bool  m_exit;
    bool  m_visible;
    bool  m_in_sizemove;
    float m_DPI;
    float m_logical_width;
    float m_logical_height;

    int m_pixel_width;
    int m_pixel_height;

    winrt::Windows::Graphics::Display::DisplayOrientations m_native_orientation;
    winrt::Windows::Graphics::Display::DisplayOrientations m_current_orientation;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::Windows::ApplicationModel::Core::CoreApplication::Run(winrt::make<App>());
}

// Window size helper
_Use_decl_annotations_
void GetWindowBounds(::IUnknown* window, RECT* rect)
{
    if (!rect)
        return;

    *rect = {};

    if (!window)
        return;

    winrt::Windows::UI::Core::CoreWindow cw{ nullptr };
    try {
        winrt::check_hresult(window->QueryInterface(winrt::guid_of<winrt::Windows::UI::Core::CoreWindow>(), winrt::put_abi(cw)));
    }
    catch (...) {
        return;
    }

    auto b = cw.Bounds();

    auto currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
    float dpi = currentDisplayInformation.LogicalDpi();

    const int x = ConvertDipsToPixels(b.X, dpi);
    const int y = ConvertDipsToPixels(b.Y, dpi);
    const int w = ConvertDipsToPixels(b.Width, dpi);
    const int h = ConvertDipsToPixels(b.Height, dpi);

    rect->left = static_cast<long>(x);
    rect->top = static_cast<long>(y);
    rect->right = static_cast<long>(x + w);
    rect->bottom = static_cast<long>(y + h);
}