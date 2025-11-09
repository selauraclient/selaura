#include "windows.hpp"

namespace Selaura {
    WindowsPlatform::WindowsPlatform(HANDLE mcThreadHandle) {
        this->mcThreadHandle = mcThreadHandle;
    }

    PlatformType WindowsPlatform::GetPlatformType() const {
        return PlatformType::Windows;
    }

    void WindowsPlatform::PauseGameThread() const {
        SuspendThread(this->mcThreadHandle);
    }

    void WindowsPlatform::ResumeGameThread() const {
        ResumeThread(this->mcThreadHandle);
    }

    std::filesystem::path WindowsPlatform::GetSelauraFolder() const {
        PWSTR appDataPath = nullptr;
        if (FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath))) {
            throw std::runtime_error("Failed to get AppData path");
        }

        std::filesystem::path folder = appDataPath;
        CoTaskMemFree(appDataPath);

        folder /= R"(Minecraft Bedrock\Users\Shared\games\com.mojang\Selaura)";

        return folder;
    }

    void WindowsPlatform::InitConsole() const {
        AllocConsole();

        AttachConsole(GetCurrentProcessId());
        SetConsoleTitleA("Selaura Runtime Console");

        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(hOut, &mode)) {
                SetConsoleMode(hOut, mode | 0x0004);
            }
        }
    }

    std::span<std::byte> WindowsPlatform::GetMinecraftModule() const {
        const HMODULE handle = GetModuleHandleA("Minecraft.Windows.exe");
        if (!handle) return {};

        MODULEINFO moduleInfo;
        if (!GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo))) return {};
        return std::span<std::byte>{static_cast<std::byte*>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage};
    }
};