#pragma once

#include <cstdint>

constexpr uint32_t ASTEROIDS_PER_PACKET = 50;

enum class AsteroidPacketType : uint32_t
{
    Join = 1,
    Welcome,
    PlayerInput,
    GameState,
    AsteroidChunk,
    Restart
};

struct JoinPacket
{
    AsteroidPacketType type = AsteroidPacketType::Join;
};

struct WelcomePacket
{
    AsteroidPacketType type = AsteroidPacketType::Welcome;

    int32_t playerID = -1;
    uint32_t asteroidCount = 0;
};

struct PlayerInputPacket
{
    AsteroidPacketType type = AsteroidPacketType::PlayerInput;

    int32_t playerID = -1;

    // -1 = left
    //  0 = stop
    //  1 = right
    float movement = 0.0f;
};

struct NetworkPlayerState
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float survivalTime = 0.0f;

    int32_t alive = 1;
};

struct GameStatePacket
{
    AsteroidPacketType type = AsteroidPacketType::GameState;

    NetworkPlayerState players[2];

    int32_t matchStarted = 0;
    int32_t matchOver = 0;

    // 0 = no winner / tie
    // 1 = Player 1
    // 2 = Player 2
    int32_t winner = 0;
};

struct NetworkAsteroidState
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float velocityZ = 0.0f;

    float radius = 0.0f;

    uint32_t resetCount = 0;
};

struct AsteroidChunkPacket
{
    AsteroidPacketType type =
        AsteroidPacketType::AsteroidChunk;

    uint32_t startIndex = 0;
    uint32_t count = 0;

    NetworkAsteroidState asteroids[ASTEROIDS_PER_PACKET];
};

struct RestartPacket
{
    AsteroidPacketType type =
        AsteroidPacketType::Restart;
};