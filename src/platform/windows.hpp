#pragma once

#ifdef WIN32
#include <Windows.h>
#include <Psapi.h>
#include <shlobj_core.h>

#include "platform.hpp"

namespace Selaura {
    struct WindowsPlatform : public Platform {
        explicit WindowsPlatform(HANDLE mcThreadHandle);

        [[nodiscard]] PlatformType GetPlatformType() const override;
        void PauseGameThread() const override;
        void ResumeGameThread() const override;
        [[nodiscard]] std::filesystem::path GetSelauraFolder() const override;
        void InitConsole() const override;
        [[nodiscard]] std::span<std::byte> GetMinecraftModule() const override;
    private:
        HANDLE mcThreadHandle;
    };
};
#endif