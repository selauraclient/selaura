#pragma once
#include <memory>
#include "Packet.hpp"

struct MinecraftPackets {
    static std::shared_ptr<Packet> createPacket_hk(MinecraftPacketIds id);
};
