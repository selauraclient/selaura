#include "runtime.hpp"

#include <print>

#include "hooks/hooks.hpp"

namespace Selaura {
    Runtime::Runtime(const RuntimeContext& ctx) {
        std::println("[Selaura Runtime] Created runtime.");
        std::println("[Selaura Runtime] Running on Minecraft Version: {}.{}.{}.{}",
            ctx.mVersionMajor,
            ctx.mVersionMinor,
            ctx.mVersionBuild,
            ctx.mVersionRevision);

        this->mThreadId = ctx.mThreadId;

        this->mClientCtx = std::make_unique<ClientContext>();
        this->mServerCtx = std::make_unique<ServerContext>();
        this->mEventManager = std::make_unique<EventManager>();
    }

    void Runtime::start(){
        Selaura::InitHooks();
    }
};