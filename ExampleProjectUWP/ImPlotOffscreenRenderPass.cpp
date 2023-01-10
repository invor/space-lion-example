#include "ImPlotOffscreenRenderPass.h"

#include "MaterialComponentManager.hpp"
#include "NameComponentManager.hpp"
#include "RenderTaskComponentManager.hpp"

#include <imgui.h>
#include <examples/imgui_impl_dx11.h>
#include "imgui_impl_uwp.h"

void Graphics::Dx11::addImPlotOffscreenRenderPass(EngineCore::Graphics::Dx11::Frame& frame, EngineCore::WorldState& world_state, EngineCore::Graphics::Dx11::ResourceManager& resource_mngr)
{
    struct ImPlotOffscreenPassData
    {

    };

    struct ImPlotOffscreenPassResources
    {
        ID3D11Device4* d3d11_device;
        ID3D11DeviceContext4* d3d11_device_context;

        EngineCore::Graphics::WeakResource<dxowl::RenderTarget> render_target;
    };

    frame.addRenderPass<ImPlotOffscreenPassData, ImPlotOffscreenPassResources>("ImPlotOffscreenPass",
        [](ImPlotOffscreenPassData& data, ImPlotOffscreenPassResources& resources)
        {
        },
        [&resource_mngr](ImPlotOffscreenPassData& data, ImPlotOffscreenPassResources& resources)
        {
            // obtain access to device resources
            resources.d3d11_device = resource_mngr.getD3D11Device();
            resources.d3d11_device_context = resource_mngr.getD3D11DeviceContext();

            // try to get render target object
            resources.render_target = resource_mngr.getRenderTarget("ImPlotProxyRenderTarget");

            if (resources.render_target.id == EngineCore::Graphics::Dx11::ResourceManager::invalidResourceID())
            {
                D3D11_TEXTURE2D_DESC texture_desc;
                D3D11_RENDER_TARGET_VIEW_DESC rndr_tgt_view_desc;
                D3D11_SHADER_RESOURCE_VIEW_DESC shdr_rsrc_view_desc;

                // Initialize the render target texture description.
                ZeroMemory(&texture_desc, sizeof(texture_desc));

                // Setup the render target texture description.
                texture_desc.Width = 512;
                texture_desc.Height = 512;
                texture_desc.MipLevels = 1;
                texture_desc.ArraySize = 1;
                texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                texture_desc.SampleDesc.Count = 1;
                texture_desc.Usage = D3D11_USAGE_DEFAULT;
                texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
                texture_desc.CPUAccessFlags = 0;
                texture_desc.MiscFlags = 0;

                // Setup the description of the render target view.
                rndr_tgt_view_desc.Format = texture_desc.Format;
                rndr_tgt_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                rndr_tgt_view_desc.Texture2D.MipSlice = 0;

                // Setup the description of the shader resource view.
                shdr_rsrc_view_desc.Format = texture_desc.Format;
                shdr_rsrc_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                shdr_rsrc_view_desc.Texture2D.MostDetailedMip = 0;
                shdr_rsrc_view_desc.Texture2D.MipLevels = 1;

                resource_mngr.createRenderTargetAsync("ImPlotProxyRenderTarget", texture_desc, shdr_rsrc_view_desc, rndr_tgt_view_desc);
            }

        },
        [&world_state,&resource_mngr](ImPlotOffscreenPassData const& data, ImPlotOffscreenPassResources const& resources)
        {
            // obtain context for rendering
            const auto device_context = resources.d3d11_device_context;
            const auto device = resources.d3d11_device;

            // create temporary imgui context
            auto orig_imgui_ctx = ImGui::GetCurrentContext();
            auto imgui_ctx = ImGui::CreateContext(ImGui::GetFont()->ContainerAtlas);
            ImGui::SetCurrentContext(imgui_ctx);
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui_ImplUWP_Init();
            ImGui_ImplDX11_Init(device, device_context);

            if (resources.render_target.state == EngineCore::Graphics::READY)
            {
                auto render_target_texture_desc = resources.render_target.resource->getTextureDesc();

                // clear render target here
                {
                    CD3D11_VIEWPORT viewport(
                        0.0, 0.0, (float)render_target_texture_desc.Width, (float)render_target_texture_desc.Height);
                    device_context->RSSetViewports(1, &viewport);

                    const float clear_color[4] = { 0.5f, 0.2f, 0.4f, 1.0f };
                    const float clear_depth = 1.0f;

                    // Clear swapchain and depth buffer. NOTE: This will clear the entire render target view, not just the specified view.
                    device_context->ClearRenderTargetView(resources.render_target.resource->getRenderTargetView().Get(), clear_color);

                    ID3D11RenderTargetView* renderTargets[] = { resources.render_target.resource->getRenderTargetView().Get() };
                    device_context->OMSetRenderTargets((UINT)std::size(renderTargets), renderTargets, NULL);
                }

                ImGui_ImplDX11_NewFrame();
                ImGui_ImplUWP_NewFrame(512, 512);
                ImGui::NewFrame();

                // create imgui/implot elements
                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();

                ImGui::Render();
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                ImGui::EndFrame();

                //TODO copy rendering result to material texture of target entities

            //TODO get destination texture
                auto texture_resource = resource_mngr.getTexture2DResource("spatial_window_texture");

                if (texture_resource.state == EngineCore::Graphics::READY)
                {
                    D3D11_BOX src_region;
                    src_region.left = 0;
                    src_region.right = 512;
                    src_region.top = 0;
                    src_region.bottom = 512;
                    src_region.front = 0;
                    src_region.back = 1;

                    ID3D11Resource* src_rsrc;
                    resources.render_target.resource->getShaderResourceView()->GetResource(&src_rsrc);
                    ID3D11Resource* dest_rsrc;
                    texture_resource.resource->getShaderResourceView()->GetResource(&dest_rsrc);
                    device_context->CopySubresourceRegion(dest_rsrc, 0, 0, 0, 0, src_rsrc, 0, &src_region);
                }
            }

            // delete temporary imgui context, reset to original imgui context
            ImGui::DestroyContext(imgui_ctx);
            ImGui::SetCurrentContext(orig_imgui_ctx);

        }
        );
}
