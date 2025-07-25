#pragma once
#include "Packet.hpp"
#include <spdlog/spdlog.h>

#include "../../../patcher.hpp"

template <MinecraftPacketIds id>
struct PacketHandlerDispatcherInstance_callbacks;

template <>
struct PacketHandlerDispatcherInstance_callbacks<MinecraftPacketIds::Text> {
    void handle(void* networkIdentifier, void* netEventCallback, const std::shared_ptr<Packet>& packet) {
        selaura::call_fn<&PacketHandlerDispatcherInstance_callbacks<MinecraftPacketIds::Text>::handle>(this, networkIdentifier, netEventCallback, packet);
    }
};