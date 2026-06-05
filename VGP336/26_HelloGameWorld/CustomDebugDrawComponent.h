#pragma once

#include "CustomTypeIds.h"
#include <VEngine/Inc/VEngine.h>

class CustomDebugDrawComponent : public VEngine::Component
{
public:
    SET_TYPE_ID(CustomComponentId::CustomDebugDraw);

    void Initialize() override;

    void Terminate() override;

    void DebugUI() override;

    void Deserialize(const rapidjson::Value& value) override;

    void AddDebugDraw() const;

private:
    const VEngine::TransformComponent* mTransformComponent = nullptr;
    VEngine::Math::Vector3 mPosition = VEngine::Math::Vector3::Zero;
    VEngine::Color mColor = VEngine::Colors::White;

    uint32_t mSlices = 0;
    uint32_t mRings = 0;
    float mRadius = 0;
};