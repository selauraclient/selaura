#pragma once
#include <filesystem>
#include <span>

namespace Selaura {
    enum class PlatformType : int {
        Windows = 0
    };

    struct Platform {
        virtual ~Platform() = default;
        virtual PlatformType GetPlatformType() const = 0;

        virtual void PauseGameThread() const = 0;
        virtual void ResumeGameThread() const = 0;

        virtual std::filesystem::path GetSelauraFolder() const = 0;

        virtual void InitConsole() const = 0;

        virtual std::span<std::byte> GetMinecraftModule() const = 0;
    };
};