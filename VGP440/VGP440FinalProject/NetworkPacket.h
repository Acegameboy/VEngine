#pragma once

#include <cstdint>

enum class AsteroidPacketType : uint32_t
{
    Join = 1,
    Welcome = 2
};

struct JoinPacket
{
    AsteroidPacketType type = AsteroidPacketType::Join;
};

struct WelcomePacket
{
    AsteroidPacketType type = AsteroidPacketType::Welcome;
    int32_t playerID = -1;
};