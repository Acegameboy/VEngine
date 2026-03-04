#pragma once

#include <VEngine/Inc/VEngine.h>

enum class GameEventType
{
    PressSpace = 1,

    PressEnter
};

class PressSpaceEvent : public VEngine::Core::Event
{
public:
    PressSpaceEvent() {}
    SET_EVENT_TYPE_ID(GameEventType::PressSpace)
};

class PressEnterEvent : public VEngine::Core::Event
{
public:
    PressEnterEvent() {}
    SET_EVENT_TYPE_ID(GameEventType::PressEnter)
};
