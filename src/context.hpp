#pragma once
#include <thread>

#include <api/mc/world/Minecraft.hpp>
#include <api/mc/client/ClientInstance.hpp>
#include <platform/platform.hpp>

namespace Selaura {
    struct SharedContext {
        Minecraft* mMinecraft;
    };

    struct ClientContext : SharedContext {
        ClientInstance* mClientInstance;
    };

    struct ServerContext : SharedContext {};

    struct RuntimeContext {
        std::thread::id mThreadId;
        int mVersionMajor;
        int mVersionMinor;
        int mVersionBuild;
        int mVersionRevision;

        ClientContext* mClientCtx;
        ServerContext* mServerCtx;
        std::unique_ptr<Platform> mPlatform;
    };
};