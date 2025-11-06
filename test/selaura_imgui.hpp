#pragma once
#include <api/mc/client/renderer/MeshHelpers.hpp>
#include <api/mc/client/renderer/MinecraftUIRenderContext.hpp>

#include "imgui/imgui.h"

struct SelauraImGui_State {
    MinecraftUIRenderContext* mcuirc;
    ScreenContext* scrctx;
    Tessellator* tessellator;
};

namespace SelauraImGui {
    inline SelauraImGui_State state{};

    inline mce::MaterialPtr* GetMaterial() {
        static mce::MaterialPtr* mat = nullptr;
        if (!mat) mat = mce::MaterialPtr::createMaterial(HashedString("ui_fill_color"));
        return mat;
    };

    inline void SetClip(MinecraftUIRenderContext* mcuirc, RectangleArea& area) {
        mcuirc->setClippingRectangle(area);
        mcuirc->enableScissorTest(area);
    }

    inline void PushClip(MinecraftUIRenderContext* mcuirc) {
        mcuirc->saveCurrentClippingRectangle();
    }

    inline void PopClip(MinecraftUIRenderContext* mcuirc) {
        mcuirc->restoreSavedClippingRectangle();
        mcuirc->disableScissorTest();
    }

    inline void LoadFontTexture(MinecraftUIRenderContext* ctx) {
        unsigned char* pixels;
        int width, height, bytesPerPixel;

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);
    }

    inline bool Init(MinecraftUIRenderContext* ctx, ScreenContext* scrn) {
        if (ImGui::GetCurrentContext() == nullptr) ImGui::CreateContext();
        IMGUI_CHECKVERSION();

        state.mcuirc = ctx;
        ImGuiIO& io = ImGui::GetIO();
        io.BackendRendererName = "imgui_impl_selaura";
        io.IniFilename = nullptr;

        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::GetStyle().AntiAliasedFill = false;

        state.scrctx = scrn;
        state.tessellator = scrn->tessellator;

        LoadFontTexture(ctx);

        return true;
    }

    inline void NewFrame() {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)800, (float)600);
    }

    inline void RenderDrawData(ImDrawData* drawData) {
        if (!state.mcuirc || drawData->CmdListsCount == 0) return;
        auto mat = GetMaterial();

        //ctx->saveCurrentClippingRectangle();
        ImVec2 clip_off = drawData->DisplayPos;
        ImVec2 clip_scale = drawData->FramebufferScale;

        for (int n = 0; n < drawData->CmdListsCount; n++) {
            const ImDrawList* cmd_list = drawData->CmdLists[n];
            const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
            const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback) continue;

                //mce::TexturePtr* texPtr = reinterpret_cast<mce::TexturePtr*>(pcmd->GetTexID());

                state.tessellator->begin(mce::PrimitiveMode::TriangleList, 0);
                for (unsigned int idx = 0; idx < pcmd->ElemCount; idx += 3) {
                    const ImDrawIdx i0 = idx_buffer[pcmd->IdxOffset + idx + 2];
                    const ImDrawIdx i1 = idx_buffer[pcmd->IdxOffset + idx + 1];
                    const ImDrawIdx i2 = idx_buffer[pcmd->IdxOffset + idx + 0];
                    const ImDrawVert& v0 = vtx_buffer[pcmd->VtxOffset + i0];
                    const ImDrawVert& v1 = vtx_buffer[pcmd->VtxOffset + i1];
                    const ImDrawVert& v2 = vtx_buffer[pcmd->VtxOffset + i2];

                    auto colToFloat = [](unsigned int c){
                        return ImVec4(
                            (float)((c)&0xFF)/255.f,
                            (float)((c>>8)&0xFF)/255.f,
                            (float)((c>>16)&0xFF)/255.f,
                            (float)((c>>24)&0xFF)/255.f
                        );
                    };

                    ImVec4 c0 = colToFloat(v0.col);
                    ImVec4 c1 = colToFloat(v1.col);
                    ImVec4 c2 = colToFloat(v2.col);

                    const float x0 = (v0.pos.x - clip_off.x) * clip_scale.x;
                    const float y0 = (v0.pos.y - clip_off.y) * clip_scale.y;
                    const float x1 = (v1.pos.x - clip_off.x) * clip_scale.x;
                    const float y1 = (v1.pos.y - clip_off.y) * clip_scale.y;
                    const float x2 = (v2.pos.x - clip_off.x) * clip_scale.x;
                    const float y2 = (v2.pos.y - clip_off.y) * clip_scale.y;

                    state.tessellator->color(c0.x, c0.y, c0.z, c0.w);
                    state.tessellator->vertexUV(x0, y0, 0.0f, v0.uv.x, v0.uv.y);
                    state.tessellator->color(c1.x, c1.y, c1.z, c1.w);
                    state.tessellator->vertexUV(x1, y1, 0.0f, v1.uv.x, v1.uv.y);
                    state.tessellator->color(c2.x, c2.y, c2.z, c2.w);
                    state.tessellator->vertexUV(x2, y2, 0.0f, v2.uv.x, v2.uv.y);

                }
            }
        }

        char pad[0x58]{};
        MeshHelpers::renderMeshImmediately(state.scrctx, state.tessellator, mat, pad);
        //ctx->restoreSavedClippingRectangle();


    }
};