#include "MinecraftPackets.hpp"
#include <spdlog/spdlog.h>
#include "../../../patcher.hpp"
#include "../../../../client.hpp"

std::shared_ptr<Packet> MinecraftPackets::createPacket_hk(MinecraftPacketIds id) {
    return selaura::call_fn<&MinecraftPackets::createPacket_hk>(id);
}