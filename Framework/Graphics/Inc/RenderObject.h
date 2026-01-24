#pragma once

#include "MeshBuffer.h"
#include "Transform.h"
#include "Material.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Animator.h"

namespace VEngine::Graphics
{
    struct Skeleton;

    class RenderObject
    {
    public:
        void Terminate();

        Transform transform;  // location/orientation
        MeshBuffer meshBuffer;// shape
        Material material;    // light data
        TextureId diffuseMapId; //diffuse texture for an object
        TextureId specMapId;  //specular map for an object
        TextureId normalMapId; //normal texture for an object
        TextureId bumpMapId;   //bump texture for an object
    };

    class RenderGroup
    {
    public:
        void Initialize(const std::filesystem::path& modelFilePath);
        void Terminate();

        ModelId modelId;
        Transform transform;
        std::vector<RenderObject> renderObjects;
        const Skeleton* skeleton = nullptr;
        const Animator* animator = nullptr;
    };
}