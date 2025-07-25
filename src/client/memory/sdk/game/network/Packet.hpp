#pragma once

enum class MinecraftPacketIds : int {
    Text = 0x9
};

struct PacketHandlerDispatcherInstance {};

struct Packet {
    std::byte pad[0x20];
    PacketHandlerDispatcherInstance* handler;
    std::byte pad2[0x8];
};