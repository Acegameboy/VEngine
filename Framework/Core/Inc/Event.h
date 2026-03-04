#pragma once

namespace VEngine::Core
{
    using EventTypeId = std::size_t;

    class Event
    {
    public:
        Event() = default;
        virtual ~Event() = default;

        virtual EventTypeId GetTypeId() const = 0;
    };
}

#define SET_EVENT_TYPE_ID(id)\
    static VEngine::Core::EventTypeId StaticGetTypeId() { return static_cast<VEngine::Core::EventTypeId>(id); }\
    VEngine::Core::EventTypeId GetTypeId() const override { return StaticGetTypeId(); }