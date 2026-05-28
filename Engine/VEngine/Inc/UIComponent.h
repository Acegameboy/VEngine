#pragma once

#include "Component.h"

namespace VEngine
{
    class UIComponent : public Component
    {
    public:
        virtual void Render() = 0;
    };
}