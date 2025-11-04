#include "Precompiled.h"
#include "RenderObject.h"
#include "ModelManager.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void RenderObject::Terminate()
{
    meshBuffer.Terminate();
    TextureManager* tm = TextureManager::Get();
    tm->ReleaseTexture(diffuseMapId);
    tm->ReleaseTexture(specMapId);
    tm->ReleaseTexture(normalMapId);
    tm->ReleaseTexture(bumpMapId);
}

void VEngine::Graphics::RenderGroup::Initialize(const std::filesystem::path& modelFilePath)
{
    modelId = ModelManager::Get()->LoadModel(modelFilePath);
    const Model* model = ModelManager::Get()->GetModel(modelId);
    ASSERT(model != nullptr, "RenderGroup: model %s did not load", modelFilePath.u8string().c_str());

    auto TryLoadTexture = [](const auto& textureName)->TextureId
    {
            if (textureName.empty())
            {
                return 0;
            }

            return TextureManager::Get()->LoadTexture(textureName, false);
    };

    for (const Model::MeshData& meshData : model->meshData)
    {
        RenderObject& renderObject = renderObjects.emplace_back();
        renderObject.meshBuffer.Initialize(meshData.mesh);
        if (meshData.materialIndex < model->materialData.size())
        {
            //add material data
            const Model::MaterialData& materialData = model->materialData[meshData.materialIndex];
            renderObject.material = materialData.material;

            renderObject.diffuseMapId = TryLoadTexture(materialData.diffuseMapName);
            renderObject.specMapId = TryLoadTexture(materialData.specMapName);
            renderObject.normalMapId = TryLoadTexture(materialData.normalMapName);
            renderObject.bumpMapId = TryLoadTexture(materialData.bumpMapName);
        }
    }
}

void VEngine::Graphics::RenderGroup::Terminate()
{
    for (RenderObject& renderObject : renderObjects)
    {
        renderObject.Terminate();
    }
    renderObjects.clear();
}