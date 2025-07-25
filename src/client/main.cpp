#include <thread>

#include "client.hpp"
#include "platform/platform.hpp"


void init() {
#ifdef SELAURA_WINDOWS
    AllocConsole();

    AttachConsole(GetCurrentProcessId());
    SetConsoleTitleA("Selaura Client Console");

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
#endif
    static auto instance = std::make_shared<selaura::client>();
    instance->init();
}

#ifdef SELAURA_WINDOWS
BOOL APIENTRY DllMain(HMODULE hmodule, DWORD dw_reason, LPVOID lp_reserved) {
    if (dw_reason == DLL_PROCESS_ATTACH) {
        const auto& handle = selaura::get_dynamic_module("Minecraft.Windows.exe");
        if (!handle.valid()) return false;
        DisableThreadLibraryCalls(static_cast<HMODULE>(handle.native_handle));

        std::thread(&init).detach();
    }
    return true;
}
#endif

#ifdef SELAURA_LINUX
extern "C" [[gnu::visibility("default")]] void mod_init() {
    std::thread(&init).detach();
}
#endif