#pragma once
#include <Windows.h>
#include <print>

#include <api/imports.hpp>
#include <../src/runtime.hpp>

#include <event/events.hpp>
#include <api/helpers/mcuirc.hpp>
#include <api/mc/client/renderer/MeshHelpers.hpp>

#include "selaura_imgui.hpp"

BOOL APIENTRY DllMain(HINSTANCE, DWORD, LPVOID) {
    return TRUE;
}

void AfterUI(Selaura::SetupAndRenderEvent<Selaura::EventPhase::Post>& ev) {
    static bool state = false;
    if (!state) {
        SelauraImGui::Init(ev.mCtx, ev.mCtx->mScreenContext);
        state = true;
    }

    SelauraImGui::NewFrame();
    ImGui::NewFrame();

    ImGui::GetForegroundDrawList()->AddRectFilled({0, 0}, {100, 100}, ImColor(255, 0, 0));

    ImGui::EndFrame();
    ImGui::Render();
    SelauraImGui::RenderDrawData(ImGui::GetDrawData());
}

SELAURA_API void SelauraPluginInit(Selaura::Runtime* pRuntime) {
    pRuntime->mEventManager->subscribe(&AfterUI);
}